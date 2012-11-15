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

//own header
#include "ShaderLibrary.h"

#include "ShaderLibraryxsd.h"
#include "FFShader.h"

#ifndef _AC_NO_CG_
#include "CgShader.h"
#include "SkinAndBonesShader.h"
#endif

#include <y60/base/iostream_functions.h>
#include <y60/base/NodeNames.h>
#include <y60/base/property_functions.h>
#include <y60/base/NodeValueNames.h>
#include <asl/dom/Nodes.h>

#include <asl/zip/PackageManager.h>
#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/os_functions.h>


using namespace std;
using namespace asl;
using namespace y60;
using namespace dom;

#define DB(x) //x

namespace y60 {
    bool ShaderLibrary::_myGLisReadyFlag = false;

    ShaderLibrary::ShaderLibrary() {
        // Cg Context created by CgContextHolder
    }

    ShaderLibrary::~ShaderLibrary()  {
        AC_DEBUG << "ShaderLibrary::~ShaderLibrary()";
        _myShaders.clear(); // destroy all shaders first
    }

#ifndef _AC_NO_CG_
    CGcontext
    ShaderLibrary::getCgContext() {
        return _myCgContext.get();
    }

    CgContextHolder::~CgContextHolder() {
        if (_myCgContext) {
            AC_DEBUG << "~CgContextHolder() - destroying cg context";
            cgDestroyContext(_myCgContext);
        }
    }
#endif

    void
    ShaderLibrary::load(const std::string & theLibraryFileName, std::string theVertexProfileName, std::string theFragmentProfileName) {
        AC_DEBUG << "ShaderLibrary::load: Loading shader library '" << theLibraryFileName << "'";

        asl::PackageManagerPtr myPackageManager = AppPackageManager::get().getPtr();
        string myShaderLibraryFileName = myPackageManager->searchFile(theLibraryFileName);
        if (myShaderLibraryFileName.empty()) {
            throw ShaderLibraryException(string("Could not find library '") + theLibraryFileName + "' in " +
                    AppPackageManager::get().getPtr()->getSearchPath(), PLUS_FILE_LINE);
        }
        myPackageManager->add(asl::getDirectoryPart(myShaderLibraryFileName));

        string myShaderLibraryStr;
        asl::readFile(myShaderLibraryFileName, myShaderLibraryStr);
        if (myShaderLibraryStr.empty()) {
            throw ShaderLibraryException(string("Could not load library '") + myShaderLibraryFileName + "'",
                    PLUS_FILE_LINE);
        }
        dom::Document myShaderLibraryXml;
        asl::Ptr<dom::ValueFactory> myFactory = asl::Ptr<dom::ValueFactory>(new dom::ValueFactory());
        dom::registerStandardTypes(*myFactory);
        registerSomTypes(*myFactory);
        myShaderLibraryXml.setValueFactory(myFactory);
        dom::Document mySchema(y60::ourShaderLibraryxsd);

        myShaderLibraryXml.addSchema(mySchema,"");
        myShaderLibraryXml.parse(myShaderLibraryStr);

        load(myShaderLibraryXml.childNode(SHADER_LIST_NAME), theVertexProfileName, theFragmentProfileName);
        AC_INFO << "Loaded Shaderlibrary " << myShaderLibraryFileName;
    }

    void
    ShaderLibrary::reload() {
        for (GLShaderVector::size_type i = 0; i < _myShaders.size(); ++i) {
            if (_myShaders[i]->isLoaded()) {
                _myShaders[i]->unload();
                _myShaders[i]->load(*this);
            }
        }
    }

