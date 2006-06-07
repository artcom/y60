//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: ShaderLibrary.cpp,v 1.9 2005/04/24 00:41:18 pavel Exp $
//   $RCSfile: ShaderLibrary.cpp,v $
//   $Author: pavel $
//   $Revision: 1.9 $
//   $Date: 2005/04/24 00:41:18 $
//
//
//
//=============================================================================

#include "ShaderLibrary.h"
#include "ShaderLibrary_xsd.h"
#include "FFShader.h"

#ifndef _AC_NO_CG_
#include "CGShader.h"
#endif

#include <y60/NodeNames.h>
#include <y60/property_functions.h>
#include <y60/NodeValueNames.h>
#include <y60/iostream_functions.h>
#include <dom/Nodes.h>

#include <asl/PackageManager.h>
#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <asl/string_functions.h>


using namespace std;
using namespace asl;
using namespace y60;
using namespace dom;

#define DB(x) //x

namespace y60 {

	ShaderLibrary::ShaderLibrary() 
#ifndef _AC_NO_CG_
	: _myCgContext(0)
    {
        _myCgContext = cgCreateContext();
        assertCg("ShaderLibrary::ShaderLibrary() - cgCreateContext()", _myCgContext);
    }
#else
	{ }
#endif

    ShaderLibrary::~ShaderLibrary()  {
        _myShaders.clear(); // destroy all shaders first
#ifndef _AC_NO_CG_ 
        cgDestroyContext(_myCgContext);
        assertCg("ShaderLibrary::~ShaderLibrary() - cgDestroyContext()", _myCgContext);
#endif
    }
#ifndef _AC_NO_CG_
    CGcontext
	ShaderLibrary::getCgContext() {
		return _myCgContext;
	}
#endif

    void
    ShaderLibrary::load(const std::string & theLibraryFileName) {
        AC_DEBUG << "Loading shader library: " << theLibraryFileName;
        string myShaderLibraryFileName = AppPackageManager::get().getPtr()->searchFile(theLibraryFileName);
        if (myShaderLibraryFileName.empty()) {
            throw ShaderLibraryException(string("Could not find library '") + theLibraryFileName + "' in " +
                AppPackageManager::get().getPtr()->getSearchPath(), PLUS_FILE_LINE);
        }
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
        dom::Document mySchema(ourShaderLibrary_xsd);

        myShaderLibraryXml.addSchema(mySchema,"");
        myShaderLibraryXml.parse(myShaderLibraryStr);

        load(myShaderLibraryXml.childNode(SHADER_LIST_NAME));
        std::string myShaderDir = asl::getDirectoryPart(myShaderLibraryFileName);
        _myShaderDirectory = myShaderDir;
        AC_INFO << "Loaded Shaderlibrary " + myShaderLibraryFileName;
    }

    void
    ShaderLibrary::reload() {
        for (int i = 0; i < _myShaders.size(); ++i) {
            if (_myShaders[i]->isLoaded()) {
                _myShaders[i]->unload();
                _myShaders[i]->load(*this);
            }
        }
    }

