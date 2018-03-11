#include <algorithm>
#include <vector>
#include <iostream>

#include "fullereneTypes.h" 
#include "buildFullerene.h"



Result buildGraph(const Params& params, std::vector<Vertex>& graph)
{
    LOG( L"Building graph...");

    std::vector<Face> faces;
    std::vector<Edge> edges;
    std::vector<Vertex> vertexes;
    
    faces.resize(params.faceNum);
    vertexes.reserve(params.faceNum * 2);
    edges.reserve(params.faceNum * 3);
    
    try
    {
        //initializing number of edges for pentagons
        for (auto p : params.pentagons)
        {
            faces[p].size = 5;
        }
        //building first face
        auto face = &faces[0];
        for (int edgeIndex = 0; edgeIndex < face->size; ++edgeIndex)
        {
            int v2 = 0;
            if (edgeIndex + 1 != face->size) v2 = edgeIndex + 1;
            edges.push_back(Edge(0,-1,edgeIndex, v2));
                        
            face->edge[edgeIndex] = edgeIndex;

            int e0 = edgeIndex - 1 ;
            if (0 == edgeIndex) e0 = face->size - 1;
            vertexes.push_back(Vertex(e0, edgeIndex, -1, 0, static_cast<int>(vertexes.size()), 0));
        }
        //loop on the remainig faces
        size_t freeFaceIndex = 0;
        size_t targetFaceIndex = 1;
        int curentVertexIdx = 0;
        bool cycle = false;
        unsigned int curentCycle = 1;
        for (int faceIndex = 1; faceIndex < params.faceNum; ++faceIndex)
        {
            if (cycle)
            {
                cycle = false;
                targetFaceIndex = faceIndex + 1;
                ++curentCycle;
                curentVertexIdx = 0;
            }
            face = &faces[faceIndex];
            auto freeFace = &faces[freeFaceIndex];
            LOG( L"Face " << faceIndex << L", edges number" << face->size);
             //search for an unoccupied free face's edge 
            int edgeIndex = 0;
            while (-1 != edges[freeFace->edge[edgeIndex]].s2)
            {
                ++edgeIndex;
                if (edgeIndex == freeFace->size)
                {
                    ++freeFaceIndex;
                    if (freeFaceIndex == params.faceNum )
                    {
                        LOG_ERROR( L"Can't find a free face" );
                        throw std::exception();
                    }
                    LOG( L"Free face " << freeFaceIndex);
                    freeFace = &faces[freeFaceIndex];
                    edgeIndex = 0;
                }
            } 
            //Founded it. Use it.
            LOG( L"Unoccupied edge " << edgeIndex << L", free face " << faceIndex);
            face->edge[0] = freeFace->edge[edgeIndex];

            auto edge = &(edges[face->edge[0]]);
            edge->s2 = faceIndex;
            //fill edges and vertexes of the current face
            for (edgeIndex = 1; edgeIndex < face->size; ++edgeIndex)
            {
                //if the vertex have 3 connections
                int vertexIndex = 3;
                do
                {
                    --vertexIndex;
                } while ((-1 != vertexes[edge->v1].e[vertexIndex]) && (0 != vertexIndex));
                //then search a free edge of the vertex 
                if (vertexes[edge->v1].e[vertexIndex] != -1)
                {
                    vertexIndex = 3;
                    do
                    {
                        --vertexIndex;
                    } while ((-1 != edges[vertexes[edge->v1].e[vertexIndex]].s2) && (0 != vertexIndex));
                    if (-1 != edges[vertexes[edge->v1].e[vertexIndex]].s2)
                    {
                        LOG_ERROR( L"No free edge found." );
                        throw std::exception(); 
                    }
                    //occuping it
                    face->edge[edgeIndex] = vertexes[edge->v1].e[vertexIndex];
                    edge = &edges[vertexes[edge->v1].e[vertexIndex]];
                    edge->s2 = faceIndex;
                    LOG( L"Edge occupied by the face " << edge->s1 << L", edge" 
                        << face->edge[edgeIndex] << L", edge index on face " << edgeIndex);
                }
                else
                {
                    break;
                }
            }
            //If all edges occupied, then continue
            if (edgeIndex == face->size)
            {
                //проверим, а туда ли мы пришли
                if (edges[face->edge[0]].v2 != edges[face->edge[face->size - 1]].v1)
                {
                    LOG_ERROR( L"Face " << faceIndex << L" did not close." );
                    throw std::exception(); 
                }
                else LOG( L"Ok! The graph is closed.");
                continue;
            }
            int edgeBegin = edgeIndex;
            //revers order
            edge = &edges[face->edge[0]];
            for (edgeIndex = face->size - 1; edgeIndex >= 2; --edgeIndex)
            {
                //if vertex have three connection
                int vertexIndex = 3;
                do
                {
                    --vertexIndex;
                } while ((-1 != vertexes[edge->v2].e[vertexIndex]) && (0 != vertexIndex));

                //then search a free edge
                if (-1 != vertexes[edge->v2].e[vertexIndex])
                {
                    vertexIndex = 3;
                    do
                    {
                        --vertexIndex;
                    } while ((-1 != edges[vertexes[edge->v2].e[vertexIndex]].s2) && (0 != vertexIndex));
                    if (edges[vertexes[edge->v2].e[vertexIndex]].s2 != -1)
                    {
                        LOG_ERROR( L"No free edge found." );
                        throw std::exception();
                    }
                    //occuping it
                    face->edge[edgeIndex] = vertexes[edge->v2].e[vertexIndex];
                    edge = &edges[vertexes[edge->v2].e[vertexIndex]];
                    edge->s2 = faceIndex;
                    LOG( L"The edge occupied by the face " << edge->s1 << L", edge" << face->edge[edgeIndex] 
                        << L", edge index on the face " << edgeIndex );
                    if (edge->s1 == targetFaceIndex)
                    {
                        LOG( L"Cycle " << targetFaceIndex );
                        cycle = true;
                    }
                }
                else
                {
                    break;
                }
            }
            //creating the face's new edges and vertexes
            int edgeEnd = edgeIndex;
            vertexes[edges[face->edge[edgeBegin - 1]].v1].e[2] = static_cast<int>(edges.size());
            for (edgeIndex = edgeBegin; edgeIndex < edgeEnd; ++edgeIndex)
            {
                face->edge[edgeIndex] = static_cast<int>(edges.size());

                int v1 = edges[face->edge[edgeIndex - 1]].v1;
                if (edgeIndex != edgeBegin) v1 = edges[face->edge[edgeIndex - 1]].v2;
                edges.push_back(Edge(faceIndex, -1, v1, static_cast<int>(vertexes.size())));
                
                vertexes.push_back(Vertex(static_cast<int>(edges.size()) - 1, 
                    static_cast<int>(edges.size()), -1, curentCycle, curentVertexIdx, 0));
                                
                ++curentVertexIdx;
            }
            //creating the face's last edge 
            face->edge[edgeIndex] = static_cast<int>(edges.size());
            int v2 = edges[face->edge[0]].v2;
            if (edgeIndex != face->size - 1) v2 = edges[face->edge[edgeIndex + 1]].v2;
            vertexes[v2].e[2] = static_cast<int>(edges.size());
            int v1 = edges[face->edge[edgeIndex - 1]].v2;
            if (edgeBegin == edgeEnd)
            {
                v1 = edges[face->edge[edgeIndex - 1]].v1;
                vertexes[v1].e[2] = static_cast<int>(edges.size());
            }
            edges.push_back(Edge(faceIndex, -1, v1, v2));
        }// main loop of creating faces
        //marking last face
        face = &faces[params.faceToExpand];
        ++curentCycle;
        if ((edges[face->edge[0]].v1 == edges[face->edge[1]].v1)
            || (edges[face->edge[0]].v1 == edges[face->edge[1]].v2))
        {
            auto v2 = edges[face->edge[0]].v2;
            vertexes[v2].e[5] = 1000;
            vertexes[v2].e[4] = 0;
            vertexes[v2].e[3] = curentCycle;
        }
        else
        {
            auto v1 = edges[face->edge[0]].v1;
            vertexes[v1].e[5] = 1000;
            vertexes[v1].e[4] = 0;
            vertexes[v1].e[3] = curentCycle;
        }
        for (int i = 1; i < face->size; ++i)
        {
            int j = face->edge[i];
            if ((edges[j].v1 == edges[face->edge[i - 1]].v1)
                || (edges[j].v1 == edges[face->edge[i - 1]].v2))
            {
                auto v1 = edges[j].v1;
                vertexes[v1].e[5] = 1000;
                vertexes[v1].e[4] = i;
                vertexes[v1].e[3] = curentCycle;
            }
            else
            {
                auto v2 = edges[j].v2;
                vertexes[v2].e[5] = 1000;
                vertexes[v2].e[4] = i;
                vertexes[v2].e[3] = curentCycle;
            }
        }
        //creating output graph
        graph.resize(vertexes.size());
        for (size_t i = 0; i < vertexes.size(); ++i)
        {
            graph[i].e[4] = static_cast<int>(i);
            graph[i].e[2] = vertexes[i].e[3];
            graph[i].e[3] = vertexes[i].e[4];
            graph[i].e[5] = vertexes[i].e[5];
        }
        //sorting, last face vertexes will be on the end
        std::sort(graph.begin(), graph.end(), [](Vertex& a, Vertex& b) {
            if ( a.e[5] == b.e[5])
            {
                if (a.e[2] > b.e[2]) return false;
                else
                {
                    if (a.e[2] < b.e[2]) return true;
                    else
                    {
                        if (a.e[3] > b.e[3]) return false;
                        else
                        {
                            if (a.e[3] < b.e[3]) return true;
                            else return true;
                        }
                    }
                }
            }
            else
            {
                if( a.e[5] > b.e[5]) return false;
                else return true;
            }
            return true;
        });
        //reindexing vertexes
        for (size_t vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            curentVertexIdx = graph[vertexIndex].e[4];
            for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
            {
                auto edge = &edges[vertexes[curentVertexIdx].e[edgeIndex]];
                if (edge->v1 == curentVertexIdx)
                    edge->v1 = static_cast<int>(vertexIndex);
                else edge->v2 = static_cast<int>(vertexIndex);
            }
        }
        //creating vertexes output array
        for (size_t vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            auto vertex = &(graph[vertexIndex]);
            curentVertexIdx = vertex->e[4];
            for (int edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
            {
                auto edge = &edges[vertexes[curentVertexIdx].e[edgeIndex]];
                if (edge->v1 == vertexIndex)
                    vertex->e[edgeIndex] = edge->v2;
                else vertex->e[edgeIndex] = edge->v1;
            }
            vertex->e[3] = vertexes[curentVertexIdx].e[3];
            vertex->e[4] = vertexes[curentVertexIdx].e[4];
            vertex->e[5] = vertexes[curentVertexIdx].e[5];
        }
        LOG( L"The graph is complete." );
     }
    catch (...)
    {
        LOG_ERROR( L"Error! The graph is not built. " );
        return Result::FAIL;
    }
    return Result::OK;
}