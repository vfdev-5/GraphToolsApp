#ifndef GREEDYGRAPHCOLORINGTOOLS_H
#define GREEDYGRAPHCOLORINGTOOLS_H

// Qt
#include <QVector>
#include <QPair>
#include <QHash>

//******************************************************************************

namespace gt {

//******************************************************************************

struct Vertex
{
    int id;
    double weight;
    int color;
    Vertex() :
        id(-1),
        weight(0.0),
        color(-1)
    {
    }
};

//******************************************************************************

//struct Edge
//{
//    typedef QPair<Vertex,Vertex> EdgeConnection;
//    EdgeConnection connection;
//    double weight;
//};

//******************************************************************************

typedef QPair<Vertex*, double> EdgeConnection;

struct Graph
{

    Graph()
    {
    }
    QVector<Vertex> vertices;
//    QHash<int, QList<Vertex*> > edges;
    QHash<int, QList<EdgeConnection> > edges;

};

//******************************************************************************

void GreedyGraphColoring(Graph * graph);

double ComputeMinDistance(const Graph & graph, int startIndex, int endIndex, QList<Vertex *> *path);

//******************************************************************************

}

//******************************************************************************

#endif // GREEDYGRAPHCOLORINGTOOLS_H
