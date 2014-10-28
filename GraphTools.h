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

struct Edge
{
    Edge() : a(0), b(0), weight(-1)
    {}
    Vertex *a,*b;
    double weight;
};

//******************************************************************************

typedef QPair<Vertex*, double> EdgeConnection;

struct Graph
{

    Graph()
    {
    }
    QVector<Vertex> vertices;
    void setEdges(const QVector<Edge> & edges);

    const QVector<Edge> & getEdges() const
    { return _edges; }
    const QHash<int, QList<EdgeConnection> > & getEdgeConnections() const
    { return _edgeConnections; }

protected:

    QVector<Edge> _edges;
    QHash<int, QList<EdgeConnection> > _edgeConnections;


};

//******************************************************************************

bool TestStdDoubleMaxLimit();

void GreedyGraphColoring(Graph * graph);

double ComputeMinDistance(const Graph & graph, int startIndex, int endIndex, QList<int> *path);

//******************************************************************************

}

//******************************************************************************

#endif // GREEDYGRAPHCOLORINGTOOLS_H
