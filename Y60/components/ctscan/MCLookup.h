
#ifndef MCLookup_included_
#define MCLookup_included_

#include <vector>
#include <map>

class MCLookup {
    public:
        enum HalfEdgeType {
            INTERNAL,
            MIN_X,
            MAX_X,
            MIN_Y,
            MAX_Y,
            MIN_Z,
            MAX_Z
        };
        struct HalfEdgeNeighbor {
            HalfEdgeType type;
            int internal_index;
        };
        struct CubeCase {
            std::vector<HalfEdgeNeighbor> neighbors;
            std::vector<int> edges;
            std::vector<int> faces;
        };
        std::vector<CubeCase> cubeCases;
        MCLookup();
    private:
        void initFaceData();
        void initHalfEdgeData();
        void initEdgeData();
        MCLookup::HalfEdgeType classifyEdge(int theCurEdge, int theNextEdge); 
        bool intInArray(int theValue, const int * theArray);
        int calcInnerEdge(const std::vector<int> & theFaces, int theCurEdge, int theNextEdge);
        void addFaceData(int theCubeCase, const std::vector<int> & theFaces);
        void addPattern(int thePatternIndex, std::map<int,int> & theMatchedMap);
        void rotatePattern(int & theCubeCase, std::vector<int> & theFaces, const int * theCaseRotation, const int* theFaceRotation);
};

#endif
