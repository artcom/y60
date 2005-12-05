//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ProgressBar.h,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/04/18 16:46:33 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_ProgressBar_h_
#define _ac_maya_ProgressBar_h_

#include <string>

class ProgressBar {
    public:
        ProgressBar(bool theEnabled);
        ~ProgressBar();

        void advance(unsigned theStep = 1);
        void setSize(unsigned theSize);
        void setStatus(const std::string & theStatus);

    private:
        bool _myEnabled;
};

#endif
