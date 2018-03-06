#include <algorithm>
#include <vector>
#include <iostream>

#include "fullereneTypes.h" 
#include "buildFullerene.h"

Result buildGraph(const Params& params, std::vector<Vertex>& graph, std::vector<Face>& faces)
{
    std::vector<Edge> edges;
    std::vector<Vertex> vertexes;
    int    edgeBegin;
    size_t faceIndex, edgeEnd, vertexIndex;
    
    std::wcout << L"Building graph..." << std::endl;

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
        //build first face
        auto pSd = &faces[0];
        for (size_t edgeIndex = 0; edgeIndex < pSd->size; ++edgeIndex)
        {
            edges.resize(edges.size() + 1);
            auto pEd = edges.end() - 1;
            pSd->edge[edgeIndex] = edgeIndex;
            pEd->v1 = edgeIndex;
            if (edgeIndex + 1 == pSd->size) pEd->v2 = 0;
            else  pEd->v2 = edgeIndex + 1;
            pEd->s1 = 0;
            pEd->s2 = -1;
 
            vertexes.resize(vertexes.size() + 1);
            auto pVr = vertexes.end() - 1;
            if (0 == edgeIndex) pVr->e[0] = pSd->size - 1;
            else pVr->e[0] = edgeIndex - 1;
            pVr->e[1] = edgeIndex;
            pVr->e[2] = -1;
            pVr->e[3] = 0;
            pVr->e[4] = vertexes.size() - 1;
            pVr->e[5] = 0;
        }
        //loop on the remainig faces
        size_t freeFace = 0;
        size_t targetFace = 1;
        size_t curentVertex = 0;
        bool cycle = false;
        unsigned int curentCycle = 1;
        for (faceIndex = 1; faceIndex < params.faceNum; ++faceIndex)
        {
            if (cycle)
            {
                cycle = false;
                targetFace = faceIndex + 1;
                ++curentCycle;
                curentVertex = 0;
            }
            pSd = &faces[faceIndex];
            auto fSd = &faces[freeFace];
            std::wcout << L"Face " << faceIndex << L", edges number" << pSd->size << std::endl;
             //search for an unoccupied free face's edge 
            size_t edgeIndex = 0;
            while (-1 != edges[fSd->edge[edgeIndex]].s2)
            {
                ++edgeIndex;
                if (edgeIndex == fSd->size)
                {
                    ++freeFace;
                    if (freeFace == params.faceNum )
                    {
                        std::wcerr << L"Can't find a free face" << std::endl;
                        throw std::exception();
                    }
                    std::wcout << L"Free face " << freeFace << std::endl;
                    fSd = &faces[freeFace];
                    edgeIndex = 0;
                }
            } 
            //Founded it. Use it.
            std::wcout << L"Unoccupied edge " << edgeIndex << L", free face " << faceIndex << std::endl;
            pSd->edge[0] = fSd->edge[edgeIndex];

            auto pEd = &(edges[pSd->edge[0]]);
            pEd->s2 = faceIndex;
            //fill edges and vertexes of the current face
            for (edgeIndex = 1; edgeIndex < pSd->size; ++edgeIndex)
            {
                //if the vertex have 3 connections
                vertexIndex = 3;
                do
                {
                    --vertexIndex;
                } while ((-1 != vertexes[pEd->v1].e[vertexIndex]) && (0 != vertexIndex));
                //then search a free edge of the vertex 
                if (vertexes[pEd->v1].e[vertexIndex] != -1)
                {
                    vertexIndex = 3;
                    do
                    {
                        --vertexIndex;
                    } while ((-1 != edges[vertexes[pEd->v1].e[vertexIndex]].s2) && (0 != vertexIndex));
                    if (-1 != edges[vertexes[pEd->v1].e[vertexIndex]].s2)
                    {
                        std::wcerr << L"No free edge found." << std::endl;
                        throw std::exception(); 
                    }
                    //occuping it
                    pSd->edge[edgeIndex] = vertexes[pEd->v1].e[vertexIndex];
                    pEd = &edges[vertexes[pEd->v1].e[vertexIndex]];
                    pEd->s2 = faceIndex;
                    std::wcout << L"Edge occupied by the face " << pEd->s1 << L", edge" 
                        << pSd->edge[edgeIndex] << L", edge index on face " << edgeIndex << std::endl;
                }
                else
                {
                    break;
                }
            }
            //If all edges occupied, then continue
            if (edgeIndex == pSd->size)
            {
                //проверим, а туда ли мы пришли
                if (edges[pSd->edge[0]].v2 != edges[pSd->edge[pSd->size - 1]].v1)
                {
                    std::wcerr << L"Face " << faceIndex << L" did not close." << std::endl;
                    throw std::exception(); 
                }
                else std::wcout << L"Ok! The graph is closed." << std::endl;
                continue;
            }
            edgeBegin = edgeIndex;
            //revers order
            pEd = &edges[pSd->edge[0]];
            for (edgeIndex = pSd->size - 1; edgeIndex >= 2; --edgeIndex)
            {
                //if vertex have three connection
                vertexIndex = 3;
                do
                {
                    --vertexIndex;
                } while ((-1 != vertexes[pEd->v2].e[vertexIndex]) && (0 != vertexIndex));

                //then search a free edge
                if (-1 != vertexes[pEd->v2].e[vertexIndex])
                {
                    vertexIndex = 3;
                    do
                    {
                        --vertexIndex;
                    } while ((-1 != edges[vertexes[pEd->v2].e[vertexIndex]].s2) && (0 != vertexIndex));
                    if (edges[vertexes[pEd->v2].e[vertexIndex]].s2 != -1)
                    {
                        std::wcerr << L"No free edge found." << std::endl;
                        throw std::exception();
                    }
                    //occuping it
                    pSd->edge[edgeIndex] = vertexes[pEd->v2].e[vertexIndex];
                    pEd = &edges[vertexes[pEd->v2].e[vertexIndex]];
                    pEd->s2 = faceIndex;
                    std::wcout << L"The edge occupied by the face " << pEd->s1 << L", edge" << pSd->edge[edgeIndex] 
                        << L", edge index on the face " << edgeIndex << std::endl;
                    if (pEd->s1 == targetFace)
                    {
                        std::wcout << L"Cycle " << targetFace << std::endl;
                        cycle = true;
                    }
                }
                else
                {
                    break;
                }
            }
            //creating the face's new edges and vertexes
            edgeEnd = edgeIndex;
            vertexes[edges[pSd->edge[edgeBegin - 1]].v1].e[2] = edges.size();
            for (edgeIndex = edgeBegin; edgeIndex < edgeEnd; ++edgeIndex)
            {
                edges.resize(edges.size()+1);
                pEd = &edges.back();
                pSd->edge[edgeIndex] = edges.size() - 1;
                pEd->s1 = faceIndex;
                pEd->s2 = -1;
                if (edgeIndex == edgeBegin) pEd->v1 = edges[pSd->edge[edgeIndex - 1]].v1;
                else pEd->v1 = edges[pSd->edge[edgeIndex - 1]].v2;

                vertexes.resize(vertexes.size() + 1);
                auto pVr = vertexes.end() - 1;
                pEd->v2 = vertexes.size() - 1;
                pVr->e[0] = edges.size() - 1;
                pVr->e[1] = edges.size();
                pVr->e[2] = -1;
                pVr->e[3] = curentCycle;
                pVr->e[4] = curentVertex;
                pVr->e[5] = 0;
                ++curentVertex;
            }
            //creating the face's last edge 
            edges.resize(edges.size() + 1);
            pEd = &edges.back();
            pSd->edge[edgeIndex] = edges.size() - 1;
            pEd->s1 = faceIndex;
            pEd->s2 = -1;

            if (edgeIndex == pSd->size - 1) pEd->v2 = edges[pSd->edge[0]].v2;
            else pEd->v2 = edges[pSd->edge[edgeIndex + 1]].v2;

            if (edgeBegin == edgeEnd)
            {
                pEd->v1 = edges[pSd->edge[edgeIndex - 1]].v1;
                vertexes[pEd->v1].e[2] = edges.size() - 1;
                vertexes[pEd->v2].e[2] = edges.size() - 1;
            }
            else
            {
                pEd->v1 = edges[pSd->edge[edgeIndex - 1]].v2;
                vertexes[pEd->v2].e[2] = edges.size() - 1;
            }
            // main loop of creating faces
        }
        //marking last face
        pSd = &faces[params.faceToExpand];
        ++curentCycle;
        if ((edges[pSd->edge[0]].v1 == edges[pSd->edge[1]].v1)
            || (edges[pSd->edge[0]].v1 == edges[pSd->edge[1]].v2))
        {
            vertexes[edges[pSd->edge[0]].v2].e[5] = 1000;
            vertexes[edges[pSd->edge[0]].v2].e[4] = 0;
            vertexes[edges[pSd->edge[0]].v2].e[3] = curentCycle;
        }
        else
        {
            vertexes[edges[pSd->edge[0]].v1].e[5] = 1000;
            vertexes[edges[pSd->edge[0]].v1].e[4] = 0;
            vertexes[edges[pSd->edge[0]].v1].e[3] = curentCycle;
        }
        for (size_t edgeIndex = 1; edgeIndex < pSd->size; ++edgeIndex)
        {
            if ((edges[pSd->edge[edgeIndex]].v1 == edges[pSd->edge[edgeIndex - 1]].v1)
                || (edges[pSd->edge[edgeIndex]].v1 == edges[pSd->edge[edgeIndex - 1]].v2))
            {
                vertexes[edges[pSd->edge[edgeIndex]].v1].e[5] = 1000;
                vertexes[edges[pSd->edge[edgeIndex]].v1].e[4] = edgeIndex;
                vertexes[edges[pSd->edge[edgeIndex]].v1].e[3] = curentCycle;
            }
            else
            {
                vertexes[edges[pSd->edge[edgeIndex]].v2].e[5] = 1000;
                vertexes[edges[pSd->edge[edgeIndex]].v2].e[4] = edgeIndex;
                vertexes[edges[pSd->edge[edgeIndex]].v2].e[3] = curentCycle;
            }
        }
        //creating output graph
        graph.resize(vertexes.size());
        for (vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            graph[vertexIndex].e[4] = vertexIndex;
            graph[vertexIndex].e[2] = vertexes[vertexIndex].e[3];
            graph[vertexIndex].e[3] = vertexes[vertexIndex].e[4];
            graph[vertexIndex].e[5] = vertexes[vertexIndex].e[5];
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
        for (vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            curentVertex = graph[vertexIndex].e[4];
            for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
            {
                if (edges[vertexes[curentVertex].e[edgeIndex]].v1 == curentVertex)
                    edges[vertexes[curentVertex].e[edgeIndex]].v1 = vertexIndex;
                else edges[vertexes[curentVertex].e[edgeIndex]].v2 = vertexIndex;
            }
        }
        //creating vertexes output array
        for (vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            auto pVr = &(graph[vertexIndex]);
            curentVertex = pVr->e[4];
            for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
            {
                if (edges[vertexes[curentVertex].e[edgeIndex]].v1 == vertexIndex)
                    pVr->e[edgeIndex] = edges[vertexes[curentVertex].e[edgeIndex]].v2;
                else pVr->e[edgeIndex] = edges[vertexes[curentVertex].e[edgeIndex]].v1;
            }
            pVr->e[3] = vertexes[curentVertex].e[3];
            pVr->e[4] = vertexes[curentVertex].e[4];
            pVr->e[5] = vertexes[curentVertex].e[5];
        }
        //creating edges output array
        for (faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
        {
            pSd = &faces[faceIndex];
            if ((edges[pSd->edge[0]].v1 == edges[pSd->edge[1]].v1)
                || (edges[pSd->edge[0]].v1 == edges[pSd->edge[1]].v2))
            {
                pSd->edge[0] = edges[pSd->edge[0]].v2;
            }
            else
            {
                pSd->edge[0] = edges[pSd->edge[0]].v1;
            }
            for (size_t edgeIndex = 1; edgeIndex < pSd->size; ++edgeIndex)
            {
                if ((edges[pSd->edge[edgeIndex]].v1 == edges[pSd->edge[edgeIndex - 1]].v1)
                    or (edges[pSd->edge[edgeIndex]].v1 == edges[pSd->edge[edgeIndex - 1]].v2))
                {
                    pSd->edge[edgeIndex] = edges[pSd->edge[edgeIndex]].v1;
                }
                else
                {
                    pSd->edge[edgeIndex] = edges[pSd->edge[edgeIndex]].v2;
                }
            }
        }
        std::wcout << L"The graph is complete." << std::endl;
     
     }
    catch (...)
    {
        std::wcerr << L"Error! The graph is not built. " << std::endl;
        return Result::FAIL;
    }
    return Result::OK;
}