#include <algorithm>
#include <vector>
#include <iostream>

#include "fullereneTypes.h" 
#include "buildFullerene.h"


bool Build::fillEV(const int faceIndex, const int edgeIndex, const Vertex& vertex, Edge **edge)
{
    int eIdx = 2;

    while ((-1 != vertex.e[eIdx]) && (0 != eIdx))
    {
        --eIdx;
    }
    //check a free edge of the vertex 
    if (-1 == vertex.e[eIdx])
    {
        return true;
    }
    eIdx = 2;
    while ((-1 != edges[vertex.e[eIdx]].s2) && (0 != eIdx))
    {
        --eIdx;
    }
    
    //occuping it
    faces[faceIndex].edge[edgeIndex] = vertex.e[eIdx];

    (*edge) = &edges[vertex.e[eIdx]];
    (*edge)->s2 = faceIndex;
    LOG("Edge occupied by the face " << (*edge)->s1 << ", edge"
        << faces[faceIndex].edge[edgeIndex] << ", edge index on face " << edgeIndex);
    return false;
}

Result Build::graph(const Params& params, std::vector<Vertex>& graph)
{
    LOG("Building graph...");

    faces.resize(params.faceNum);
    graph.reserve(params.faceNum * 2);
    edges.reserve(params.faceNum * 3);


    //initializing number of edges for pentagons
    for (auto p : params.pentagons)
    {
        faces[p].size = 5;
    }
    //building first face
    unsigned int currentCycle = 0;
    auto face = &faces[0];
    for (int edgeIndex = 0; edgeIndex < face->size; ++edgeIndex)
    {
        int v2 = 0;
        if (edgeIndex + 1 != face->size) v2 = edgeIndex + 1;
        edges.push_back(Edge(0, -1, edgeIndex, v2));

        face->edge[edgeIndex] = edgeIndex;

        int e0 = edgeIndex - 1;
        if (0 == edgeIndex) e0 = face->size - 1;

        graph.push_back(Vertex(e0, edgeIndex, -1, currentCycle, static_cast<int>(graph.size()), false));
    }
    //loop on the remainig faces
    std::size_t freeFaceIndex = 0;
    int targetFaceIndex = 1;
    std::size_t currentVertexIdx = 0;
    ++currentCycle;
    bool newCycle = false;

    for (int faceIndex = 1; faceIndex < params.faceNum; ++faceIndex)
    {
        if (newCycle)
        {
            newCycle = false;
            targetFaceIndex = faceIndex + 1;
            ++currentCycle;
            currentVertexIdx = 0;
        }
        face = &faces[faceIndex];
        auto freeFace = &faces[freeFaceIndex];
        LOG("Face " << faceIndex << ", edges number" << face->size);
        //search for an unoccupied free face's edge 
        int edgeIndex = 0;
        while (-1 != edges[freeFace->edge[edgeIndex]].s2)
        {
            ++edgeIndex;
            if (edgeIndex == freeFace->size)
            {
                ++freeFaceIndex;
                if (freeFaceIndex == params.faceNum)
                {
                    LOG_ERROR("Can't find a free face");
                    return Result::FAIL;
                }
                LOG("Free face " << freeFaceIndex);
                freeFace = &faces[freeFaceIndex];
                edgeIndex = 0;
            }
        }
        //Founded it. Use it.
        LOG("Unoccupied edge " << edgeIndex << ", free face " << faceIndex);

        //fill edges and graph of the current face
        face->edge[0] = freeFace->edge[edgeIndex];
        auto edge = &(edges[face->edge[0]]);
        edge->s2 = faceIndex;

        for (edgeIndex = 1; edgeIndex < face->size; ++edgeIndex)
        {
            if (fillEV(faceIndex, edgeIndex, graph[edge->v1], &edge)) break;
        }
        //If all edges occupied, then continue
        if (edgeIndex == face->size)
        {
            //проверим, а туда ли мы пришли
            if (edges[face->edge[0]].v2 != edges[face->edge[face->size - 1]].v1)
            {
                LOG_ERROR("Face " << faceIndex << " did not close.");
                return Result::FAIL;
            }
            else LOG("Ok! The graph is closed.");
            continue;
        }
        int edgeBegin = edgeIndex;
        //revers order
        edge = &edges[face->edge[0]];
        for (edgeIndex = face->size - 1; edgeIndex >= 2; --edgeIndex)
        {
            if (fillEV(faceIndex, edgeIndex, graph[edge->v2], &edge)) break;
            if (edge->s1 == targetFaceIndex)
            {
                LOG("Cycle " << targetFaceIndex);
                newCycle = true;
            }
        }
        //creating the face's new edges and graph
        int edgeEnd = edgeIndex;
        graph[edges[face->edge[edgeBegin - 1]].v1].e[2] = static_cast<int>(edges.size());
        for (edgeIndex = edgeBegin; edgeIndex < edgeEnd; ++edgeIndex)
        {
            face->edge[edgeIndex] = static_cast<int>(edges.size());

            int v1 = edges[face->edge[edgeIndex - 1]].v1;
            if (edgeIndex != edgeBegin)
            {
                v1 = edges[face->edge[edgeIndex - 1]].v2;
            }
            edges.push_back(Edge(faceIndex, -1, v1, static_cast<int>(graph.size())));

            graph.push_back(Vertex(static_cast<int>(edges.size()) - 1,
                static_cast<int>(edges.size()), -1, currentCycle, currentVertexIdx, false));

            ++currentVertexIdx;
        }
        //creating the face's last edge 
        face->edge[edgeIndex] = static_cast<int>(edges.size());
        int v2 = edges[face->edge[0]].v2;
        if (edgeIndex != face->size - 1) v2 = edges[face->edge[edgeIndex + 1]].v2;
        graph[v2].e[2] = static_cast<int>(edges.size());
        int v1 = edges[face->edge[edgeIndex - 1]].v2;
        if (edgeBegin == edgeEnd)
        {
            v1 = edges[face->edge[edgeIndex - 1]].v1;
            graph[v1].e[2] = static_cast<int>(edges.size());
        }
        edges.push_back(Edge(faceIndex, -1, v1, v2));
    }// main loop of creating faces
    //marking last face
    face = &faces[params.faceToExpand];
    ++currentCycle;
    int v;
    if ((edges[face->edge[0]].v1 == edges[face->edge[1]].v1)
        || (edges[face->edge[0]].v1 == edges[face->edge[1]].v2))
    {
        v = edges[face->edge[0]].v2;
    }
    else
    {
        v = edges[face->edge[0]].v1;
    }
    graph[v].lastFace = true;
    graph[v].cycleVertexIndex = 0;
    graph[v].cycle = currentCycle;

    for (int i = 1; i < face->size; ++i)
    {
        int j = face->edge[i];
        if ((edges[j].v1 == edges[face->edge[i - 1]].v1)
            || (edges[j].v1 == edges[face->edge[i - 1]].v2))
        {
            v = edges[j].v1;
        }
        else
        {
            v = edges[j].v2;
        }
        graph[v].lastFace = true;
        graph[v].cycleVertexIndex = i;
        graph[v].cycle = currentCycle;
    }
    for (std::size_t i = 0; i < graph.size(); ++i)
    {
        graph[i].index = i;
    }
    //sorting, last face will be on the end
    std::sort(graph.begin(), graph.end(), [](Vertex& a, Vertex& b) {
        if (a.lastFace == b.lastFace)
        {
            if (a.cycle > b.cycle) return false;
            else
            {
                if (a.cycle < b.cycle) return true;
                else
                {
                    if (a.cycleVertexIndex > b.cycleVertexIndex) return false;
                    else
                    {
                        if (a.cycleVertexIndex < b.cycleVertexIndex) return true;
                        else return true;
                    }
                }
            }
        }
        else
        {
            if (a.lastFace && !b.lastFace) return false;
            else return true;
        }
        return true;
    });
    //reindexing graph edges
    for (std::size_t vertexIndex = 0; vertexIndex < graph.size(); ++vertexIndex)
    {
        currentVertexIdx = graph[vertexIndex].index;
        for (std::size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
        {
            auto edge = &edges[graph[vertexIndex].e[edgeIndex]];
            if (edge->v1 == static_cast<int>(currentVertexIdx))
                edge->v1 = static_cast<int>(vertexIndex);
            else edge->v2 = static_cast<int>(vertexIndex);
        }
    }
    //replacement in graph array edges indexes to vertexes indexes
    for (std::size_t vertexIndex = 0; vertexIndex < graph.size(); ++vertexIndex)
    {
        auto vertex = &(graph[vertexIndex]);
        for (int edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
        {
            auto edge = &edges[vertex->e[edgeIndex]];
            if (edge->v1 == static_cast<int>(vertexIndex))
                vertex->e[edgeIndex] = edge->v2;
            else vertex->e[edgeIndex] = edge->v1;
        }
    }
    LOG("The graph is complete.");

    return Result::OK;
}