    void
    ShaderLibrary::load(const dom::NodePtr theNode, std::string theVertexProfileName, std::string theFragmentProfileName) {

        AC_TRACE << "Library wants vertex profile '" << theVertexProfileName << "', fragment profile '" << theFragmentProfileName << "'";

        // determine vertex shader profile name according to following priority:
        // 1) function args, 2) environment var. 3) ask Cg library

        _myVertexProfileName = "";
        if (theVertexProfileName == "") {
            if (!asl::get_environment_var("Y60_VERTEX_SHADER_PROFILE", _myVertexProfileName)) {
                if (ShaderLibrary::GLisReady()) {
                    CGprofile myShaderProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
                    if(myShaderProfile != CG_PROFILE_UNKNOWN) {
                        _myVertexProfileName = cgGetProfileString(myShaderProfile);
                    }
                }
                if(_myVertexProfileName == "") {
                    AC_ERROR << "Could not determine vertex shader profile, must open a render window before loading shader library, falling back to 'arbvp1' profile";
                    _myVertexProfileName = "arbvp1";
                }
            }
        } else {
            _myVertexProfileName = theVertexProfileName;
        }

        _myFragmentProfileName = "";
        if (theFragmentProfileName == "") {
            if (!asl::get_environment_var("Y60_FRAGMENT_SHADER_PROFILE", _myFragmentProfileName)) {
                if (ShaderLibrary::GLisReady()) {
                    CGprofile myShaderProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
                    if(myShaderProfile != CG_PROFILE_UNKNOWN) {
                        _myFragmentProfileName = cgGetProfileString(myShaderProfile);
                    }
                }
                if(_myFragmentProfileName == "") {
                    AC_ERROR << "Could not determine fragment shader profile, must open a render window before loading shader library, falling back to 'arbfp1' profile";
                    _myFragmentProfileName = "arbfp1";
                }
            }
        } else {
            _myFragmentProfileName = theFragmentProfileName;
        }

        AC_INFO << "Engine wants vertex profile '" << _myVertexProfileName << "', fragment profile '" << _myFragmentProfileName << "'";

        //XXX: review, maybe just printing an error and using a known profile, is the proper way to handle unknown cg profiles
        try {
            /*int myVertexShaderProfile =*/ asl::getEnumFromString(_myVertexProfileName, ShaderProfileStrings); // just for parameter check
        } catch(const asl::ParseException) {
            throw ShaderException(std::string("wanted cg vertex profile:") + _myVertexProfileName + std::string(" is unknown & unsupported by the engine"),PLUS_FILE_LINE);
        }
        try {
            /*int myFragmentShaderProfile =*/ asl::getEnumFromString(_myFragmentProfileName, ShaderProfileStrings); // just for parameter check
        } catch(const asl::ParseException) {
            throw ShaderException(std::string("wanted cg fragment profile:") + _myFragmentProfileName + std::string(" is unknown & unsupported by the engine"),PLUS_FILE_LINE);
        }

        for (NodeList::size_type i = 0; i < theNode->childNodesLength("shader"); i++) {
            const dom::NodePtr theShaderNode = theNode->childNode("shader", i);
            GLShaderPtr myGLShader;
            DB(AC_TRACE << "loading shader " << theShaderNode->getAttributeString("name") << endl);
            if (theShaderNode->childNode(FIXED_FUNCTION_SHADER_NODE_NAME)) {
                myGLShader = GLShaderPtr(new FFShader(theShaderNode));
            }
#ifndef _AC_NO_CG_
            else if (theShaderNode->childNode(VERTEX_SHADER_NODE_NAME) &&
                    theShaderNode->getAttributeString(NAME_ATTRIB) == "SkinAndBones") {
                myGLShader = GLShaderPtr(new SkinAndBonesShader(theShaderNode, _myVertexProfileName, _myFragmentProfileName));
            } else if (theShaderNode->childNode(VERTEX_SHADER_NODE_NAME)) {
                myGLShader = GLShaderPtr(new CgShader(theShaderNode, _myVertexProfileName, _myFragmentProfileName));
            }
#endif
            if (myGLShader) {
                if (myGLShader->isSupported()) {
                    _myShaders.push_back(myGLShader);
                } else {
                    AC_WARNING << "Cg Shader '"<< theShaderNode->getAttributeString("name") << "' is not supported by engine profiles " << _myVertexProfileName << "/"<< _myFragmentProfileName;
                }
            } else {
                throw ShaderException(string("Can't determine shader type for shader with id '") +
                        theShaderNode->getAttributeString(ID_ATTRIB) + "'",PLUS_FILE_LINE);
            }
        }
    }

