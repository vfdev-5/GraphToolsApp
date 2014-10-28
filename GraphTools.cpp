
// STD
#include <limits>
#include <iostream>

// Qt

// Project
#include "GraphTools.h"

//******************************************************************************

namespace gt {

//******************************************************************************

void Graph::setEdges(const QVector<Edge> & edges)
{
    _edges = edges;
    _edgeConnections.clear();
    foreach (Edge edge, _edges)
    {
        int vertexIndex1 = edge.a->id;
        int vertexIndex2 = edge.b->id;
        int weight = edge.weight;

        if (!_edgeConnections.contains(vertexIndex1))
        {
            QList<gt::EdgeConnection> vts = QList<gt::EdgeConnection>()
                    << gt::EdgeConnection(&vertices[vertexIndex2], weight);
            _edgeConnections.insert(vertexIndex1, vts);
        }
        else
        {
            QList<gt::EdgeConnection> & vts = _edgeConnections[vertexIndex1];
            vts << gt::EdgeConnection(&vertices[vertexIndex2], weight);
        }

        if (!_edgeConnections.contains(vertexIndex2))
        {
            QList<gt::EdgeConnection> vts = QList<gt::EdgeConnection>()
                    << gt::EdgeConnection(&vertices[vertexIndex1], weight);
            _edgeConnections.insert(vertexIndex2, vts);
        }
        else
        {
            QList<gt::EdgeConnection> & vts = _edgeConnections[vertexIndex2];
            vts << gt::EdgeConnection(&vertices[vertexIndex1], weight);
        }
    }
}

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

        QList<EdgeConnection> connectedVertices = graph->getEdgeConnections().value(notColoredVertex.id);
        bool sameColorVertexFound=false;
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
 * https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm
 * http://e-maxx.ru/algo/ford_bellman
 */
double ComputeMinDistance(const Graph & graph, int startIndex, int endIndex, QList<int> * path)
{
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
    QVector<double> distMatrix(nbVertices, std::numeric_limits<double>::max());
    QVector<int> p(nbVertices, -1);
    distMatrix[startIndex] = 0.0;

    // computation part:
    for (int i=0; i<nbVertices-1; i++)
    {
        bool isModified=false;
        // loop on edges
        const QVector<Edge> & edges = graph.getEdges();
        for (int j=0; j<edges.size();j++)
        {
            if (distMatrix[ edges[j].a->id ] < std::numeric_limits<double>::max())
            {
                if ( distMatrix[ edges[j].b->id ] > distMatrix[ edges[j].a->id ] + edges[j].weight )
                {
                    distMatrix[ edges[j].b->id ] = distMatrix[ edges[j].a->id ] + edges[j].weight;
                    p[ edges[j].b->id ] = edges[j].a->id;
                    isModified=true;
                }
            }
        }
        if (!isModified)
            break;
    }

    double minDistance = distMatrix[endIndex];
    if (minDistance == std::numeric_limits<double>::max())
        return minDistance;

#ifdef _DEBUG
    std::cout << "Distance matrix : ";
    for (int i=0;i<nbVertices;i++)
    {
        std::cout << "(" << i << ", " << distMatrix[i] << ") ";
    }
    std::cout << std::endl;
#endif

    // get path :
    for (int c = endIndex; c != -1; c=p[c])
    {
        path->prepend(c);
    }

    return minDistance;
}

/*
double _ComputeMinDistance(const Graph & graph, int startIndex, int endIndex, QList<int> * path)
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
    QVector< QVector<int> > pathMatrix(nbVertices);

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
            if (pathMatrix[v.id].isEmpty())
            {
                pathMatrix[v.id].resize(nbVertices-1);
                pathMatrix[v.id].fill(-1);
            }

            QList<EdgeConnection> connectedVertices = graph.edges.value(v.id);
            foreach (EdgeConnection ec, connectedVertices)
            {
                Vertex * u = ec.first;
                w = ec.second;
                if (distMatrix[v.id][i] > distMatrix[u->id][i-1] + w)
                {
                    distMatrix[v.id][i] = distMatrix[u->id][i-1] + w;
                    pathMatrix[v.id][i] = u->id;
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



    *path = pathMatrix[endIndex].toList();

    return minDistance;

}
*/
//******************************************************************************

}
