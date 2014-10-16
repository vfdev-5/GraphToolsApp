#ifndef GREEDYGRAPHCOLORINGTOOLS_H
#define GREEDYGRAPHCOLORINGTOOLS_H

// Qt
#include <QVector>
#include <QPair>
#include <QHash>

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

struct Edge
{
    typedef QPair<Vertex,Vertex> EdgeConnection;
    EdgeConnection connection;
    double weight;
};

//******************************************************************************

struct Graph
{

    Graph()
    {
    }
    QVector<Vertex> vertices;
//    QVector<Edge> edges;
    QHash<int, QList<Vertex*> > edges;

};

//******************************************************************************

void GreedyGraphColoring(Graph * graph);

//******************************************************************************


#endif // GREEDYGRAPHCOLORINGTOOLS_H
