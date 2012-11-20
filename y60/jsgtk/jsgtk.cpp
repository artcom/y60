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

// own header
#include "jsgtk.h"


// custom ART+COM widgets
#include "JSDragButton.h"
#include "JSEmbeddedButton.h"
#include "JSEmbeddedToggle.h"
#include "JSCWRuler.h"
#include "JSGradientSlider.h"
#include "JSHistogram.h"
#include "JSGrayScale.h"
#include "JSTNTMeasurementList.h"
#include "JSTNTThresholdList.h"
#include "JSACIconFactory.h"
#include "JSConsoleView.h"

// standard Gtk widgets
#include "JSFrame.h"
#include "JSRenderArea.h"
#include "JSDrawingArea.h"
#include "JSEventBox.h"
#include "JSVBox.h"
#include "JSHBox.h"
#include "JSButtonBox.h"
#include "JSVButtonBox.h"
#include "JSHButtonBox.h"
#include "JSContainer.h"
#include "JSWindow.h"
#include "JSScrolledWindow.h"
#include "JSMenuItem.h"
#include "JSTearoffMenuItem.h"
#include "JSSeparatorMenuItem.h"
#include "JSCheckMenuItem.h"
#include "JSRadioMenuItem.h"
#include "JSEntry.h"
#include "JSSpinButton.h"
#include "JSRadioButton.h"
#include "JSCheckButton.h"
#include "JSToggleButton.h"
#include "JSColorButton.h"
#include "JSRadioToolButton.h"
#include "JSToggleToolButton.h"
#include "JSToolButton.h"
#include "JSSeparatorToolItem.h"
#include "JSToolItem.h"
#include "JSButton.h"
#include "JSACColumnRecord.h"
#include "JSTreeView.h"
#include "JSListStore.h"
#include "JSHPaned.h"
#include "JSVPaned.h"
#include "JSToolbar.h"
#include "JSDialog.h"
#include "JSMessageDialog.h"
#include "JSFileChooserDialog.h"
#include "JSColorSelectionDialog.h"
#include "JSLabel.h"
#include "JSArrow.h"
#include "JSImage.h"
#include "JSStatusBar.h"
#include "JSProgressBar.h"
#include "JSVRuler.h"
#include "JSHRuler.h"
#include "JSFixed.h"
#include "JSTable.h"
#include "JSComboBox.h"
#include "JSComboBoxText.h"
#include "JSVScale.h"
#include "JSHScale.h"
#include "JSScale.h"
#include "JSRange.h"
#include "JSMenu.h"
#include "JSNotebook.h"
#include "JSColorSelection.h"
#include "JSTextView.h"

#include "JSCellRenderer.h"
#include "JSCellRendererText.h"

#include "JSGtkMain.h"
#include "JSGlade.h"
#include "JSCellRenderer.h"
#include "JSTreeIter.h"
#include "JSStockID.h"
#include "JSGtkEnums.h"
#include "JSSigConnection.h"
#include "JSSignalProxies.h"
#include "JSSignals.impl"
#include "JSGdkEvent.h"
#include "JSGdkCursor.h"
#include "JSSeparator.h"
#include "JSVSeparator.h"
#include "JSHSeparator.h"

#include "JSTooltips.h"
// just for unit tests
#include "JSTestSubject.h"

#include <y60/jsbase/JSWrapper.impl>
#include <iostream>
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>

using namespace jslib;
using namespace std;
using namespace asl;

namespace jslib {
    INSTANTIATE_SIGNAL0_WRAPPER(void);
    INSTANTIATE_SIGNAL1_WRAPPER(void, float);
    INSTANTIATE_SIGNAL1_WRAPPER(void, double);
    INSTANTIATE_SIGNAL1_WRAPPER(bool, double);
    INSTANTIATE_SIGNAL1_WRAPPER(void, const std::string &);
    INSTANTIATE_SIGNAL2_WRAPPER(void, double, double);
    INSTANTIATE_SIGNAL3_WRAPPER(void, double, double, unsigned int);
}

