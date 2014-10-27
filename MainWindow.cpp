
// Qt
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QSpinBox>

// Project
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GraphTools.h"

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

//******************************************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _initialText(0),
    _drawingEdge(0),
    _isDrawingEdge(false),
    _editedItem(0),
    _isChooseVertexMode(false),
    _chooseSender(0)
{
    ui->setupUi(this);
    // connect
    connect(ui->_clear, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->_runGGC, SIGNAL(clicked()), this, SLOT(runGGC()));
    connect(ui->_runMVD, SIGNAL(clicked()), this, SLOT(runMVD()));
    connect(ui->_chooseSVId, SIGNAL(clicked()), this, SLOT(onChooseVertexId()));
    connect(ui->_chooseEVId, SIGNAL(clicked()), this, SLOT(onChooseVertexId()));



    // setup value editor
    _valueEditor.setWindowFlags(Qt::Popup);
    _valueEditor.installEventFilter(this);
    _valueEditor.setAlignment(Qt::AlignRight);
    connect(&_valueEditor, SIGNAL(returnPressed()), this, SLOT(onValueEdited()));

    // setup scene:
    _scene.setSceneRect(0.0, 0.0, 1.0, 1.0);
    _scene.installEventFilter(this);

    clear();

    // setup view
    ui->_graphView->setScene(&_scene);
    ui->_graphView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

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

    _editedItem=0;
    _chooseSender=0;
    _isChooseVertexMode=false;
    ui->_startVertexId->setValue(0);
    ui->_endVertexId->setValue(0);
    ui->_distance->setText("");

}

//******************************************************************************

