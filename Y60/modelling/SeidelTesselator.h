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
            typedef struct {
            double x, y;
            } point_t, vector_t;


            /* Segment attributes */

            typedef struct {	
            point_t v0, v1;		/* two endpoints */
            int is_inserted;		/* inserted in trapezoidation yet ? */
            int root0, root1;		/* root nodes in Q */
            int next;			/* Next logical segment */
            int prev;			/* Previous segment */
            } segment_t;


            /* Trapezoid attributes */

            typedef struct {
            int lseg, rseg;		/* two adjoining segments */
            point_t hi, lo;		/* max/min y-values */
            int u0, u1;
            int d0, d1;
            int sink;			/* pointer to corresponding in Q */
            int usave, uside;		/* I forgot what this means */
            int state;
            } trap_t;


            /* Node attributes for every node in the query structure */

            typedef struct {
            int nodetype;			/* Y-node or S-node */
            int segnum;
            point_t yval;
            int trnum;
            int parent;			/* doubly linked DAG */
            int left, right;		/* children */
            } node_t;


            typedef struct {
            int vnum;
            int next;			/* Circularly linked list  */
            int prev;			/* describing the monotone */
            int marked;			/* polygon */
            } monchain_t;			


            typedef struct {
            point_t pt;
            int vnext[4];			/* next vertices for the 4 chains */
            int vpos[4];			/* position of v in the 4 chains */
            int nextfree;
            } vertexchain_t;

            SeidelTesselator();
            virtual ~SeidelTesselator();
            dom::NodePtr createSurface2DFromContour(y60::ScenePtr theScene, const std::string & theMaterialId,
                                                    const std::vector<VectorOfVector2f> & theContours,
                                                    const std::string & theName = "Surface2DShape");

        private:
            // monotone stuff
            monchain_t mchain[TRSIZE]; /* Table to hold all the monotone */
				  /* polygons . Each monotone polygon */
				  /* is a circularly linked list */

            vertexchain_t vert[SEGSIZE]; /* chain init. information. This */
				    /* is used to decide which */
				    /* monotone polygon to split if */
				    /* there are several other */
				    /* polygons touching at the same */
				    /* vertex  */

            int mon[SEGSIZE];	/* contains position of any vertex in */
				/* the monotone chain for the polygon */
            int visited[TRSIZE];
            int chain_idx;
            int op_idx;
            int mon_idx;

            int triangulate_monotone_polygons(int, int, int (*)[3]);
            int inside_polygon(trap_t * t);
            int newmon();
            int new_chain_element();
            double get_angle(point_t * vp0, point_t * vpnext, point_t * vp1);
            int get_vertex_positions(int v0, int v1, int * ip, int * iq);
            int make_new_monotone_poly(int mcur, int v0, int v1);
            int monotonate_trapezoids(int n);
            int traverse_polygon(int mcur, int trnum, int from, int dir);
            int triangulate_single_polygon(int nvert, int posmax, int side, int op[][3]);
            
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










    


            node_t qs[QSIZE];		/* Query structure */
            trap_t tr[TRSIZE];		/* Trapezoid structure */
            segment_t seg[SEGSIZE];		/* Segment table */


    };
    typedef asl::Ptr<SeidelTesselator, dom::ThreadingModel> SeidelTesselatorPtr;

}
#endif // AC_Y60_TESSELATION_FUNCTIONS_INCLUDED

