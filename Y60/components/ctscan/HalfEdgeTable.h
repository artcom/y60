
#ifndef HalfEdgeTable_included_
#define HalfEdgeTable_included_

#include <vector>

class HalfEdgeTable {
    public:
        enum Type {
            INTERNAL,
            MIN_X,
            MAX_X,
            MIN_Y,
            MAX_Y,
            MIN_Z,
            MAX_Z
        };
        struct HalfEdgeNeighbor {
            Type type;
            int internal_index;
        };
        struct CubeCase {
            std::vector<HalfEdgeNeighbor> neighbors;
        };
        std::vector<CubeCase> cubeCases;
        HalfEdgeTable();
    private:
        HalfEdgeTable::Type classifyEdge(int theCurEdge, int theNextEdge); 
        bool intInArray(int theValue, const int * theArray);
        int calcInnerEdge(const int * theFace, int theCurEdge, int theNextEdge);
};

#endif
