#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QGraphicsScene>
#include <QShowEvent>
#include <QLineEdit>

// Project
#include "GraphViewer.h"

namespace Ui {
class GraphToolsWidget;
}

namespace GT {

//******************************************************************************

class GraphToolsWidget : public GT::GraphViewer
{
    Q_OBJECT

public:

    explicit GraphToolsWidget(QWidget *parent = 0);
    ~GraphToolsWidget();

public slots:
    virtual void clear();
    void runGGC();
    void runCCV();
    void runMVD();
    void cleanMVD();

protected:
    virtual bool eventFilter(QObject *, QEvent *);

protected slots:
    void onChooseVertexId();

private:

    Ui::GraphToolsWidget *ui;
    QGraphicsItemGroup* _path; //!< GraphicsItem contains data info : key=0 -> vertex1 number, key=1 -> vertex2 number, key=3 -> edge weight

    bool _isChooseVertexMode;
    QObject * _chooseSender;

};

//******************************************************************************

}

#endif // MAINWINDOW_H
