
// Project
#include "GreedyGraphColoringTools.h"

//******************************************************************************

bool ColorGraph(Graph * graph, int color)
{
    bool output=false;
    for (int i=0; i<graph->vertices.size();i++)
    {
        Vertex & notColoredVertex = graph->vertices[i];
        if (notColoredVertex.color >= 0)
            continue;


        QList<Vertex*> connectedVertices = graph->edges.value(notColoredVertex.id);
        bool sameColorVertexFound=false;
        foreach (Vertex* vertex, connectedVertices)
        {
            if (vertex->color == color)
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


