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

#ifndef ACGTK_TNT_MEASUREMENT_LIST_INCLUDED
#define ACGTK_TNT_MEASUREMENT_LIST_INCLUDED

#include "y60_acgtk_settings.h"

#include <asl/dom/Nodes.h>

#ifdef OSX
#undef check
#undef nil
#endif

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/image.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4250 4275)
#endif //defined(_MSC_VER)

namespace acgtk {

class Y60_ACGTK_DECL TNTMeasurementList : public Gtk::TreeView {
    public:
        /** The default constructor creates a default list view without icons
         *  and special layout. It must be present because all wrapped objects
         *  have to be default constructable
         */
        TNTMeasurementList();

        /** This constructor creates the nice looking list view with icons and stuff
         */
        TNTMeasurementList(const std::string & theVisibleIcon,
                           const std::string & theHiddenIcon,
                           const std::string & theLockedIcon,
                           const std::string & theEditableIcon);
        virtual ~TNTMeasurementList();

        /** Associates a given type name with an icon. The icon is read from disc.
         */
        void registerTypeIcon(const std::string & theTypeName,
                              const std::string & theIconFile);

        /** Appends a new row to the list, fills in the values from theMeasurement
         *  and returns an iterator to the new row.
         */
        Gtk::TreeIter append(dom::NodePtr theMeasurementNode, const Glib::ustring & theDisplayValue);

        /** Removes the row correspondig to @p theMeasurementNode
         */
        Gtk::TreeIter
        remove(dom::NodePtr theMeasurementNode);

        bool isEndIter(const Gtk::TreeIter & theIter);

        /** Updates the row witht the id of @p theMeasurementNode with the data given by
         *  @p theMeasurementNode  and @p theDisplayValue.
         */
        Gtk::TreeIter update(dom::NodePtr theMeasurementNode, const Glib::ustring & theDisplayValue);

        /** Clears the list. */
        void clear();

        sigc::signal<void, Glib::ustring> signal_visible_toggled() const { return _myVisibilityToggledSignal; }
        sigc::signal<void, Glib::ustring> signal_editable_toggled() const { return _myLockedToggledSignal; }
        sigc::signal<void> signal_right_click() const { return _myRightClickSignal; }
        sigc::signal<void, Glib::ustring, Glib::ustring> signal_name_changed() const { return _myNameChangedSignal; }

        enum ColumnNumbers {
            COL_IS_VISIBLE,
            COL_TYPE_ICON,
            COL_NAME,
            COL_VALUE,
            COL_IS_EDITABLE,
            COL_XML_ID,
            COL_TYPE
        };

    private:
        typedef Gtk::TreeModel::Children Children;
        Children::iterator
        findRowById(const Glib::ustring & theId);

        class TNTMeasurementModelColumns : public Gtk::TreeModel::ColumnRecord {
            public:
                TNTMeasurementModelColumns() {
                    add(is_visible);
                    add(type_icon);
                    add(name);
                    add(value);
                    add(is_editable);
                    add(xml_id);
                    add(type);
                }
                Gtk::TreeModelColumn<bool>                        is_visible;
                Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> >  type_icon;
                Gtk::TreeModelColumn<Glib::ustring>               name;
                Gtk::TreeModelColumn<Glib::ustring>               value; // is a string because it contains the unit name (like mm)
                Gtk::TreeModelColumn<bool>                        is_editable;
                Gtk::TreeModelColumn<Glib::ustring>               xml_id;
                Gtk::TreeModelColumn<Glib::ustring>               type; // used to sort by type
        };
        typedef std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> > IconMap;
        void onVisibilityToggled(const Glib::ustring & thePathString);
        void onEditableToggled(const Glib::ustring & thePathString);
        void onNameEdited(const Glib::ustring & thePath, const Glib::ustring & theValue);
        void createDummyRows();
        virtual bool on_button_press_event(GdkEventButton *ev); // override
        void onSortByName();
        void onSortByType();

        Glib::RefPtr<Gtk::ListStore> _myListModel;
        TNTMeasurementModelColumns   _myColumns;
        Gtk::Image                   _myVisibilityHeaderIcon;
        Gtk::Image                   _myEditableHeaderIcon;

        Gtk::TreeViewColumn * _myNameColumn;
        Gtk::TreeViewColumn * _myTypeColumn;


        IconMap                      _myTypeIcons;

        sigc::signal<void, Glib::ustring> _myVisibilityToggledSignal;
        sigc::signal<void, Glib::ustring> _myLockedToggledSignal;
        sigc::signal<void> _myRightClickSignal;
        sigc::signal<void, Glib::ustring, Glib::ustring> _myNameChangedSignal;
        /*
        sigc::signal<bool, GdkEventKey*> _myKeyPressSignal;
        sigc::signal<bool, GdkEventKey*> _myKeyReleaseSignal;
        */
};

} // end of namespace

#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#endif // ACGTK_TNT_MEASUREMENT_LIST_INCLUDED
