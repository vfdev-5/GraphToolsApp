#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

// Qt
#include <QWidget>
#include <QGraphicsScene>
#include <QShowEvent>
#include <QResizeEvent>
#include <QLineEdit>

static const double VERTEX_SIZE=0.1;
static const int KEY_EDGE_VERTEX1=0;
static const int KEY_EDGE_VERTEX2=1;
static const int KEY_EDGE_WEIGHT=2;
static const int KEY_VERTEX_ID=0;

static const double VERTEX_CIRCLE_Z = 10.0;
static const double VERTEX_TEXT_Z = 11.0;
static const double EDGE_LINE_Z = 0.0;
static const double EDGE_TEXT_Z = 5.0;
static const double PATH_LINE_Z = 2.0;

namespace GT {

class Graph;

//******************************************************************************

class GraphViewer : public QWidget
{
    Q_OBJECT
public:
    explicit GraphViewer(QWidget *parent = 0);

public slots:
    virtual void clear();

protected slots:
    void onValueEdited();

protected:
    bool setupGraph(GT::Graph * graph);
    void showEvent(QShowEvent * e);
    void resizeEvent(QResizeEvent * e);
    virtual bool eventFilter(QObject *, QEvent *);

    QGraphicsScene _scene;
    QGraphicsView * _view;

    QVector<QGraphicsEllipseItem*> _vertices;
    QVector<QGraphicsLineItem*> _edges; //!< GraphicsItem contains data info : key=0 -> vertex1 number, key=1 -> vertex2 number, key=3 -> edge weight

    QGraphicsSimpleTextItem * _initialText;

    bool _isDrawingEdge;
    QGraphicsLineItem* _drawingEdge;

    QLineEdit _valueEditor;
    QGraphicsItem* _editedItem;

private:
    void onSceneMousePress(QGraphicsSceneMouseEvent* event);
    void onSceneMouseMove(QGraphicsSceneMouseEvent* event);
    void onSceneMouseRelease(QGraphicsSceneMouseEvent* event);
    void onSceneMouseDoubleClick(QGraphicsSceneMouseEvent* event);

};

//******************************************************************************

}

#endif // GRAPHVIEWER_H
