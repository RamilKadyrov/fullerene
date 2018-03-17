
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "fullereneTypes.h"
#include "buildFullerene.h"
#include "drawFullerene.h"
#include "paintFullerene.h"
#include "expandFullerene.h"

const char*  errorMsg = "Error! Can't ";

int parseParams(std::ifstream* _infile, Params& _params)
{
    if (nullptr == _infile) return 1;
    std::vector<unsigned int> p;
    _params.nSide = 6;

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
    _params.faceToExpand = p[1] - 1;
    for (std::size_t i = 2; i < p.size(); ++i)
    {
        if (_params.faceToExpand == static_cast<int>(p[i]) - 1 ) _params.nSide = 5;
        _params.pentagons.push_back(p[i] - 1);
    }
    return 0;
}

int main(int argc, char** argv)
{
    if (3 != argc) {
        LOG( "Fullerene diagram creating\nfullerene_diagram in.txt out.svg" );
        exit(0);
    }
    const char* infileName = argv[1];
    const char* outfileName = argv[2];

    std::ifstream infile(infileName);
    if (!infile.is_open())
    {
        LOG_ERROR( errorMsg << "open file: " << infileName );
        exit(1);
    }
    Params params;
    if (parseParams(&infile, params) != 0)
    {
        LOG_ERROR(errorMsg << "parse parametrs from file: " << infileName);
        infile.close();
        exit(3);
    }
    infile.close();
    std::ofstream outfile(outfileName);
    if (!outfile.is_open())
    {
        LOG_ERROR( errorMsg << "create or open file: " << outfileName );
        exit(2);
    }
    outfile.close();
    
    std::vector<Vertex> graph;
    Build build;

    if (build.graph(params, graph) != Result::OK)
    {
        return 1;
    }

    double gRadius = 1.0;
    if (draw(graph, gRadius) != Result::OK)
    {
        return 2;
    }

    if (expand(graph, gRadius, params.nSide) != Result::OK)
    {
        return 3;
    }

    paint(outfileName, graph);
    return 0;
}

