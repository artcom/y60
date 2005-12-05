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
//   $RCSfile: ACColumnRecord.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2004/11/25 11:45:08 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_ACCOLUMNRECORD_INCLUDED_
#define _Y60_ACGTKSHELL_ACCOLUMNRECORD_INCLUDED_

#include <gtkmm/liststore.h>

class ACColumnRecord : public Gtk::TreeModel::ColumnRecord {
    public:
        ACColumnRecord(unsigned int theColCount);
        Gtk::TreeModelColumn<unsigned int> _myRecId;
        std::vector<Gtk::TreeModelColumn<Glib::ustring> > _myColumns;
    private:
        ACColumnRecord();
        ACColumnRecord(const ACColumnRecord &);
        ACColumnRecord & operator=(const ACColumnRecord &);
};

#endif

