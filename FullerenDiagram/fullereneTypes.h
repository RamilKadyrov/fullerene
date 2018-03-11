#pragma once
struct Face{
    int size = 6;
    int edge[6];
};

struct  Edge {
    int s1, s2, v1, v2;
    Edge()
    {
        s1 = s2 = v1 = v2 = -1;
    }
    Edge(int _s1, int _s2, int _v1, int _v2):s1(_s1), s2(_s2), v1(_v1), v2(_v2)
    {}
};

struct Vertex {
    int e[6] = {0};
    int e0, e1, e2; // edges indexes
    int cycle;
    int x, y;
    bool last_face;
    Vertex() { }
    Vertex(int e0, int e1, int e2, int e3, int e4, int e5)
    {
        
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
        e[3] = e3;
        e[4] = e4;
        e[5] = e5;
    }
    //e[0],e[1],e[2] - index (edge or vertex)
    //e[3] - cycle or x coordinate
    //e[4] - vertex index in cycle or y coordinate
    //e[5] - last face mark
};


struct Params{
    int faceNum;
    int faceToExpand;
    int nSide;
    std::vector<unsigned int> pentagons;
};

enum class Result {
    OK,
    FAIL,
};

const unsigned int enlarge = 5;

#if defined( _DEBUG) || !defined(NDEBUG)
#define LOG(msg) std::wcout << msg << std::endl
#else
#define LOG(msg)
#endif // DEBUG
#define LOG_ERROR(msg) std::wcerr << msg << std::endl



