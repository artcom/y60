//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: XmlHelper.h,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================
#ifndef INCL_XMLHELPER
#define INCL_XMLHELPER

#include <string>

#include <asl/dom/Nodes.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel32.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace TexGen {

    int getIntXMLParam (const dom::Node & theNode, 
                        const std::string & theField) ;
    float getFloatXMLParam (const dom::Node & theNode, 
                            const std::string & theField) ;
    int getDefaultedIntXMLParam (const dom::Node & theNode, 
                                 const std::string & theField,
                                 int theDefault) ;
    const std::string & getRequiredXMLField(const dom::Node & theNode, 
                                            const std::string & theFieldName) ;
    PLPixel32 getXMLColorNode (const dom::Node & theNode);
    
}
#endif
