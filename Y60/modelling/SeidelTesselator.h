//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef AC_Y60_TESSELATION_FUNCTIONS_INCLUDED
#define AC_Y60_TESSELATION_FUNCTIONS_INCLUDED

#include "SeidelTesselatorUtils.h"
#include <y60/Scene.h>

#include <dom/Nodes.h>
#include <asl/Vector234.h>
#include <asl/Exception.h>

#include <string>
#include <iostream>

namespace y60 {
    DEFINE_EXCEPTION(SeidelTesselatorException, asl::Exception);




    class SeidelTesselator {
        public:
            class seidel_point{
                public:
                    seidel_point() : x(0),y(0) {}
                    double x;
                    double y;
            };
            typedef seidel_point point_t;
            typedef seidel_point vector_t;
            /*typedef struct {
                
            double x, y;
            } point_t, vector_t;
            */

            /* Segment attributes */
            
            class segment_t {	
                public:
                    segment_t() : is_inserted(0), root0(0), root1(0), next(0), prev(0) {} 
                    point_t v0;
                    point_t v1;		/* two endpoints */
                    int is_inserted;		/* inserted in trapezoidation yet ? */
                    int root0;
                    int root1;		/* root nodes in Q */
                    int next;			/* Next logical segment */
                    int prev;			/* Previous segment */
            };


            /* Trapezoid attributes */

            class trap_t {
                public:
                    trap_t() : lseg(0), rseg(0), u0(0), u1(0), d0(0), d1(0), sink(0), usave(0), uside(0), state(0) {}
                    int lseg;
                    int rseg;		/* two adjoining segments */
                    point_t hi;
                    point_t lo;		/* max/min y-values */
                    int u0;
                    int u1;
                    int d0;
                    int d1;
                    int sink;			/* pointer to corresponding in Q */
                    int usave;
                    int uside;		/* I forgot what this means */
                    int state;
            };


            /* Node attributes for every node in the query structure */

            class node_t {
                public:
                    node_t() : nodetype(0), segnum(0), trnum(0), parent(0), left(0), right(0) {}
                    int nodetype;			/* Y-node or S-node */
                    int segnum;
                    point_t yval;
                    int trnum;
                    int parent;			/* doubly linked DAG */
                    int left;
                    int right;		/* children */
            };


            class monchain_t {
                public:
                    monchain_t() : vnum(0), next(0), prev(0), marked(0) {}
                    int vnum;
                    int next;			/* Circularly linked list  */
                    int prev;			/* describing the monotone */
                    int marked;			/* polygon */
            };			


            class vertexchain_t {
                public:
                    vertexchain_t() : nextfree(0) {
                        for (int i = 0; i< 4;i++) {
                            vnext[i] = 0;
                            vpos[i] = 0;
                        }
                    }
                    point_t pt;
                    int vnext[4];			/* next vertices for the 4 chains */
                    int vpos[4];			/* position of v in the 4 chains */
                    int nextfree;
            };

            SeidelTesselator();
            virtual ~SeidelTesselator();
            dom::NodePtr createSurface2DFromContour(y60::ScenePtr theScene, const std::string & theMaterialId,
                                                    const VectorOfVector2f & theContour,
                                                    const std::string & theName = "Surface2DShape");

        private:
            // monotone stuff
            std::vector<monchain_t> mchain; /* Table to hold all the monotone */
            //monchain_t mchain[TRSIZE]; /* Table to hold all the monotone */
				  /* polygons . Each monotone polygon */
				  /* is a circularly linked list */

            std::vector<vertexchain_t> vert; /* chain init. information. This */
            //vertexchain_t vert[SEGSIZE]; /* chain init. information. This */
				    /* is used to decide which */
				    /* monotone polygon to split if */
				    /* there are several other */
				    /* polygons touching at the same */
				    /* vertex  */

            std::vector<int> mon;	/* contains position of any vertex in */
            //int mon[SEGSIZE];	/* contains position of any vertex in */
				/* the monotone chain for the polygon */
            int chain_idx;
            int op_idx;
            int mon_idx;

            int triangulate_monotone_polygons(int, int, std::vector<std::vector<int> > & op);
            int inside_polygon(trap_t * t);
            int newmon();
            int new_chain_element();
            double get_angle(point_t * vp0, point_t * vpnext, point_t * vp1);
            int get_vertex_positions(int v0, int v1, int * ip, int * iq);
            int make_new_monotone_poly(int mcur, int v0, int v1);
            int monotonate_trapezoids(int n);
            int traverse_polygon(int mcur, int trnum, int from, int dir);
            int triangulate_single_polygon(int nvert, int posmax, int side, std::vector<std::vector<int> > & op);
            
            // construct stuff
            int q_idx;
            int tr_idx;
            int construct_trapezoids(int);
            int _greater_than(point_t *, point_t *);
            int _equal_to(point_t *, point_t *);
            int _greater_than_equal_to(point_t *, point_t *);
            int _less_than(point_t *, point_t *);
            int locate_endpoint(point_t *, point_t *, int);
            int newnode();
            int newtrap();
            int _max(point_t *yval, point_t *v0, point_t *v1);
            int _min(point_t *yval, point_t *v0, point_t *v1);
            int init_query_structure(int segnum);
            int is_left_of(int segnum, point_t * v);
            int inserted(int segnum, int whichpt);
            int merge_trapezoids(int segnum, int tfirst, int tlast, int side);
            int add_segment(int segnum);
            int find_new_roots(int segnum);










    

            int choose_segment();
            int choose_idx;

            //node_t qs[QSIZE];		/* Query structure */
            std::vector<node_t> qs;		/* Query structure */

            //int visited[TRSIZE];
            std::vector<int> visited;

            //trap_t tr[TRSIZE];		/* Trapezoid structure */
            std::vector<trap_t> tr;		/* Trapezoid structure */

            std::vector<segment_t>  seg;		/* Segment table */
            //segment_t seg[SEGSIZE];		/* Segment table */


    };
    typedef asl::Ptr<SeidelTesselator, dom::ThreadingModel> SeidelTesselatorPtr;

}
#endif // AC_Y60_TESSELATION_FUNCTIONS_INCLUDED

