
// STD
#include <iostream>
#include <limits>

// Qt
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QSpinBox>

// Project
#include "ui_GraphToolsWidget.h"
#include "GraphToolsWidget.h"
#include "GraphTools.h"

namespace GT
{

//******************************************************************************

QList<QColor> getColorPanel()
{
    QList<QColor> o = QList<QColor>()
            << Qt::red
            << Qt::yellow
            << Qt::green
            << Qt::blue
            << Qt::cyan
            << Qt::magenta
            << Qt::darkRed
            << Qt::darkGreen
            << Qt::darkBlue
            << Qt::darkCyan
            << Qt::darkMagenta
            << Qt::darkYellow
            << Qt::darkGray
            << Qt::gray
            << Qt::lightGray;
    return o;
}

void removeItem(QGraphicsItem * path)
{
    path->scene()->removeItem(path);
    delete path;
}

//******************************************************************************

GraphToolsWidget::GraphToolsWidget(QWidget *parent) :
    GraphViewer(parent),
    ui(new Ui::GraphToolsWidget),
    _isChooseVertexMode(false),
    _chooseSender(0),
    _path(0)
{
    setWindowTitle(tr("Graph Tools App"));

    QWidget * w = new QWidget();
    ui->setupUi(w);
    layout()->addWidget(w);

    // connect
    connect(ui->_clear, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->_runGGC, SIGNAL(clicked()), this, SLOT(runGGC()));
    connect(ui->_runMVD, SIGNAL(clicked()), this, SLOT(runMVD()));
    connect(ui->_cleanMVD, SIGNAL(clicked()), this, SLOT(cleanMVD()));
    connect(ui->_runCCV, SIGNAL(clicked()), this, SLOT(runCCV()));
    connect(ui->_chooseSVId, SIGNAL(clicked()), this, SLOT(onChooseVertexId()));
    connect(ui->_chooseEVId, SIGNAL(clicked()), this, SLOT(onChooseVertexId()));

    clear();

    resize(500,600);
}

//******************************************************************************

void GraphToolsWidget::clear()
{
    if (_path) {
        removeItem(_path);
        _path=0;
    }

    _chooseSender=0;
    _isChooseVertexMode=false;
    ui->_startVertexId->setValue(0);
    ui->_endVertexId->setValue(0);
    ui->_distance->setText("");
    ui->_chooseSVId->setDown(_isChooseVertexMode);
    ui->_chooseEVId->setDown(_isChooseVertexMode);

    GraphViewer::clear();

}

//******************************************************************************

void GraphToolsWidget::runGGC()
{
    // setup graph data
    GT::Graph graph;
    if (!setupGraph(&graph))
    {
        return;
    }

    // Apply greedy graph coloring algorithm
    GT::GreedyGraphColoring(&graph);

    // Show results
    QList<QColor> colorPanel = getColorPanel();
    for (int i=0; i<qMin(graph.vertices.size(), _vertices.size());i++)
    {
        int colorLabel = graph.vertices[i].color;
        QColor color;
        if (colorLabel >= colorPanel.size())
            color = QColor(colorLabel,colorLabel,colorLabel);
        else
            color = colorPanel[colorLabel];
        _vertices[i]->setBrush(color);
    }

}

//******************************************************************************

void GraphToolsWidget::runMVD()
{

    int startVertexId = ui->_startVertexId->value()-1;
    int endVertexId = ui->_endVertexId->value()-1;
    if (startVertexId < 0 || endVertexId < 0)
        return;

    // setup graph data
    GT::Graph graph;
    if (!setupGraph(&graph))
    {
        return;
    }

    // Apply minimal distance computation
    QList<int> path;
    double distance = GT::ComputeMinDistance(graph, startVertexId, endVertexId, &path);


    // Display the result:
    if (distance < -12344.0)
    {
        ui->_distance->setText(QString("Path can not be found").arg(distance));
        return;
    }
    else if (distance == std::numeric_limits<double>::max())
    {
        ui->_distance->setText(QString("No path between vertices"));
        return;
    }

    ui->_distance->setText(QString("%1").arg(distance));

#ifdef _DEBUG
    std::cout << "Shortest path : ";
    foreach (int index, path)
    {
        std::cout << index + 1 << " -> ";
    }
    std::cout << " || " << std::endl;
#endif


    // draw path :
    if (_path) {
        removeItem(_path);
        _path=0;
    }
    _path = new QGraphicsItemGroup();
    _scene.addItem(_path);

    for (int i=0; i<path.size()-1;i++)
    {
        int pvi1=path[i];
        int pvi2=path[i+1];

        if (pvi1 < 0 || pvi1 > _vertices.size()-1 ||
                pvi2 < 0 || pvi2 > _vertices.size()-1)
        {
            std::cerr << "Failed to find drawn vertices" << std::endl;
            break;
        }

        QGraphicsLineItem* line = new QGraphicsLineItem(
                    _vertices[pvi1]->scenePos().x(),
                    _vertices[pvi1]->scenePos().y(),
                    _vertices[pvi2]->scenePos().x(),
                    _vertices[pvi2]->scenePos().y()
                    );
        line->setPen(QPen(Qt::red,VERTEX_SIZE*0.05));
        _path->addToGroup(line);
    }
    _path->setZValue(PATH_LINE_Z);

}

//******************************************************************************

void clearVertexOverlay(QGraphicsItem * vertex)
{
    QGraphicsScene * scene = vertex->scene();
    foreach (QGraphicsItem* item, vertex->childItems())
    {
        QGraphicsEllipseItem * overlay = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if (overlay && overlay->data(0).toString() == "overlay")
        {
            scene->removeItem(overlay);
        }
    }
}

void GraphToolsWidget::cleanMVD()
{
    if (_path) {
        removeItem(_path);
        _path=0;
    }

    // clean overlays:
    int index;
    if (ui->_startVertexId->value() > 0)
    {
        index = ui->_startVertexId->value() - 1;
        QGraphicsEllipseItem * vertex = _vertices[index];
        clearVertexOverlay(vertex);
    }

    if (ui->_endVertexId->value() > 0)
    {
        index = ui->_endVertexId->value() - 1;
        QGraphicsEllipseItem * vertex = _vertices[index];
        clearVertexOverlay(vertex);
    }
}

//******************************************************************************

void GraphToolsWidget::runCCV()
{

    // setup graph data
    GT::Graph graph;
    if (!setupGraph(&graph))
    {
        return;
    }

    // Apply greedy graph coloring algorithm
    QVector< QVector<Vertex*> > connectedVertices;
    if (!GT::ColorConnectedVertices(graph, &connectedVertices))
    {
        std::cerr << "Algorithm to color connected vertices is failed" << std::endl;
        return;
    }

    std::cout << "Number of sets of connected vertices : " << connectedVertices.size() << std::endl;

    // Show results
    QList<QColor> colorPanel = getColorPanel();
    for (int i=0; i<qMin(graph.vertices.size(), _vertices.size());i++)
    {
        int colorLabel = graph.vertices[i].color;
        QColor color;
        if (colorLabel >= colorPanel.size() && colorLabel < 256)
            color = QColor(colorLabel,colorLabel,colorLabel);
        else if (colorLabel >= 0 && colorLabel < colorPanel.size())
            color = colorPanel[colorLabel];
        else
            color = QColor(Qt::white);
        _vertices[i]->setBrush(color);
    }

}

//******************************************************************************

void GraphToolsWidget::onChooseVertexId()
{
    if (_path) {
        removeItem(_path);
        _path=0;
    }

    QToolButton * tb = qobject_cast<QToolButton*>(sender());
    if (tb)
    {
        // disable previous button if ChooseVertexMode is already is on
        if (_isChooseVertexMode)
        {
            ui->_chooseSVId->setDown(false);
            ui->_chooseEVId->setDown(false);
        }

        _isChooseVertexMode=true;

        // clear previous result:
        int index = -1;
        if (ui->_chooseSVId == tb && ui->_startVertexId->value() > 0)
        {
            index = ui->_startVertexId->value() - 1;
        }
        else if (ui->_chooseEVId == tb && ui->_endVertexId->value() > 0)
        {
            index = ui->_endVertexId->value() - 1;
        }
        if (index >= 0 && index < _vertices.size())
        {
            QGraphicsEllipseItem * vertex = _vertices[index];
            clearVertexOverlay(vertex);
        }

        tb->setDown(true);
        _chooseSender=tb;
    }
}

//******************************************************************************

bool GraphToolsWidget::eventFilter(QObject * object, QEvent * event)
{
    if (&_scene == object)
    {

        if (event->type() == QEvent::GraphicsSceneMouseDoubleClick)
        {
            QGraphicsSceneMouseEvent * mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QGraphicsItem * item = _scene.itemAt(mouseEvent->scenePos(), QTransform());
            QGraphicsSimpleTextItem* text = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
            // Choose vertex as start or end
            if (qgraphicsitem_cast<QGraphicsEllipseItem*>(item) || text)
            {
//                QGraphicsItem * item = _scene.itemAt(mouseEvent->scenePos(), QTransform());
                QGraphicsEllipseItem* vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(item->parentItem());
                if (!vertex)
                    vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);

                if (vertex && _isChooseVertexMode)
                {
                    QRectF r=vertex->rect().adjusted(-VERTEX_SIZE*0.05,
                                                     -VERTEX_SIZE*0.05,
                                                     VERTEX_SIZE*0.05,
                                                     VERTEX_SIZE*0.05);
                    QGraphicsEllipseItem* overlay = _scene.addEllipse(r,QPen(Qt::black,0));
                    overlay->setData(0,"overlay");
                    overlay->setZValue(vertex->zValue()-1);
                    overlay->setParentItem(vertex);
                    QGraphicsSimpleTextItem * text = _scene.addSimpleText("");
                    text->setParentItem(overlay);
                    text->setPen(QPen(Qt::blue,0));
                    text->setTransform(
                                QTransform::fromScale(0.005, 0.005)
                                * QTransform::fromTranslate(-0.5*r.width(),r.height()*0.51)
                                );


                    // set value to UI:
                    if (_chooseSender == ui->_chooseSVId)
                    {
                        ui->_chooseSVId->setDown(false);
                        ui->_startVertexId->setValue(vertex->data(KEY_VERTEX_ID).toInt()+1);
                        overlay->setPen(QPen(Qt::darkBlue,0));
                        text->setText("Start");
                    }
                    else if (_chooseSender == ui->_chooseEVId)
                    {
                        ui->_chooseEVId->setDown(false);
                        ui->_endVertexId->setValue(vertex->data(KEY_VERTEX_ID).toInt()+1);
                        overlay->setPen(QPen(Qt::darkYellow,0));
                        text->setText("End");
                    }
                    _isChooseVertexMode=false;
                    _chooseSender=0;

                }

            }
        }
    }
    return GraphViewer::eventFilter(object, event);
}

//******************************************************************************

GraphToolsWidget::~GraphToolsWidget()
{
    delete ui;
}

//******************************************************************************

}
