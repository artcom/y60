//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================


#ifndef _ac_ctscan_Mesh_h_
#define _ac_ctscan_Mesh_h_

#include <dom/Nodes.h>
#include "EdgeList.h"
#include "ErrorMap.h"
#include <map>
#include <set>
#include <sigc++/sigc++.h>
#include <glibmm/ustring.h>

namespace y60 {
/**
 * Mesh operations on a Shape Node.
 * @ingroup Y60componentsctscan
 */ 
class Mesh {
public:
    Mesh(dom::NodePtr theShapeNode);
    unsigned colorize(unsigned int theStartIndex, unsigned int theColorIndex);
    bool edgeCollapse(unsigned theIndex);
    unsigned deleteIndicesByColor(unsigned int theColor);
    unsigned deleteDanglingVertices();
    bool check() const;
    void lockWrite();
    void unlockWrite(bool theForceFlag = false);
    void computeError();
    unsigned colorizeError(float theMaximumError, unsigned theColor);
    unsigned collapseByError(float theMaxError);
    static bool test(dom::NodePtr theShapeNode);
    void setColor(unsigned theColor);
    DEFINE_EXCEPTION(InconsistencyException, asl::Exception);
    static unsigned colorSweptSphere(const asl::Sphere<float> & theSphere, 
        const asl::Vector3f & theMotion, 
        const dom::NodePtr & theTransformationRoot, unsigned int theColor);
    void notifyProgress(double theProgress, const std::string & theMessage);
    /** Progress signal for simplification */
    sigc::signal<void, double, Glib::ustring> signal_progress() const { return _myProgressSignal; }
    void setSimplifyProceed(bool theSimplifySwitch) { _mySimplifyMode = theSimplifySwitch; }
    bool getSimplifyProceed() const { return _mySimplifyMode; }
private:
    int deleteFace(unsigned int theIndex);
    float calcEdgeError(unsigned theIndex);
    void findHalfEdges(int thePosition, unsigned theDistance, std::set<int> & theDirtySet);
    unsigned generateErrorMap(float theMaxError);
    unsigned eraseErrorMap(float theValue, int theIndex);
    int recalcEdgeErrorRecursivly(int theIndex);
    void updateErrorMap(float theOldValue, float theNewValue, int theIndex);
    unsigned insertErrorMap(float theNewValue, int theIndex);

    dom::NodePtr                    _myShapeNode;
    dom::NodePtr                    _myElementsNode;
    dom::NodePtr                    _myHalfEdgesNode;
    dom::NodePtr                    _myColorIndexNode;
    dom::NodePtr                    _myNormalIndexNode;
    dom::NodePtr                    _myPositionIndexNode;
    dom::NodePtr                    _myVertexDataNode;
    dom::NodePtr                    _myPositionVerticesNode;
    dom::NodePtr                    _myNormalVerticesNode;
    //std::vector<dom::NodePtr>       _myIndexNodes;
    EdgeListPtr                     _myEdgeList;

    std::vector<asl::Vector3f> *    _myNormalVertices;
    std::vector<asl::Vector3f> *    _myPositionVertices;
    VectorOfUnsignedInt *           _myPositions;
    VectorOfUnsignedInt *           _myNormals;
    VectorOfUnsignedInt *           _myColors;
    VectorOfSignedInt *             _myHalfEdges;
    int                             _myWriteLockCount;
    // std::vector<float>              _myEdgeError;
    const static int                _myFaceSize = 3;
    // typedef std::map<float, std::set<int> > ErrorMap;
    ErrorMap                        _myErrorMap;
    float                           _myMaxError;
    // unsigned                        _myErrors;
    sigc::signal<void, double, Glib::ustring>  _myProgressSignal;
    bool                            _mySimplifyMode;
};

}

#endif // _ac_ctscan_Mesh_h_