// add all non-abstract JS Wrapper classes here
bool initGtkClasses(JSContext *cx, JSObject *theGlobalObject) {

    // === Custom ART+COM Widgets =======================================
    if (!JSConsoleView::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSACIconFactory::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGrayScale::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHistogram::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSCWRuler::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGradientSlider::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEmbeddedButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEmbeddedToggle::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSDragButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTNTMeasurementList::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTNTThresholdList::initClass(cx, theGlobalObject)) {
        return false;
    }
    // === Standard Gtk Widgets =======================================
    if (!JSGtkMain::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSWindow::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSScrolledWindow::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRenderArea::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSDrawingArea::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHBox::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVBox::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHButtonBox::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVButtonBox::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSCheckMenuItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRadioMenuItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSeparatorMenuItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTearoffMenuItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSMenuItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEntry::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSpinButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRadioButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSColorButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSCheckButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSToggleButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSeparatorToolItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSToolItem::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSToolButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSToggleToolButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRadioToolButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSButton::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSACColumnRecord::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGlade::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHPaned::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVPaned::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSToolbar::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSDialog::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSMessageDialog::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSFileChooserDialog::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSColorSelectionDialog::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSStockID::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSImage::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSLabel::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSArrow::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSStatusBar::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSProgressBar::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHRuler::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVRuler::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSFixed::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTable::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGtk::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSComboBox::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSComboBoxText::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVScale::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHScale::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSMenu::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSNotebook::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGdkEvent::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEventBox::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSFrame::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVSeparator::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSHSeparator::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTextView::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSColorSelection::initClass(cx, theGlobalObject)) {
        return false;
    }

    // === GLib::SignalProxy Stuff =======================================
    // don't forget to add a class trait in JSSignalProxies.h
    INIT_SIGNALPROXY0(void);
    INIT_SIGNALPROXY1(void, int);
    INIT_SIGNALPROXY1(void, double);
    INIT_SIGNALPROXY1(bool, double);
    INIT_SIGNALPROXY1(Glib::ustring, double);
    INIT_SIGNALPROXY1(bool, GdkEventButton*);
    INIT_SIGNALPROXY1(bool, GdkEventMotion*);
    INIT_SIGNALPROXY1(bool, GdkEventKey*);
    INIT_SIGNALPROXY1(bool, GdkEventCrossing*);
    INIT_SIGNALPROXY1(bool, GdkEventFocus*);
    INIT_SIGNALPROXY1(bool, GdkEventAny*);
    INIT_SIGNALPROXY1(std::string, const std::string &);
    INIT_SIGNALPROXY2(void, const Glib::ustring &, const Glib::ustring &);
    INIT_SIGNALPROXY2(void, guint, const Glib::ustring &);
    INIT_SIGNALPROXY2(void, GtkNotebookPage*, guint);

    // === sigc::signal Stuff =======================================
    // don't forget to add a class trait in JSSignals.h
    INIT_SIGNAL0(void);
    INIT_SIGNAL0(int);
    INIT_SIGNAL1(void, int);
    INIT_SIGNAL1(void, float);
    INIT_SIGNAL1(void, double);
    INIT_SIGNAL1(bool, double);
    INIT_SIGNAL1(void, Glib::ustring);
    INIT_SIGNAL1(void, const std::string &);
    INIT_SIGNAL2(void, double, double);
    INIT_SIGNAL2(bool, double, Glib::ustring);
    INIT_SIGNAL2(void, Glib::ustring, Glib::ustring);
    INIT_SIGNAL3(void, double, double, unsigned int);

    if (!JSSigConnection::initClass(cx, theGlobalObject)) {
        return false;
    }

    // === Gtk::TreeView Stuff =======================================
    if (!JSTreeModel::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSListStore::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTreeView::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTreeIter::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSCellRenderer::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSCellRendererText::initClass(cx, theGlobalObject)) {
        return false;
    }

    // === other Gtk/Gdk related Stuff ==============================
    if ( ! JSGdkCursor::initClass(cx, theGlobalObject)) {
        return false;
    }
    if ( ! JSTooltips::initClass(cx, theGlobalObject)) {
        return false;
    }
    // === only for unit tests ==============================
    if ( ! JSTestSubject::initClass(cx, theGlobalObject)) {
        return false;
    }

    return true;
}

