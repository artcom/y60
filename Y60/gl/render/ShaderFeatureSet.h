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
//
//   $RCSfile: ShaderFeatureSet.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef AC_Y60_SHADER_FEATURE_SET_INCLUDED
#define AC_Y60_SHADER_FEATURE_SET_INCLUDED

#include <asl/dom/Nodes.h>
#include <y60/base/DataTypes.h>
#include <asl/base/Exception.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace y60 {

typedef std::map<std::string, VectorOfString> FeatureMap;

class ShaderFeatureSet {
    public:
        ShaderFeatureSet();
        virtual ~ShaderFeatureSet();
        const VectorOfString * getFeatures(const std::string & theFeatureClass) const;
        const VectorOfTextureUsage & getTextureFeature() const;
        bool hasFeature(const std::string & theFeatureClass) const;
        void load(const dom::NodePtr theNode);
        unsigned getFeatureCount() const;
        
    private:
        FeatureMap _myFeatureMap;
        VectorOfTextureUsage _myTextureFeature;
};

} // namespace y60



#endif // AC_Y60_SHADER_FEATURE_SET_INCLUDED