void MainWindow::runGGC()
{
    // setup graph data
    gt::Graph graph;
    if (!setupGraph(&graph))
    {
        return;
    }

    // Apply greedy graph coloring algorithm
    gt::GreedyGraphColoring(&graph);

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

void MainWindow::runMVD()
{

    int startVertexId = ui->_startVertexId->value()-1;
    int endVertexId = ui->_endVertexId->value()-1;
    if (startVertexId < 0 || endVertexId < 0)
        return;

    // setup graph data
    gt::Graph graph;
    if (!setupGraph(&graph))
    {
        return;
    }

    // Apply minimal distance computation
    QList<int> path;
    double distance = gt::ComputeMinDistance(graph, startVertexId, endVertexId, &path);


    // Display the result:
    ui->_distance->setText(QString("%1").arg(distance));

//    foreach (QGraphicsLineItem * edge, _edges)
//    {
//        int vertexIndex1 = edge->data(KEY_EDGE_VERTEX1).toInt();
//        int vertexIndex2 = edge->data(KEY_EDGE_VERTEX2).toInt();
//    }

}

//******************************************************************************

void MainWindow::onValueEdited()
{
    bool ok=false;
    int newvalue = _valueEditor.text().toInt(&ok);
    if (ok)
    {

        if (qgraphicsitem_cast<QGraphicsSimpleTextItem*>(_editedItem))
        {
            QGraphicsSimpleTextItem* text = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(_editedItem);
            text->setText(QString("%1").arg(newvalue));
            QGraphicsItem* parent = text->parentItem();
            if (parent)
            {
                parent->setData(KEY_EDGE_WEIGHT, newvalue);
            }
            _valueEditor.hide();
        }

    }
}

//******************************************************************************

void MainWindow::onChooseVertexId()
{
    _isChooseVertexMode=true;
    QToolButton * tb = qobject_cast<QToolButton*>(sender());
    if (tb)
    {
        tb->setDown(true);
        _chooseSender=tb;
    }
}

//******************************************************************************

bool MainWindow::setupGraph(gt::Graph *graph)
{
    if (!graph)
        return false;

    graph->vertices.resize(_vertices.size());
    for (int i=0;i<graph->vertices.size();i++)
    {
        graph->vertices[i].id = _vertices[i]->data(KEY_VERTEX_ID).toInt();
    }

    foreach ( QGraphicsLineItem * edge, _edges)
    {
        int vertexIndex1 = edge->data(KEY_EDGE_VERTEX1).toInt();
        int vertexIndex2 = edge->data(KEY_EDGE_VERTEX2).toInt();
        int weight = edge->data(KEY_EDGE_WEIGHT).toInt();


        if (!graph->edges.contains(vertexIndex1))
        {
            QList<gt::EdgeConnection> vts = QList<gt::EdgeConnection>()
                    << gt::EdgeConnection(&graph->vertices[vertexIndex2], weight);
            graph->edges.insert(vertexIndex1, vts);
        }
        else
        {
            QList<gt::EdgeConnection> & vts = graph->edges[vertexIndex1];
            vts << gt::EdgeConnection(&graph->vertices[vertexIndex2], weight);
        }

        if (!graph->edges.contains(vertexIndex2))
        {
            QList<gt::EdgeConnection> vts = QList<gt::EdgeConnection>()
                    << gt::EdgeConnection(&graph->vertices[vertexIndex1], weight);
            graph->edges.insert(vertexIndex2, vts);
        }
        else
        {
            QList<gt::EdgeConnection> & vts = graph->edges[vertexIndex2];
            vts << gt::EdgeConnection(&graph->vertices[vertexIndex1], weight);
        }

    }

    if (graph->vertices.isEmpty())
        return false;

    return true;
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

            QGraphicsSceneMouseEvent * mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

            if (_scene.items(QRectF(
                        mouseEvent->scenePos().x() - VERTEX_SIZE*0.5,
                        mouseEvent->scenePos().y() - VERTEX_SIZE*0.5,
                        VERTEX_SIZE,
                        VERTEX_SIZE)
                        ).isEmpty())
            {
                QGraphicsEllipseItem * vertex = _scene.addEllipse(
                            QRectF(-VERTEX_SIZE*0.5, -VERTEX_SIZE*0.5, VERTEX_SIZE, VERTEX_SIZE),
                            QPen(Qt::black, 0),
                            QBrush(Qt::white)
                            );
                vertex->setTransform(
                            QTransform::fromTranslate(mouseEvent->scenePos().x(), mouseEvent->scenePos().y())
                            );
                vertex->setZValue(0.0);
                _vertices << vertex;
                int id = _vertices.size()-1;
                vertex->setData(KEY_VERTEX_ID, id);
                QGraphicsSimpleTextItem * vertexId = _scene.addSimpleText(QString("%1").arg(id+1));
                vertexId->setParentItem(vertex);
                vertexId->setTransform(
                            QTransform::fromScale(0.005, 0.005)
                            * QTransform::fromTranslate(-VERTEX_SIZE*( (id < 9) ? 0.18 : 0.28 ), -VERTEX_SIZE*0.35)
                            );
                vertexId->setZValue(vertex->zValue()+0.1);

            }
            else
            {
                QGraphicsItem * item = _scene.itemAt(mouseEvent->scenePos(), QTransform());
                if (qgraphicsitem_cast<QGraphicsEllipseItem*>(item)
                        || qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item))
                {
                    QGraphicsEllipseItem* vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(item->parentItem());
                    if (!vertex)
                        vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);

                    if (vertex)
                    {
                        _isDrawingEdge=true;
                        _drawingEdge = _scene.addLine(0.0,
                                                      0.0,
                                                      mouseEvent->scenePos().x()-vertex->scenePos().x(),
                                                      mouseEvent->scenePos().y()-vertex->scenePos().y(),
                                                      QPen(Qt::black, 0));
                        _drawingEdge->setTransform(QTransform::fromTranslate(vertex->scenePos().x(), vertex->scenePos().y()));
                        _drawingEdge->setZValue(1.0);
                        _drawingEdge->setData(KEY_EDGE_VERTEX1, vertex->data(KEY_VERTEX_ID));
                    }

                }
            }
        }
        else if (event->type() == QEvent::GraphicsSceneMouseMove && _isDrawingEdge)
        {
            QGraphicsSceneMouseEvent * mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            double x = _drawingEdge->transform().dx();
            double y = _drawingEdge->transform().dy();
            _drawingEdge->setLine(0.0,0.0, mouseEvent->scenePos().x()-x, mouseEvent->scenePos().y()-y);
        }
        else if (event->type() == QEvent::GraphicsSceneMouseRelease && _isDrawingEdge)
        {
            QGraphicsSceneMouseEvent * mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

            // need to put the connecting line on the background otherwise it is detected under the mouse
            _drawingEdge->setZValue(-1.0);

            QGraphicsItem * item = _scene.itemAt(mouseEvent->scenePos(), QTransform());
            if (qgraphicsitem_cast<QGraphicsEllipseItem*>(item)
                    || qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item))
            {
                QGraphicsEllipseItem* vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(item->parentItem());
                if (!vertex)
                    vertex = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);

                if (vertex)
                {
                    double x = _drawingEdge->transform().dx();
                    double y = _drawingEdge->transform().dy();
                    _drawingEdge->setLine(0.0,0.0, vertex->scenePos().x()-x, vertex->scenePos().y()-y);
                    _drawingEdge->setZValue(-1.0);

                    if (_drawingEdge->data(KEY_EDGE_VERTEX1) == vertex->data(KEY_EDGE_VERTEX1))
                        _scene.removeItem(_drawingEdge);
                    else
                    {
                        int defaultWeight = 1;
                        _drawingEdge->setData(KEY_EDGE_VERTEX2, vertex->data(KEY_VERTEX_ID));
                        _drawingEdge->setData(KEY_EDGE_WEIGHT, defaultWeight);
                        // draw edge weight
                        QGraphicsSimpleTextItem * edgeWeight = _scene.addSimpleText(QString("%1").arg(defaultWeight));
                        edgeWeight->setParentItem(_drawingEdge);
                        edgeWeight->setBrush(Qt::blue);
                        QLineF line = _drawingEdge->line();
                        edgeWeight->setTransform(
                                    QTransform::fromScale(0.005, 0.005)
                                    * QTransform::fromTranslate(line.x2()*0.5, line.y2()*0.5)
                                    );

                        // add to graph edges
                        _edges << _drawingEdge;
                    }
                }
            }
            else
            {
                _scene.removeItem(_drawingEdge);
            }
            _isDrawingEdge=false;
            _drawingEdge=0;
        }
        else if (event->type() == QEvent::GraphicsSceneMouseDoubleClick)
        {
            QGraphicsSceneMouseEvent * mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QGraphicsItem * item = _scene.itemAt(mouseEvent->scenePos(), QTransform());
            QGraphicsSimpleTextItem* text = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);

            // Modify edge weight
            if (text)
            {
                QGraphicsLineItem* edge = qgraphicsitem_cast<QGraphicsLineItem*>(text->parentItem());
                if (edge)
                { // text is edge weight

                    _editedItem = text;

                    int weight = edge->data(KEY_EDGE_WEIGHT).toInt();
                    _valueEditor.setText(QString("%1").arg(weight));
                    _valueEditor.show();
                    _valueEditor.resize(20,_valueEditor.height());
                    _valueEditor.move(mouseEvent->screenPos());

                }

            }
            // Choose vertex as start or end
            if (qgraphicsitem_cast<QGraphicsEllipseItem*>(item) || text)
            {
                QGraphicsItem * item = _scene.itemAt(mouseEvent->scenePos(), QTransform());
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
    return QMainWindow::eventFilter(object, event);
}

//******************************************************************************

void MainWindow::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    ui->_graphView->fitInView(_scene.sceneRect(), Qt::KeepAspectRatio);
}

//******************************************************************************

MainWindow::~MainWindow()
{
    delete ui;
}

//******************************************************************************