namespace jslib {

template<class TARGET>
bool try_to_cast(JSContext * cx, Gtk::Widget * theWidget, jsval & theTarget, bool takeOwnership) {
    TARGET * myNative = dynamic_cast<TARGET*>(theWidget);
    if (myNative) {
        asl::Ptr<TARGET> myOwner;
        if (takeOwnership) {
            cerr << "taking ownership of " << myNative->get_name() << endl;
            myOwner = asl::Ptr<TARGET>(myNative);
        }
        theTarget = as_jsval(cx, myOwner, myNative);
        return true;
    }
    return false;
}

#define TRY_DYNAMIC_CAST(TARGET) if (try_to_cast<TARGET>(cx, theWidget, myJsVal, takeOwnership)) return myJsVal;

// add all non abstract classes derived from Gtk::Widget here
// put derived classes before their bases
jsval gtk_jsval(JSContext *cx, Gtk::Widget * theWidget, bool takeOwnership) {
    jsval myJsVal = JSVAL_NULL;

    TRY_DYNAMIC_CAST(acgtk::ConsoleView);
    TRY_DYNAMIC_CAST(acgtk::GrayScale);
    TRY_DYNAMIC_CAST(acgtk::Histogram);
    TRY_DYNAMIC_CAST(acgtk::CWRuler);
    TRY_DYNAMIC_CAST(acgtk::GradientSlider);
    TRY_DYNAMIC_CAST(acgtk::RenderArea);
    TRY_DYNAMIC_CAST(acgtk::DragButton);
    TRY_DYNAMIC_CAST(acgtk::EmbeddedToggle);
    TRY_DYNAMIC_CAST(acgtk::EmbeddedButton);
    TRY_DYNAMIC_CAST(acgtk::TNTMeasurementList);
    TRY_DYNAMIC_CAST(acgtk::TNTThresholdList);

    TRY_DYNAMIC_CAST(Gtk::ColorSelection);
    TRY_DYNAMIC_CAST(Gtk::Frame);
    TRY_DYNAMIC_CAST(Gtk::DrawingArea);
    TRY_DYNAMIC_CAST(Gtk::ColorSelectionDialog);
    TRY_DYNAMIC_CAST(Gtk::FileChooserDialog);
    TRY_DYNAMIC_CAST(Gtk::MessageDialog);
    TRY_DYNAMIC_CAST(Gtk::Dialog );
    TRY_DYNAMIC_CAST(Gtk::Window );
    TRY_DYNAMIC_CAST(Gtk::ScrolledWindow );
    TRY_DYNAMIC_CAST(Gtk::Statusbar );
    TRY_DYNAMIC_CAST(Gtk::HBox );
    TRY_DYNAMIC_CAST(Gtk::VBox );
    TRY_DYNAMIC_CAST(Gtk::HButtonBox );
    TRY_DYNAMIC_CAST(Gtk::VButtonBox );
    TRY_DYNAMIC_CAST(Gtk::TearoffMenuItem );
    TRY_DYNAMIC_CAST(Gtk::SeparatorMenuItem );
    TRY_DYNAMIC_CAST(Gtk::RadioMenuItem );
    TRY_DYNAMIC_CAST(Gtk::CheckMenuItem );
    TRY_DYNAMIC_CAST(Gtk::MenuItem );
    TRY_DYNAMIC_CAST(Gtk::EventBox );
    TRY_DYNAMIC_CAST(Gtk::SpinButton );
    TRY_DYNAMIC_CAST(Gtk::RadioButton );
    TRY_DYNAMIC_CAST(Gtk::CheckButton );
    TRY_DYNAMIC_CAST(Gtk::ToggleButton );
    TRY_DYNAMIC_CAST(Gtk::ColorButton );
    TRY_DYNAMIC_CAST(Gtk::Button );
    TRY_DYNAMIC_CAST(Gtk::RadioToolButton );
    TRY_DYNAMIC_CAST(Gtk::ToggleToolButton );
    TRY_DYNAMIC_CAST(Gtk::ToolButton );
    TRY_DYNAMIC_CAST(Gtk::SeparatorToolItem );
    TRY_DYNAMIC_CAST(Gtk::ToolItem );
    TRY_DYNAMIC_CAST(Gtk::TreeView );
    TRY_DYNAMIC_CAST(Gtk::Entry );
    TRY_DYNAMIC_CAST(Gtk::HPaned );
    TRY_DYNAMIC_CAST(Gtk::VPaned );
    TRY_DYNAMIC_CAST(Gtk::TextView );
    TRY_DYNAMIC_CAST(Gtk::Toolbar );
    TRY_DYNAMIC_CAST(Gtk::Label );
    TRY_DYNAMIC_CAST(Gtk::Arrow );
    TRY_DYNAMIC_CAST(Gtk::Image );
    TRY_DYNAMIC_CAST(Gtk::ProgressBar );
    TRY_DYNAMIC_CAST(Gtk::HRuler);
    TRY_DYNAMIC_CAST(Gtk::VRuler);
    TRY_DYNAMIC_CAST(Gtk::Fixed);
    TRY_DYNAMIC_CAST(Gtk::Table);
    TRY_DYNAMIC_CAST(Gtk::ComboBox);
    TRY_DYNAMIC_CAST(Gtk::VScale);
    TRY_DYNAMIC_CAST(Gtk::HScale);
    TRY_DYNAMIC_CAST(Gtk::Menu);
    TRY_DYNAMIC_CAST(Gtk::Notebook);
    TRY_DYNAMIC_CAST(Gtk::VSeparator);
    TRY_DYNAMIC_CAST(Gtk::HSeparator);
    return myJsVal;
}

template<class NATIVE, class TARGET>
bool castFromRefPtr(JSContext * cx, JSObject * obj, Glib::RefPtr<TARGET> & theTarget) {
    if (JSA_GetClass(cx,obj) == JSClassTraits<NATIVE>::Class()) {
        Glib::RefPtr<NATIVE> myRefPtr = JSClassTraits<NATIVE>::getNativeOwner(cx, obj);

        Glib::RefPtr<TARGET> myTarget = Glib::RefPtr<TARGET>::cast_dynamic(myRefPtr);
        if (myTarget) {
            theTarget = myTarget;
            return true;
        }
    }
    return false;
}

template<class NATIVE, class TARGET>
bool castFrom(JSContext * cx, JSObject * obj, TARGET *& theTarget) {
    if (JSA_GetClass(cx,obj) == JSClassTraits<NATIVE>::Class()) {
        typedef typename JSClassTraits<NATIVE>::ScopedNativeRef NativeRef;
        NativeRef myObj(cx, obj);
        NATIVE * myNative = &myObj.getNative();
        TARGET * myTarget = dynamic_cast<TARGET*>(myNative);
        if (myTarget) {
            theTarget = myTarget;
            return true;
        }
    }
    return false;
}

template <class TARGET>
bool
ConvertFrom<TARGET>::convert(JSContext *cx, jsval theValue, Glib::RefPtr<TARGET> & theTarget)
{
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (castFromRefPtr<Gtk::ListStore, TARGET>(cx, myArgument, theTarget)) {
                return true;
            }
        }
    }
    return false;
}

