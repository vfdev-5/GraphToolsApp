
// Qt
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>

// Project
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GreedyGraphColoringTools.h"

//******************************************************************************

double VERTEX_SIZE=0.1;

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

//******************************************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _initialText(0),
    _drawingEdge(0),
    _isDrawingEdge(false)
{
    ui->setupUi(this);
    // connect
    connect(ui->_clear, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->_run, SIGNAL(clicked()), this, SLOT(run()));

    // setup scene:
    _scene.setSceneRect(0.0, 0.0, 1.0, 1.0);
    _scene.installEventFilter(this);

    clear();

    // setup view
    ui->_graphView->setScene(&_scene);

}

//******************************************************************************

void MainWindow::clear()
{
    _scene.clear();
    _vertices.clear();
    _edges.clear();

    _initialText = _scene.addSimpleText("Click here to add a vertex");
    _initialText->setPen(QColor(167,167,167));
    _initialText->setScale(0.005);
    _initialText->setPos(0, 0.5);
    _initialText->setOpacity(0.3);

}

//******************************************************************************

void MainWindow::run()
{
    // setup graph data
    Graph graph;
    graph.vertices.resize(_vertices.size());
    for (int i=0;i<graph.vertices.size();i++)
    {
        graph.vertices[i].id = _vertices[i]->data(0).toInt();
    }

    foreach ( QGraphicsLineItem * edge, _edges)
    {
        int vertexIndex1 = edge->data(0).toInt();
        int vertexIndex2 = edge->data(1).toInt();
        if (!graph.edges.contains(vertexIndex1))
        {
            QList<Vertex*> vts = QList<Vertex*>() << &graph.vertices[vertexIndex2];
            graph.edges.insert(vertexIndex1, vts);
        }
        else
        {
            QList<Vertex*> & vts = graph.edges[vertexIndex1];
            vts << &graph.vertices[vertexIndex2];
        }

        if (!graph.edges.contains(vertexIndex2))
        {
            QList<Vertex*> vts = QList<Vertex*>() << &graph.vertices[vertexIndex1];
            graph.edges.insert(vertexIndex2, vts);
        }
        else
        {
            QList<Vertex*> & vts = graph.edges[vertexIndex2];
            vts << &graph.vertices[vertexIndex1];
        }

    }

    // Apply greedy graph coloring algorithm
    GreedyGraphColoring(&graph);

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

bool MainWindow::eventFilter(QObject * object, QEvent * event)
{
    if (&_scene == object)
    {
        if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            if (_vertices.isEmpty())
                _initialText->setVisible(false);

            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);

            if (_scene.items(
                        me->scenePos().x() - VERTEX_SIZE*0.5,
                        me->scenePos().y() - VERTEX_SIZE*0.5,
                        VERTEX_SIZE,
                        VERTEX_SIZE
                        ).isEmpty())
            {
                QGraphicsEllipseItem * vertex = _scene.addEllipse(
                            QRectF(-VERTEX_SIZE*0.5, -VERTEX_SIZE*0.5, VERTEX_SIZE, VERTEX_SIZE),
                            QPen(),
                            QBrush(Qt::white)
                            );
                vertex->setTransform(
                            QTransform::fromTranslate(me->scenePos().x(), me->scenePos().y())
                            );
                vertex->setZValue(0.0);
                _vertices << vertex;
                int id = _vertices.size()-1;
                vertex->setData(0, id);
                QGraphicsSimpleTextItem * vertexId = _scene.addSimpleText(QString("%1").arg(id+1));
                vertexId->setParentItem(vertex);
                vertexId->setTransform(
                            QTransform::fromScale(0.005, 0.005)
                            * QTransform::fromTranslate(-VERTEX_SIZE*( (id < 10) ? 0.15 : 0.25 ), -VERTEX_SIZE*0.30)
                            );

            }
            else
            {

                if (qgraphicsitem_cast<QGraphicsEllipseItem*>(_scene.itemAt(me->scenePos()))
                        || qgraphicsitem_cast<QGraphicsSimpleTextItem*>(_scene.itemAt(me->scenePos())))
                {
                    QGraphicsEllipseItem* vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(_scene.itemAt(me->scenePos())->parentItem());
                    if (!vertex)
                        vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(_scene.itemAt(me->scenePos()));

                    _isDrawingEdge=true;
                    _drawingEdge = _scene.addLine(vertex->scenePos().x(), vertex->scenePos().y(), me->scenePos().x(), me->scenePos().y());
                    _drawingEdge->setZValue(1.0);
                    _drawingEdge->setData(0, vertex->data(0));

                }
            }
        }
        else if (event->type() == QEvent::GraphicsSceneMouseMove && _isDrawingEdge)
        {
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);
            QLineF line = _drawingEdge->line();
            _drawingEdge->setLine(line.x1(),line.y1(), me->scenePos().x(), me->scenePos().y());
        }
        else if (event->type() == QEvent::GraphicsSceneMouseRelease && _isDrawingEdge)
        {
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);

            // need to put the connecting line on the background otherwise it is detected under the mouse
            _drawingEdge->setZValue(-1.0);

            if (qgraphicsitem_cast<QGraphicsEllipseItem*>(_scene.itemAt(me->scenePos()))
                    || qgraphicsitem_cast<QGraphicsSimpleTextItem*>(_scene.itemAt(me->scenePos())))
            {
                QGraphicsEllipseItem* vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(_scene.itemAt(me->scenePos())->parentItem());
                if (!vertex)
                    vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(_scene.itemAt(me->scenePos()));

                QLineF line = _drawingEdge->line();
                _drawingEdge->setLine(line.x1(),line.y1(), vertex->scenePos().x(), vertex->scenePos().y());
                _drawingEdge->setZValue(-1.0);

                if (_drawingEdge->data(0) == vertex->data(0))
                    _scene.removeItem(_drawingEdge);
                else
                {
                    _drawingEdge->setData(1, vertex->data(0));
                    _edges << _drawingEdge;
                }
            }
            else
            {
                _scene.removeItem(_drawingEdge);
            }
            _isDrawingEdge=false;
            _drawingEdge=0;
        }

    }
    return QMainWindow::eventFilter(object, event);
}

//******************************************************************************

void MainWindow::showEvent(QShowEvent *e)
{
    ui->_graphView->fitInView(_scene.sceneRect(), Qt::KeepAspectRatio);
}

//******************************************************************************

MainWindow::~MainWindow()
{
    delete ui;
}

//******************************************************************************
