
// STD
#include <limits>
#include <iostream>

// Qt

// Project
#include "GraphTools.h"

//******************************************************************************

namespace GT {

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
            QList<GT::EdgeConnection> vts = QList<GT::EdgeConnection>()
                    << GT::EdgeConnection(&vertices[vertexIndex2], weight);
            _edgeConnections.insert(vertexIndex1, vts);
        }
        else
        {
            QList<GT::EdgeConnection> & vts = _edgeConnections[vertexIndex1];
            vts << GT::EdgeConnection(&vertices[vertexIndex2], weight);
        }

        if (!_edgeConnections.contains(vertexIndex2))
        {
            QList<GT::EdgeConnection> vts = QList<GT::EdgeConnection>()
                    << GT::EdgeConnection(&vertices[vertexIndex1], weight);
            _edgeConnections.insert(vertexIndex2, vts);
        }
        else
        {
            QList<GT::EdgeConnection> & vts = _edgeConnections[vertexIndex2];
            vts << GT::EdgeConnection(&vertices[vertexIndex1], weight);
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
/*!
 * \brief GreedyGraphColoring method implements greedy graph coloring algorithm
 * \param graph
 *
 * Algorithm consists to iteratively color graph vertices until all vertices are colored
 * Algorithm complexity is ???
 *
 */
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

//******************************************************************************
/*!
 * \brief FindConnectedVertices method to compute all connected vertices
 * \param graph
 * \param v
 * \param connectedVertices
 * \return
 *
 * Algorithm used in the method is Depth-first-search
 * http://en.wikipedia.org/wiki/Depth-first_search
 *
 */

QVector<Vertex *> ColorConnectedVertices(Graph & graph, Vertex & inputVertex, int color)
{
    QVector<Vertex *> out;

    if (inputVertex.color >= 0)
    {
        std::cerr << "Input vertex is already colored" << std::endl;
        return out;
    }


    QList<Vertex*> stack;
    stack.push_back(&inputVertex);
    out.append(&inputVertex);
    while (!stack.isEmpty())
    {
        Vertex * v = stack.takeLast();
        if (v->color < 0)
        {
            v->color = color;
            out.append(v);
            QList<EdgeConnection> connectedVertices = graph.getEdgeConnections().value(v->id);
            foreach (EdgeConnection ec, connectedVertices)
            {
                stack.push_back(ec.first);
            }
        }
    }
    return out;

}

//******************************************************************************

bool ColorConnectedVertices(Graph &graph, QVector< QVector<Vertex*> > * connectedVertices)
{
    if (!connectedVertices) return false;

    int color = 0;
    for (int i=0; i<graph.vertices.size();i++)
    {

        Vertex & notColoredVertex = graph.vertices[i];
        if (notColoredVertex.color >= 0)
            continue;

        QVector<Vertex*> cvertices = ColorConnectedVertices(graph, notColoredVertex, color);
        connectedVertices->append(cvertices);
        color++;
    }
    return true;
}


//******************************************************************************

}
