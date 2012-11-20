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
*/

#ifndef Y60_CONNECTED_COMPONENTS_INCLUDED
#define Y60_CONNECTED_COMPONENTS_INCLUDED

#include "y60_image_settings.h"

#include <asl/math/Vector234.h>
#include <asl/math/Box.h>
#include <asl/base/Ptr.h>

#include <asl/dom/Value.h>

#include <vector>
#include <list>
#include <map>

namespace y60 {


class Y60_IMAGE_DECL Run {
    public:
        Run(int theRow, int theStartCol, int theEndCol);
        asl::Vector2f center();
        int length();

        int _myRow;
        int _myStartCol;
        int _myEndCol;
        int _myLabel;
    private:
        static int _ourLastLabel;
};

typedef asl::Ptr<Run> RunPtr;

typedef std::list<RunPtr> RunList;
typedef std::list<asl::Vector2f> PointList;


class Blob;
typedef asl::Ptr<Blob> BlobPtr;

class Y60_IMAGE_DECL Blob {
    public:
        Blob() {}
        Blob(RunPtr theRun);
        ~Blob();
        RunList & getRuns();
        asl::Vector2f center();
        int area();
        asl::Box2i bbox() const;
        void merge( BlobPtr otherBlob);
        RunList & getList();

        BlobPtr _myParent;
    private:
        RunList _myRuns;
};


typedef std::vector<BlobPtr> BlobList;
typedef asl::Ptr<BlobList> BlobListPtr;
typedef std::map<int, BlobPtr> ComponentMap;

Y60_IMAGE_DECL BlobListPtr connectedComponents(dom::ResizeableRasterPtr image, int object_threshold);

} // end namespace

#endif // Y60_CONNECTED_COMPONENTS_INCLUDED
