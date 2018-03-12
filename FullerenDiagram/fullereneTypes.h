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
    int e[3];  // edges indexes
    int cycle;
    int x, y, r;
    size_t cycleVertexIndex;
    size_t index;
    bool lastFace;

    Vertex() { }
    
    Vertex(int _e0, int _e1, int _e2, int _cycle, size_t _index, bool _lastFace)
    {
        e[0] = _e0;
        e[1] = _e1;
        e[2] = _e2;
        cycle = _cycle;
        cycleVertexIndex = _index;
        lastFace = _lastFace;
    }
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



