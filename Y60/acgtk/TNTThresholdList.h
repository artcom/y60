#ifndef ACGTK_TNT_THRESHOLD_LIST_INCLUDED
#define ACGTK_TNT_THRESHOLD_LIST_INCLUDED

#include <dom/Nodes.h>

#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/image.h>

namespace acgtk {

class TNTThresholdList : public Gtk::TreeView {
    public:
        TNTThresholdList();
        virtual ~TNTThresholdList();

        /** Clears the list. */
        void clear();

        void refresh(dom::NodePtr thePaletteNode);

        enum ColumnNumbers {
            COL_INDEX,
            COL_NAME,
            COL_UPPER_THRESHOLD,
            COL_LOWER_THRESHOLD
        };
    private:
        class TNTThresholdModelColumns : public Gtk::TreeModel::ColumnRecord {
            public:
                TNTThresholdModelColumns() {
                    add(index);
                    add(name);
                    add(lowerThreshold);
                    add(upperThreshold);
                    add(color);
                }
                Gtk::TreeModelColumn<unsigned>       index;
                Gtk::TreeModelColumn<Glib::ustring>  name;
                Gtk::TreeModelColumn<int>            lowerThreshold;
                Gtk::TreeModelColumn<int>            upperThreshold;
                Gtk::TreeModelColumn<Gdk::Color>     color;
        };

        Glib::RefPtr<Gtk::ListStore> _myListModel;
        TNTThresholdModelColumns     _myColumns;

};

} // end of namespace

#endif // ACGTK_TNT_THRESHOLD_LIST_INCLUDED
