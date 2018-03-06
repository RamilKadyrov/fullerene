#pragma once
struct Face{
    size_t size = 6;
    int edge[6];
};

struct  Edge {
    int s1, s2, v1, v2;
};

struct Vertex {
    int e[6]; //e[0],e[1],e[2] - edge index
    //e[3] - cycle
    //e[4] - vertex index in cycle
    //e[5] - last face mark
};


struct Params{
    size_t faceNum;
    size_t faceToExpand;
    size_t nSide;
    std::vector<unsigned int> pentagons;
};

enum class Result {
    OK,
    FAIL,
};

const unsigned int enlarge = 5;