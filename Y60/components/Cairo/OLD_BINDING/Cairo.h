#ifndef _Cairo_h_
#define _Cairo_h_

#include <asl/PlugInBase.h>

#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/JSNode.h>
#include <y60/JSVector.h>

#include <iostream>
#include <vector>

#include <cairo.h>


class Cairo {
public:
    Cairo();
    ~Cairo();

    // context related functions
    void createContextFromImage(dom::NodePtr & theImageNode);
    void destroy();

    // drawing
    void moveTo(double theX, double theY);
    void lineTo(double theX, double theY);
    void relLineTo(double theX, double theY);

    void stroke();

    void setSourceRGBA(double theR, double theG, double theB);
    void setLineWidth(double theWidth);
    void fillPreserve();
    void rectangle(double theX, double theY, double theWidth, double theHeight);
    void arc(double theX, double theY, double theRadius, 
             double theAngle1, double theAngle2, bool theNegativeFlag=false) {
    void fill();
    void closePath();

    void save();
    void restore(); 

private:
    cairo_t         *_myCairoContext;
    cairo_surface_t *_myCairoSurface;
};

#endif // _Cairo_h_
