//============================================================================
//
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

// Cairo Drawing API Wrapper
// the following functions should be implemented
//
// cairo_t: 
// 

#include "Cairo.h"

#include <y60/JSVector.h>
#include <asl/string_functions.h>
#include <dom/Nodes.h>

#include <string.h>

#define DB(x)  x

#define SVG_FROM_STRING 0
#define SVG_FROM_FILE   1

using namespace std;
using namespace jslib;
using namespace y60;
using namespace asl;

Cairo::Cairo()
: _myCairoContext(NULL), _myCairoSurface(NULL)
{
}

Cairo::~Cairo() {
    if (_myCairoSurface || _myCairoContext) {
       destroy();
    }
}

void
Cairo::createContextFromImage(dom::NodePtr & theImageNode) {
    cairo_format_t  myRFormat = CAIRO_FORMAT_ARGB32;
    
    ImagePtr myImage       = theImageNode->getFacade<Image>();
    unsigned myImageWidth  = myImage->get<ImageWidthTag>();
    unsigned myImageHeight = myImage->get<ImageHeightTag>();
    
    unsigned int mySvgWidth, mySvgHeight;
    unsigned myStride = myImageWidth*4;
    
    unsigned char * myDataPtr = myImage->getRasterPtr()->pixels().begin();
    myCairoSurface = cairo_image_surface_create_for_data(myDataPtr, myRFormat, 
                                                         myImageWidth, myImageHeight, myStride);
    
    myCairoContext = cairo_create(myCairoSurface);
}

void
Cairo::save() {
    cairo_save(_myCairoContext);
}

void
Cairo::restore() {
    cairo_restore(_myCairoContext);
}

void 
Cairo::destroy() {
    cairo_surface_destroy(_myCairoSurface); 
    cairo_destroy(_myCairoContext);
}

void 
Cairo::moveTo(double theX, double theY) {
    cairo_move_to(_myCairoContext, theX, theY);
}

void 
Cairo::lineTo(double theX, double theY) {
    cairo_line_to(_myCairoContext, theX, theY);
}

void 
Cairo::relLineTo(double theX, double theY) {
    cairo_rel_line_to(_myCairoContext, theX, theY);
}

void 
Cairo::stroke() {
    cairo_stroke(_myCairoContext);
}

void 
Cairo::setSourceRGBA(double theR, double theG, double theB, double theA) {
    cairo_set_source_rgba(_myCairoContext, theR, theG, theB, theA);
}

void 
Cairo::setLineWidth(double theWidth) {
    cairo_set_line_width(_myCairoContext, theWidth);
}

void 
Cairo::fillPreserve() {
    cairo_fill_preserve(_myCairoContext); 
}

void 
Cairo::rectangle(double theX, double theY, double theWidth, double theHeight) {
    cairo_rectangle(_myCairoContext, theX, theY, theWidth, theHeight);
}

void 
Cairo::arc(double theX, double theY, double theRadius, double theAngle1, double theAngle2, bool theNegativeFlag) {
    if (theNegativeFlag) {
        cairo_arc_negative(_myCairoContext, theX, theY, theRadius, theAngle1, theAngle2);
    } else {
        cairo_arc(_myCairoContext, theX, theY, theRadius, theAngle1, theAngle2);
    }
}

void 
Cairo::fill() {
    cairo_fill(_myCairoContext); 
}

void Cairo::closePath() {
    cairo_close_path(_myCairoContext); 
}