    void
    ShaderLibrary::load() {
        if (_myShaderLibraryNames.size() == 0) {
            _myShaderLibraryNames.push_back("shaderlibrary.xml");
            _myVertexProfileNames.push_back("");
            _myFragmentProfileNames.push_back("");
        }
        for (unsigned i = 0; i < _myShaderLibraryNames.size(); ++i) {
            load(_myShaderLibraryNames[i], _myVertexProfileNames[i], _myFragmentProfileNames[i]);
        }
        _myShaderLibraryNames.clear();
        _myVertexProfileNames.clear();
        _myFragmentProfileNames.clear();
    }

    IShaderPtr
    ShaderLibrary::findShader(MaterialBasePtr theMaterial) {
        DB(AC_DEBUG << "ShaderLibrary::findShader for material: " << theMaterial->getNode());
        // we need to copy all values to tmp RequirementMap, cause we want to drop req temporarly

        MaterialRequirementFacadePtr myReqFacade = theMaterial->getChild<MaterialRequirementTag>();
        Facade::PropertyMap & myRequirementMap = myReqFacade->getProperties();
        vector<ShaderScore> myScoreBoard(_myShaders.size());

        // iterate over all features, that the material wants to have
        vector<string> myDropRequirements;

        Facade::PropertyMap::iterator myIter = myRequirementMap.begin();
        for (; myIter != myRequirementMap.end(); myIter++) {
            const NodePtr myRequirementNode = myIter->second;
            const std::string & myRequiredFeatureClass = myIter->first;
            VectorOfRankedFeature myRequiredFeature = myRequirementNode->nodeValueAs<VectorOfRankedFeature>();
            bool oneShaderCanHandleMaterial = false;
            bool myFeatureCanBedropped = false;
            AC_DEBUG << "Class : " << myRequiredFeatureClass << " , requires: " << asl::as_string(myRequiredFeature) << endl;
            for (VectorOfRankedFeature::size_type j = 0; j < myRequiredFeature.size(); j++) {
                const VectorOfString & myRequiredFeatureCombo = myRequiredFeature[j]._myFeature;

                for (GLShaderVector::size_type i = 0 ; i < _myShaders.size(); i++) {
                    GLShaderPtr myGLShader = _myShaders[i];
                    GLShader::ShaderMatch myShaderMatchResult = myGLShader->matches(myRequiredFeatureClass, myRequiredFeatureCombo);
                    if (myShaderMatchResult != GLShader::NO_MATCH) {
                        DB(AC_DEBUG << "Match: Shader '"<<myGLShader->getName()<<"' matches "<< asl::as_string(myRequiredFeatureCombo) << " in class '"<<asl::as_string(myRequiredFeatureClass)<<"'"<<endl);
                        myScoreBoard[i].featurehits++;
                        myScoreBoard[i].points += (myGLShader->getCosts()) == 0 ?
                            0.0f : ( myRequiredFeature[j]._myRanking /
                            myGLShader->getCosts());
                        // if the shader has a full match add another score point
                        // wildcard match will have a lower score (vs)
                        myScoreBoard[i].points += myShaderMatchResult == GLShader::FULL_MATCH ? 1 : 0;
                        oneShaderCanHandleMaterial = true;

                    } else {
                        DB(AC_DEBUG << "Shader '"<<myGLShader->getName()<<"' does not match "<< asl::as_string(myRequiredFeatureCombo) << " in class '"<<asl::as_string(myRequiredFeatureClass)<<"'"<<endl);
                        if ( myRequiredFeatureCombo.size() == 1 &&
                            myRequiredFeatureCombo[0] == FEATURE_DROPPED) {
                                myFeatureCanBedropped = true;
                        }
                    }
                }
            }
            if (myFeatureCanBedropped) {
                AC_INFO << "Material: " << theMaterial->get<NameTag>() << ": Feature can be dropped: "<<asl::as_string(myRequiredFeatureClass)<<endl;
                if (!oneShaderCanHandleMaterial){
                    AC_INFO << "          and no shader found to handle it: drop it!!!" <<endl;
                    // feature dropped, erase it from requirements
                    myDropRequirements.push_back(myRequiredFeatureClass);
                } else {
                    AC_INFO << "          but we found a shader to handle it: keep it!" << endl;
                }
            }
        }

        // remove all the dropped features
        for (vector<string>::size_type i = 0; i <  myDropRequirements.size(); i++ ) {
            const string & myFeatureToDrop = myDropRequirements[i];
            Facade::PropertyMap::iterator it = myRequirementMap.find(myFeatureToDrop);
            if (it != myRequirementMap.end()) {
                AC_WARNING << "### WARNING Dropping feature: " <<  myFeatureToDrop
                    << " of material: " << theMaterial->get<NameTag>() << endl;
                myRequirementMap.erase(it);
            }
        }
        // find the shader that supports all features required (the score equals the feature count)
        // implement a magic algorithm to find the best shader that serves the requirements

        AC_DEBUG << "---- Shader search rubberpoint scoreboard for material: " << theMaterial->get<NameTag>() << " ----" << endl;
        for (vector<ShaderScore>::size_type i = 0; i <myScoreBoard.size(); i++ )
        {
            const ShaderFeatureSet & myShaderFeatureSet = _myShaders[i]->getFeatureSet();
            bool allShaderFeatureUsed = myShaderFeatureSet.getFeatureCount() == myRequirementMap.size();
            DB(AC_DEBUG << "Shader: " << _myShaders[i]->getName() << " scored: "
                << myScoreBoard[i].featurehits << " features"
                << " and " << myScoreBoard[i].points << " points");
            if (allShaderFeatureUsed) {
                DB(AC_DEBUG << " and all its features could be used." << endl);
            } else {
                DB(AC_DEBUG << " but its feature support is unacceptable." << endl);
            }
        }

        GLShaderPtr myLeadingShader;
        float myMaxPoints = 0.0;
        for (vector<ShaderScore>::size_type i = 0; i < myScoreBoard.size(); i++ ) {
            const ShaderFeatureSet & myShaderFeatureSet = _myShaders[i]->getFeatureSet();
            // all material requirements must be matched with the shaders feature
            bool matchAllMaterialRequirements = myScoreBoard[i].featurehits == myRequirementMap.size();
            // get the shader with points higher than the leading shader
            bool shaderHasHigherScore = myScoreBoard[i].points > myMaxPoints;
            // shader must not be oversized, all its features must be required
            bool allShaderFeatureUsed = myShaderFeatureSet.getFeatureCount() == myRequirementMap.size();

            DB( {AC_DEBUG << endl;}
                {AC_DEBUG << "Shader: " << _myShaders[i]->getName() << " scored: " <<endl;}
                {AC_DEBUG << myScoreBoard[i].featurehits << " features of" << " of " << myRequirementMap.size() <<" requirements"<<endl;}
                {AC_DEBUG << myShaderFeatureSet.getFeatureCount() << " shader features" << " of " << myRequirementMap.size() <<" requirements"<<endl;}
                {AC_DEBUG<< " Points: " << myScoreBoard[i].points << " points"<<", best = "<<myMaxPoints<<endl;});
            if (allShaderFeatureUsed) {
                DB(AC_DEBUG << " and all its features could be used." << endl);
            } else {
                DB(AC_DEBUG << " but its feature support is unacceptable." << endl);
            }

            if ( matchAllMaterialRequirements && shaderHasHigherScore &&  allShaderFeatureUsed )
            {
                DB(AC_DEBUG << "Better shader : " << _myShaders[i]->getName() << endl);
                myMaxPoints = myScoreBoard[i].points;
                myLeadingShader = _myShaders[i];
            } else {
                DB(AC_DEBUG << "Worse shader : " << _myShaders[i]->getName() << endl);
            }
        }
        if (myLeadingShader != 0) {
            AC_DEBUG << "Using shader : " << myLeadingShader->getName()
                 << " for material: "<< theMaterial->get<NameTag>()  << endl;
            return myLeadingShader;
        }
        throw ShaderLibraryException(string("No matching shader found for CgMaterial '" )
            +  theMaterial->get<NameTag>() + "', requirement = " + as_string(myReqFacade->getNode()),
                                     PLUS_FILE_LINE);
    }
}
