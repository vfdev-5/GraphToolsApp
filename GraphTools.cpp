
// STD
#include <limits>
#include <iostream>

// Project
#include "GraphTools.h"

//******************************************************************************

namespace gt {

//******************************************************************************

bool TestStdDoubleMaxLimit()
{

    double MAX = std::numeric_limits<double>::max();
    double w = 2;

    if (MAX != MAX + w)
    {
        std::cout << "MAX != MAX + w" << std::endl;
        return false;
    }

    if (MAX != MAX - w)
    {
        std::cout << "MAX != MAX - w" << std::endl;
        return false;
    }

    if (0 > MAX - w)
    {
        std::cout << "0 > MAX - w" << std::endl;
        return false;
    }


    if (0 > MAX + w)
    {
        std::cout << "0 > MAX + w" << std::endl;
        return false;
    }

    return true;
}

//******************************************************************************

bool ColorGraph(Graph * graph, int color)
{
    bool output=false;
    for (int i=0; i<graph->vertices.size();i++)
    {
        Vertex & notColoredVertex = graph->vertices[i];
        if (notColoredVertex.color >= 0)
            continue;


//        QList<Vertex*> connectedVertices = graph->edges.value(notColoredVertex.id);
        QList<EdgeConnection> connectedVertices = graph->edges.value(notColoredVertex.id);
        bool sameColorVertexFound=false;
//        foreach (Vertex* vertex, connectedVertices)
        foreach (EdgeConnection vertex, connectedVertices)
        {

            if (vertex.first->color == color)
            {
                sameColorVertexFound=true;
                break;
            }
        }
        if (!sameColorVertexFound)
        {
            notColoredVertex.color=color;
            output=true;
        }
    }
    return output;
}

//******************************************************************************

void GreedyGraphColoring(Graph *graph)
{
    int color=0;
    while (ColorGraph(graph, color))
    {
        color++;
    }
}

//******************************************************************************
/*
 * Method to compute shortest path between two vertices.
 *
 * Compute distance matrix (A_ij) - minimal distance between starting vertex and vertex i passing through j edges
 * init : A_i0 = inf for all i != starting vertex
 *        A_(starting vertex)j = 0
 *
 * Relax edges repeatedly:
 * Loop on number of vertices - 1 and on edges :
 *
 *
 *
 * https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm
 */
double ComputeMinDistance(const Graph & graph, int startIndex, int endIndex, QList<int> * path)
{

    if (!TestStdDoubleMaxLimit())
        return -12345.0;

    if (!path)
        return -12345.0;

    if (startIndex < 0 || startIndex > graph.vertices.size()-1 ||
            endIndex < 0 || endIndex > graph.vertices.size()-1)
    {
        return -12345.0;
    }

    path->clear();

    // initialization :
    int nbVertices = graph.vertices.size();
    QVector< QVector<double> > distMatrix(nbVertices);
    QVector< QList<int> > pathMatrix(nbVertices);

    for (int v=0; v<nbVertices; v++)
    {
        Vertex vertex = graph.vertices[v];
        distMatrix[vertex.id].resize(nbVertices-1);
        if (vertex.id==startIndex)
        {
            for (int j=0; j<nbVertices-1; j++)
            {
                distMatrix[vertex.id][j] = 0.0;
            }
        }
        else
        {
            for (int j=0; j<nbVertices-1; j++)
            {
                distMatrix[vertex.id][j] = std::numeric_limits<double>::max();
            }
        }
    }

    // computation part:
    for (int i=1; i<nbVertices-1; i++)
    {
        // loop on edges
        double w = std::numeric_limits<double>::max();
        for (int j=0; j<nbVertices; j++)
        {
            Vertex v = graph.vertices[j];
            QList<EdgeConnection> connectedVertices = graph.edges.value(v.id);
            foreach (EdgeConnection ec, connectedVertices)
            {
                Vertex * u = ec.first;
                w = ec.second;
                if (distMatrix[v.id][i] > distMatrix[u->id][i-1] + w)
                {
                    distMatrix[v.id][i] = distMatrix[u->id][i-1] + w;
                    pathMatrix[v.id] << u->id;
                }
            }
        }
    }

    // test if there is negative-weight cycle
    double w = std::numeric_limits<double>::max();
    int index=nbVertices-2;
    for (int j=0; j<nbVertices; j++)
    {
        Vertex v = graph.vertices[j];
        QList<EdgeConnection> connectedVertices = graph.edges.value(v.id);
        foreach (EdgeConnection ec, connectedVertices)
        {
            Vertex * u = ec.first;
            w = ec.second;
            if (distMatrix[v.id][index] > distMatrix[u->id][index] + w)
            {
                std::cerr << "A negative-weight loop found" << std::endl;
                return -12345.0;
            }
        }
    }


    double minDistance = distMatrix[endIndex][0];
    for (int i=1;i<nbVertices-1;i++)
    {
        if (minDistance > distMatrix[endIndex][i])
            minDistance = distMatrix[endIndex][i];
    }

    *path = pathMatrix[endIndex];

    return minDistance;






}

//******************************************************************************

}
