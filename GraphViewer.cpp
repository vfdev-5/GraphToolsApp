
// Std
#include <iostream>

// Qt
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>

// Project
#include "GraphTools.h"
#include "GraphViewer.h"

namespace GT {

//******************************************************************************

GraphViewer::GraphViewer(QWidget *parent) :
    QWidget(parent),
    _initialText(0),
    _drawingEdge(0),
    _isDrawingEdge(false),
    _editedItem(0)
{
    setWindowTitle(tr("GraphViewer"));

    // setup value editor
    _valueEditor.setWindowFlags(Qt::Popup);
    _valueEditor.installEventFilter(this);
    _valueEditor.setAlignment(Qt::AlignRight);

    // setup scene:
    _scene.setSceneRect(0.0, 0.0, 1.0, 1.0);
    _scene.installEventFilter(this);

    clear();

    // setup view
    _view = new QGraphicsView();
    _view->setScene(&_scene);
    _view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setLayout(new QVBoxLayout());
    layout()->addWidget(_view);

}

//******************************************************************************

void GraphViewer::clear()
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
}

//******************************************************************************

bool GraphViewer::setupGraph(GT::Graph * graph)
{
    if (!graph)
        return false;

    graph->vertices.resize(_vertices.size());
    for (int i=0;i<graph->vertices.size();i++)
    {
        graph->vertices[i].id = _vertices[i]->data(KEY_VERTEX_ID).toInt();
    }

    // Factor 2 due to the undirected visual graph representation
    QVector<GT::Edge> edges(2*_edges.size());
    for (int i=0; i< _edges.size(); i++)
    {
        QGraphicsLineItem * edge = _edges[i];
        int vertexIndex1 = edge->data(KEY_EDGE_VERTEX1).toInt();
        int vertexIndex2 = edge->data(KEY_EDGE_VERTEX2).toInt();
        int weight = edge->data(KEY_EDGE_WEIGHT).toInt();

        if (weight < 0)
        {
            std::cerr << "Weights should not negative for undirected graphs" << std::endl;
            return false;
        }

        edges[2*i].a = &graph->vertices[vertexIndex1];
        edges[2*i].b = &graph->vertices[vertexIndex2];
        edges[2*i].weight = weight;

        edges[2*i+1].a = &graph->vertices[vertexIndex2];
        edges[2*i+1].b = &graph->vertices[vertexIndex1];
        edges[2*i+1].weight = weight;

    }


    graph->setEdges(edges);

    if (graph->vertices.isEmpty())
        return false;

    return true;
}

//******************************************************************************

void GraphViewer::onValueEdited()
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

void GraphViewer::onSceneMousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (_vertices.isEmpty())
        _initialText->setVisible(false);

    if (_scene.items(QRectF(
                mouseEvent->scenePos().x() - VERTEX_SIZE*0.5,
                mouseEvent->scenePos().y() - VERTEX_SIZE*0.5,
                VERTEX_SIZE,
                VERTEX_SIZE)
                ).isEmpty())
    {
        // Create new vertex
        QGraphicsEllipseItem * vertex = _scene.addEllipse(
                    QRectF(-VERTEX_SIZE*0.5, -VERTEX_SIZE*0.5, VERTEX_SIZE, VERTEX_SIZE),
                    QPen(Qt::black, 0),
                    QBrush(Qt::white)
                    );
        vertex->setTransform(
                    QTransform::fromTranslate(mouseEvent->scenePos().x(), mouseEvent->scenePos().y())
                    );
        vertex->setZValue(VERTEX_CIRCLE_Z);
        _vertices << vertex;
        int id = _vertices.size()-1;
        vertex->setData(KEY_VERTEX_ID, id);
        QGraphicsSimpleTextItem * vertexId = _scene.addSimpleText(QString("%1").arg(id+1));
        vertexId->setParentItem(vertex);
        vertexId->setTransform(
                    QTransform::fromScale(0.005, 0.005)
                    * QTransform::fromTranslate(-VERTEX_SIZE*( (id < 9) ? 0.18 : 0.28 ), -VERTEX_SIZE*0.35)
                    );
        vertexId->setZValue(VERTEX_TEXT_Z);

    }
    else
    {
        // Propose to draw new edge
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
                _drawingEdge->setZValue(VERTEX_CIRCLE_Z);
                _drawingEdge->setData(KEY_EDGE_VERTEX1, vertex->data(KEY_VERTEX_ID));
            }

        }
    }
}

//******************************************************************************

void GraphViewer::onSceneMouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    double x = _drawingEdge->transform().dx();
    double y = _drawingEdge->transform().dy();
    _drawingEdge->setLine(0.0,0.0, mouseEvent->scenePos().x()-x, mouseEvent->scenePos().y()-y);
}

//******************************************************************************

void GraphViewer::onSceneMouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{

    // need to put the connecting line on the background otherwise it is detected under the mouse
    _drawingEdge->setZValue(EDGE_LINE_Z);

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
            _drawingEdge->setZValue(EDGE_LINE_Z);

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
                edgeWeight->setZValue(EDGE_TEXT_Z);
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

//******************************************************************************

void GraphViewer::onSceneMouseDoubleClick(QGraphicsSceneMouseEvent *mouseEvent)
{
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
}

//******************************************************************************

bool GraphViewer::eventFilter(QObject * object, QEvent * event)
{
    if (&_scene == object)
    {
        if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            onSceneMousePress(static_cast<QGraphicsSceneMouseEvent*>(event));
        }
        else if (event->type() == QEvent::GraphicsSceneMouseMove && _isDrawingEdge)
        {
            onSceneMouseMove(static_cast<QGraphicsSceneMouseEvent*>(event));
        }
        else if (event->type() == QEvent::GraphicsSceneMouseRelease && _isDrawingEdge)
        {
            onSceneMouseRelease(static_cast<QGraphicsSceneMouseEvent*>(event));
        }
        else if (event->type() == QEvent::GraphicsSceneMouseDoubleClick)
        {
            onSceneMouseDoubleClick(static_cast<QGraphicsSceneMouseEvent*>(event));
        }
    }
    else if (object == &_valueEditor && _valueEditor.isVisible())
    {
        if (event->type() == QEvent::KeyRelease)
        {
            QKeyEvent * e = static_cast<QKeyEvent*>(event);
            if (e->key() == Qt::Key_Escape)
            {
                _valueEditor.hide();
            }
            else if (e->key() == Qt::Key_Enter)
            {
                onValueEdited();
            }
        }
    }
    return QWidget::eventFilter(object, event);
}

//******************************************************************************

void GraphViewer::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    _view->fitInView(_scene.sceneRect(), Qt::KeepAspectRatio);
}

//******************************************************************************

void GraphViewer::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    _view->fitInView(_scene.sceneRect(), Qt::KeepAspectRatio);
}

//******************************************************************************

}
