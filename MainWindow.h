#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QGraphicsScene>
#include <QShowEvent>

//******************************************************************************

namespace Ui {
class MainWindow;
}

//******************************************************************************

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void clear();
    void run();

protected:
    void showEvent(QShowEvent * e);
    bool eventFilter(QObject *, QEvent *);

private:
    Ui::MainWindow *ui;
    QGraphicsScene _scene;

    QVector<QGraphicsEllipseItem*> _vertices;
    QVector<QGraphicsLineItem*> _edges;

    QGraphicsSimpleTextItem * _initialText;

    bool _isDrawingEdge;
    QGraphicsLineItem* _drawingEdge;

};

//******************************************************************************

#endif // MAINWINDOW_H