    void
    ShaderLibrary::load(const dom::NodePtr theNode) {
        _myShaderList = theNode;
        for (int i = 0; i < _myShaderList->childNodesLength("shader"); i++) {
            const dom::NodePtr theShaderNode =  _myShaderList->childNode("shader", i);
            GLShaderPtr myGLShader(0);
            DB(AC_TRACE << "loading shader " << theShaderNode->getAttributeString("name") << endl);
            if (theShaderNode->childNode(FIXED_FUNCTION_SHADER_NODE_NAME)) {
                myGLShader = GLShaderPtr(new FFShader(theShaderNode));
            }
#ifndef _AC_NO_CG_
		    else if (theShaderNode->childNode(VERTEX_SHADER_NODE_NAME)) {
                myGLShader = GLShaderPtr(new CGShader(theShaderNode));
            }
#endif
            if (myGLShader) {
                _myShaders.push_back(myGLShader);
            } else {
                throw ShaderException(string("Can't determine shader type for shader with id '") +
                    theShaderNode->getAttributeString(ID_ATTRIB) + "'",PLUS_FILE_LINE);
            }
        }
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
            DB(AC_DEBUG << "Class : " << myRequiredFeatureClass << " , requires: " << myRequiredFeature << endl);
            for (int j = 0; j < myRequiredFeature.size(); j++) {
                const VectorOfString & myRequiredFeatureCombo = myRequiredFeature[j]._myFeature;

                for (int i = 0 ; i < _myShaders.size(); i++) {
                    GLShaderPtr myGLShader = _myShaders[i];
					GLShader::ShaderMatch myShaderMatchResult = myGLShader->matches(myRequiredFeatureClass, myRequiredFeatureCombo);
					if (myShaderMatchResult != GLShader::NO_MATCH) {
                        DB(AC_DEBUG << "Match: Shader '"<<myGLShader->getName()<<"' matches "<< myRequiredFeatureCombo << " in class '"<<myRequiredFeatureClass<<"'"<<endl);
                        myScoreBoard[i].featurehits++;
                        myScoreBoard[i].points += (myGLShader->getCosts()) == 0 ?
                                                  0.0f : ( myRequiredFeature[j]._myRanking /
                                                           myGLShader->getCosts());
						// if the shader has a full match add another score point
						// wildcard match will have a lower score (vs)
						myScoreBoard[i].points += myShaderMatchResult == GLShader::FULL_MATCH ? 1 : 0;
                        oneShaderCanHandleMaterial = true;

                    } else {
                        DB(AC_DEBUG << "Shader '"<<myGLShader->getName()<<"' does not match "<< myRequiredFeatureCombo << " in class '"<<myRequiredFeatureClass<<"'"<<endl);
                        if ( myRequiredFeatureCombo.size() == 1 &&
                            myRequiredFeatureCombo[0] == FEATURE_DROPPED) {
                                myFeatureCanBedropped = true;
                            }
                    }
                }
            }
            if (myFeatureCanBedropped) {
                AC_INFO << "Material: " << theMaterial->get<NameTag>() << ": Feature can be dropped: "<<myRequiredFeatureClass<<endl;
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
        for (int i = 0; i <  myDropRequirements.size(); i++ ) {
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
        DB(
            AC_DEBUG << "---- Shader search rubberpoint scoreboard for material: " << theMaterial->get<NameTag>() << " ----" << endl;
            for (int i = 0; i <myScoreBoard.size(); i++ )
            {
                const ShaderFeatureSet & myShaderFeatureSet = _myShaders[i]->getFeatureSet();
                bool allShaderFeatureUsed = myShaderFeatureSet.getFeatureCount() == myRequirementMap.size();
                AC_DEBUG << "Shader: " << _myShaders[i]->getName() << " scored: "
                    << myScoreBoard[i].featurehits << " features"
                    << " and " << myScoreBoard[i].points << " points";
                if (allShaderFeatureUsed) {
                    AC_DEBUG << " and all its features could be used." << endl;
                } else {
                    AC_DEBUG << " but its feature support is unacceptable." << endl;
                }
            }
        ) // end DB2

        GLShaderPtr myLeadingShader(0);
        float myMaxPoints = 0.0;
        for (int i = 0; i < myScoreBoard.size(); i++ ) {
            const ShaderFeatureSet & myShaderFeatureSet = _myShaders[i]->getFeatureSet();
            // all material requirements must be matched with the shaders feature
            bool matchAllMaterialRequirements = myScoreBoard[i].featurehits == myRequirementMap.size();
            // get the shader with points higher than the leading shader
            bool shaderHasHigherScore = myScoreBoard[i].points > myMaxPoints;
            // shader must not be oversized, all its features must be required
            bool allShaderFeatureUsed = myShaderFeatureSet.getFeatureCount() == myRequirementMap.size();

            DB(
                AC_DEBUG << endl;
                AC_DEBUG << "Shader: " << _myShaders[i]->getName() << " scored: " <<endl;
                AC_DEBUG << myScoreBoard[i].featurehits << " features of" << " of " << myRequirementMap.size() <<" requirements"<<endl;
                AC_DEBUG << myShaderFeatureSet.getFeatureCount() << " shader features" << " of " << myRequirementMap.size() <<" requirements"<<endl;
                AC_DEBUG<< " Points: " << myScoreBoard[i].points << " points"<<", best = "<<myMaxPoints<<endl;
                if (allShaderFeatureUsed) {
                    AC_DEBUG << " and all its features could be used." << endl;
                } else {
                    AC_DEBUG << " but its feature support is unacceptable." << endl;
                }
            )


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

