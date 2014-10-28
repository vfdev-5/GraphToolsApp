#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QGraphicsScene>
#include <QShowEvent>
#include <QLineEdit>

//******************************************************************************

namespace Ui {
class MainWindow;
}

namespace gt {
class Graph;
}

static const double VERTEX_SIZE=0.1;
static const int KEY_EDGE_VERTEX1=0;
static const int KEY_EDGE_VERTEX2=1;
static const int KEY_EDGE_WEIGHT=2;
static const int KEY_VERTEX_ID=0;

static const double VERTEX_CIRCLE_Z = 10.0;
static const double VERTEX_TEXT_Z = 11.0;
static const double EDGE_LINE_Z = 0.0;
static const double EDGE_TEXT_Z = 5.0;
static const double PATH_LINE_Z = 4.0;

//******************************************************************************

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void clear();
    void runGGC();
    void runMVD();

protected:
    bool setupGraph(gt::Graph * graph);
    void showEvent(QShowEvent * e);
    bool eventFilter(QObject *, QEvent *);



protected slots:
    void onValueEdited();
    void onChooseVertexId();

private:
    Ui::MainWindow *ui;
    QGraphicsScene _scene;

    QVector<QGraphicsEllipseItem*> _vertices;
    QVector<QGraphicsLineItem*> _edges; //!< GraphicsItem contains data info : key=0 -> vertex1 number, key=1 -> vertex2 number, key=3 -> edge weight
    QGraphicsItemGroup* _path; //!< GraphicsItem contains data info : key=0 -> vertex1 number, key=1 -> vertex2 number, key=3 -> edge weight


    QGraphicsSimpleTextItem * _initialText;

    bool _isDrawingEdge;
    QGraphicsLineItem* _drawingEdge;

    bool _isChooseVertexMode;
    QObject * _chooseSender;

    QLineEdit _valueEditor;
    QGraphicsItem* _editedItem;

};

//******************************************************************************

#endif // MAINWINDOW_H
