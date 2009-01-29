/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _ac_ctscan_Mesh_h_
#define _ac_ctscan_Mesh_h_

#include <asl/dom/Nodes.h>
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
    unsigned colorizeDisconnected(unsigned int theColorIndex);
    bool edgeCollapse(unsigned theIndex);
    unsigned deleteIndicesByColor(unsigned int theColor);
    unsigned deleteDanglingVertices();
    bool check() const;
    bool isClosed() const;
    void lockWrite();
    void unlockWrite(bool theForceFlag = false);
    bool computeError();
    unsigned colorizeError(float theMaximumError, unsigned theColor);
    unsigned collapseByError(float theMaxError);
    static bool test(dom::NodePtr theShapeNode);
    void setColor(unsigned theColor);
    DEFINE_EXCEPTION(InconsistencyException, asl::Exception);
    static unsigned colorScreenSpaceLasso(const dom::NodePtr & theLassoBody, 
                            const dom::NodePtr & theTransformationRoot,
                            const asl::Matrix4f & theViewProjection,
                            unsigned int theColor);
    static unsigned colorSweptSphere(const asl::Sphere<float> & theSphere, 
        const asl::Vector3f & theMotion, 
        const dom::NodePtr & theTransformationRoot, unsigned int theColor);
    bool notifyProgress(double theProgress, const std::string & theMessage);
    /** Progress signal for simplification */
    sigc::signal<bool, double, Glib::ustring> signal_progress() const { return _myProgressSignal; }
    void setSimplifyProceed(bool theSimplifySwitch) { _mySimplifyMode = theSimplifySwitch; }
    bool getSimplifyProceed() const { return _mySimplifyMode; }
    void removeMeshAndColor();
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
    sigc::signal<bool, double, Glib::ustring>  _myProgressSignal;
    bool                            _mySimplifyMode;
};

}

#endif // _ac_ctscan_Mesh_h_
