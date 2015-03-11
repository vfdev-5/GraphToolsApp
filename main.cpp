
// Qt
#include <QApplication>

// Project
#include "GraphToolsWidget.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    GT::GraphToolsWidget g;
    g.show();
    return a.exec();

}
