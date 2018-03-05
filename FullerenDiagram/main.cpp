// FullerenDiagram.cpp : 
//

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <io.h>
#include <fcntl.h>

#include "fullereneTypes.h"
#include "buildFullerene.h"
#include "drawFullerene.h"
#include "paintFullerene.h"

const wchar_t*  errorMsg = L"Error! Can't ";

int parseParams(std::ifstream* _infile, Params& _params)
{
    std::vector<unsigned int> p;
    if (nullptr == _infile) return 1;
    do
    {
        auto ch = _infile->peek();
        if (' ' == ch || ',' == ch)
        {
            _infile->ignore();
        }
        unsigned int n = 0;
        (*_infile) >> n;
        p.push_back(n);
    } while (!_infile->eof());
    if (p.size() < 3)
    {
        return 2;
    }
    _params.faceNum = p[0];
    _params.faceToExpand = p[1];
    for (size_t i = 2; i < p.size(); ++i)
    {
        _params.pentagons.push_back(p[i]);
    }
    return 0;
}

int wmain(int argc, wchar_t** argv)
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);

    if (3 != argc) {
        std::wcout << L"Fullerene diagram creating\nfullerene_diagram in.txt out.svg" << std::endl;
        exit(0);
    }
    const wchar_t* infileName = argv[1];
    const wchar_t* outfileName = argv[2];

    std::ifstream infile(infileName);
    if (!infile.is_open())
    {
        std::wcerr << errorMsg << L"open file: " << infileName << std::endl;
        exit(1);
    }
    std::ofstream outfile(outfileName);
    if (!outfile.is_open())
    {
        std::wcerr << errorMsg << L"create or open file: " << outfileName << std::endl;
        exit(2);
    }
    outfile.close();
    Params params;
    if (parseParams(&infile, params) != 0)
    {
        std::wcerr << errorMsg << L"parse parametrs from file: " << infileName << std::endl;
        exit(3);
    }
    std::vector<Vertex> graph;
    std::vector<Face> faces;
    buildGraph(params, graph, faces);

    draw(graph);

    paint(outfileName, graph);

    
    return 0;
}