template <class TARGET>
bool
ConvertFrom<TARGET>::convert(JSContext *cx, jsval theValue, TARGET *& theTarget) {
    if (JSVAL_IS_OBJECT(theValue)) {
        if (theValue == JSVAL_NULL) {
            theTarget = 0;
            return true;
        }
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (castFrom<acgtk::RenderArea>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::ConsoleView>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::GrayScale>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::Histogram>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::CWRuler>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::GradientSlider>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::EmbeddedToggle>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::DragButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::EmbeddedButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::TNTThresholdList>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<acgtk::TNTMeasurementList>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ColorSelection>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Frame>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::DrawingArea>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Window>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ScrolledWindow>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Statusbar>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::EventBox>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::HBox>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::VBox>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::HButtonBox>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::VButtonBox>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::TearoffMenuItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::SeparatorMenuItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::RadioMenuItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::CheckMenuItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::MenuItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::SpinButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::RadioButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::CheckButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ToggleButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ColorButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Button>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::RadioToolButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ToggleToolButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ToolButton>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::SeparatorToolItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ToolItem>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Entry>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::TreeView>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<ACColumnRecord>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::HPaned>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::VPaned>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::TextView>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Toolbar>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Image>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Label>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Arrow>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Dialog>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::MessageDialog>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::FileChooserDialog>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ColorSelectionDialog>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ProgressBar>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::HRuler>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::VRuler>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Fixed>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Table>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ComboBox>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::ComboBoxText>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Scale>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::VScale>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::HScale>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Menu>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::HSeparator>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::VSeparator>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::Notebook>(cx, myArgument, theTarget)) {
                return true;
            // Gtk::CellRenderer's
            } else if (castFrom<Gtk::CellRendererText>(cx, myArgument, theTarget)) {
                return true;
            } else if (castFrom<Gtk::CellRenderer>(cx, myArgument, theTarget)) {
                return true;
            }

        }
    }
    return false;
}
#define HIDE_GLIB_REF_PTR_CONVERSION( CLASS ) \
template <> \
bool \
ConvertFrom<CLASS>::convert(JSContext *cx, jsval theValue, \
                            Glib::RefPtr<CLASS> & theTarget) \
{ \
    throw asl::Exception( #CLASS " is not RefPtr aware.", PLUS_FILE_LINE); \
}

#define CONVERT_FROM_GLIB_OBJECT( CLASS  ) \
    template class ConvertFrom<CLASS>;

#define CONVERT_FROM( CLASS  ) \
    HIDE_GLIB_REF_PTR_CONVERSION( CLASS ) \
    template class ConvertFrom<CLASS>;

CONVERT_FROM(ACColumnRecord);
CONVERT_FROM(Gtk::TreeModelColumnRecord);
//CONVERT_FROM(AbstractRenderWindow);

CONVERT_FROM_GLIB_OBJECT(acgtk::ConsoleView);
CONVERT_FROM_GLIB_OBJECT(acgtk::GrayScale);
CONVERT_FROM_GLIB_OBJECT(acgtk::Histogram);
CONVERT_FROM_GLIB_OBJECT(acgtk::CWRuler);
CONVERT_FROM_GLIB_OBJECT(acgtk::GradientSlider);
CONVERT_FROM_GLIB_OBJECT(acgtk::DragButton);
CONVERT_FROM_GLIB_OBJECT(acgtk::EmbeddedToggle);
CONVERT_FROM_GLIB_OBJECT(acgtk::EmbeddedButton);
CONVERT_FROM_GLIB_OBJECT(acgtk::RenderArea);
CONVERT_FROM_GLIB_OBJECT(acgtk::TNTMeasurementList);
CONVERT_FROM_GLIB_OBJECT(acgtk::TNTThresholdList);

CONVERT_FROM_GLIB_OBJECT(Gtk::ColorSelection);
CONVERT_FROM_GLIB_OBJECT(Gtk::Frame);
CONVERT_FROM_GLIB_OBJECT(Gtk::DrawingArea);
CONVERT_FROM_GLIB_OBJECT(Gtk::Widget);
CONVERT_FROM_GLIB_OBJECT(Gtk::Separator);
CONVERT_FROM_GLIB_OBJECT(Gtk::HSeparator);
CONVERT_FROM_GLIB_OBJECT(Gtk::VSeparator);
CONVERT_FROM_GLIB_OBJECT(Gtk::Container);
CONVERT_FROM_GLIB_OBJECT(Gtk::Notebook);
CONVERT_FROM_GLIB_OBJECT(Gtk::Bin);
CONVERT_FROM_GLIB_OBJECT(Gtk::Window);
CONVERT_FROM_GLIB_OBJECT(Gtk::ScrolledWindow);
CONVERT_FROM_GLIB_OBJECT(Gtk::Statusbar);
CONVERT_FROM_GLIB_OBJECT(Gtk::Box);
CONVERT_FROM_GLIB_OBJECT(Gtk::HBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::VBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::ButtonBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::HButtonBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::VButtonBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::TearoffMenuItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::SeparatorMenuItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::CheckMenuItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::RadioMenuItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::MenuItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::SpinButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::RadioButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::CheckButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::ToggleButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::ColorButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::Button);
CONVERT_FROM_GLIB_OBJECT(Gtk::RadioToolButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::ToggleToolButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::ToolButton);
CONVERT_FROM_GLIB_OBJECT(Gtk::SeparatorToolItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::ToolItem);
CONVERT_FROM_GLIB_OBJECT(Gtk::Entry);
CONVERT_FROM_GLIB_OBJECT(Gtk::TreeView);
CONVERT_FROM_GLIB_OBJECT(Gtk::HPaned);
CONVERT_FROM_GLIB_OBJECT(Gtk::VPaned);
CONVERT_FROM_GLIB_OBJECT(Gtk::Paned);
CONVERT_FROM_GLIB_OBJECT(Gtk::Toolbar);
CONVERT_FROM_GLIB_OBJECT(Gtk::Image);
CONVERT_FROM_GLIB_OBJECT(Gtk::Label);
CONVERT_FROM_GLIB_OBJECT(Gtk::Arrow);
CONVERT_FROM_GLIB_OBJECT(Gtk::Dialog);
CONVERT_FROM_GLIB_OBJECT(Gtk::MessageDialog);
CONVERT_FROM_GLIB_OBJECT(Gtk::FileChooserDialog);
CONVERT_FROM_GLIB_OBJECT(Gtk::ColorSelectionDialog);
CONVERT_FROM_GLIB_OBJECT(Gtk::Ruler);
CONVERT_FROM_GLIB_OBJECT(Gtk::HRuler);
CONVERT_FROM_GLIB_OBJECT(Gtk::VRuler);
CONVERT_FROM_GLIB_OBJECT(Gtk::Fixed);
CONVERT_FROM_GLIB_OBJECT(Gtk::Table);
CONVERT_FROM_GLIB_OBJECT(Gtk::ComboBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::ComboBoxText);
CONVERT_FROM_GLIB_OBJECT(Gtk::Range);
CONVERT_FROM_GLIB_OBJECT(Gtk::Scale);
CONVERT_FROM_GLIB_OBJECT(Gtk::VScale);
CONVERT_FROM_GLIB_OBJECT(Gtk::HScale);
CONVERT_FROM_GLIB_OBJECT(Gtk::ProgressBar);
CONVERT_FROM_GLIB_OBJECT(Gtk::Menu);
CONVERT_FROM_GLIB_OBJECT(Gtk::EventBox);
CONVERT_FROM_GLIB_OBJECT(Gtk::TreeModel);
CONVERT_FROM_GLIB_OBJECT(Gtk::CellRenderer);
CONVERT_FROM_GLIB_OBJECT(Gtk::CellRendererText);

}

