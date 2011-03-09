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

#ifndef _Y60_ACGTKSHELL_JSSTOCKID_INCLUDED_
#define _Y60_ACGTKSHELL_JSSTOCKID_INCLUDED_

#include "y60_jsgtk_settings.h"

#include "JSWindow.h"
#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
    // supress warnings caused by gtk in vc++
    #pragma warning(disable:4250)
#endif //defined(_MSC_VER)
#include <gtkmm/dialog.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {


struct JSStockIdPropertySpec;

class JSStockID : public JSWrapper<Gtk::StockID, asl::Ptr<Gtk::StockID>, StaticAccessProtocol> {
    private:
        JSStockID();  // hide default constructor
//        typedef bool JSBASE;
    public:
        typedef Gtk::StockID NATIVE;
        typedef asl::Ptr<Gtk::StockID> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "StockID";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_STOCK_DIALOG_AUTHENTICATION = -100,

            PROP_STOCK_DIALOG_INFO,
            PROP_STOCK_DIALOG_WARNING,
            PROP_STOCK_DIALOG_ERROR,
            PROP_STOCK_DIALOG_QUESTION,

            PROP_STOCK_DND,
            PROP_STOCK_DND_MULTIPLE,

            PROP_STOCK_ADD,
            PROP_STOCK_APPLY,
            PROP_STOCK_BOLD,
            PROP_STOCK_CANCEL,
            PROP_STOCK_CDROM,
            PROP_STOCK_CLEAR,
            PROP_STOCK_CLOSE,
            PROP_STOCK_CONVERT,
            PROP_STOCK_COPY,
            PROP_STOCK_CUT,
            PROP_STOCK_DELETE,
            PROP_STOCK_EXECUTE,
            PROP_STOCK_FIND,
            PROP_STOCK_FIND_AND_REPLACE,
            PROP_STOCK_FLOPPY,
            PROP_STOCK_GOTO_BOTTOM,
            PROP_STOCK_GOTO_FIRST,
            PROP_STOCK_GOTO_LAST,
            PROP_STOCK_GOTO_TOP,
            PROP_STOCK_GO_BACK,
            PROP_STOCK_GO_DOWN,
            PROP_STOCK_GO_FORWARD,
            PROP_STOCK_GO_UP,
            PROP_STOCK_HELP,
            PROP_STOCK_HOME,
            PROP_STOCK_INDEX,
            PROP_STOCK_ITALIC,
            PROP_STOCK_JUMP_TO,
            PROP_STOCK_JUSTIFY_CENTER,
            PROP_STOCK_JUSTIFY_FILL,
            PROP_STOCK_JUSTIFY_LEFT,
            PROP_STOCK_JUSTIFY_RIGHT,
            PROP_STOCK_MISSING_IMAGE,
            PROP_STOCK_NEW,
            PROP_STOCK_NO,
            PROP_STOCK_OK,
            PROP_STOCK_OPEN,
            PROP_STOCK_PASTE,
            PROP_STOCK_PREFERENCES,
            PROP_STOCK_PRINT,
            PROP_STOCK_PRINT_PREVIEW,
            PROP_STOCK_PROPERTIES,
            PROP_STOCK_QUIT,
            PROP_STOCK_REDO,
            PROP_STOCK_REFRESH,
            PROP_STOCK_REMOVE,
            PROP_STOCK_REVERT_TO_SAVED,
            PROP_STOCK_SAVE,
            PROP_STOCK_SAVE_AS,
            PROP_STOCK_SELECT_COLOR,
            PROP_STOCK_SELECT_FONT,
            PROP_STOCK_SORT_ASCENDING,
            PROP_STOCK_SORT_DESCENDING,
            PROP_STOCK_SPELL_CHECK,
            PROP_STOCK_STOP,
            PROP_STOCK_STRIKETHROUGH,
            PROP_STOCK_UNDELETE,
            PROP_STOCK_UNDERLINE,
            PROP_STOCK_UNDO,
            PROP_STOCK_YES,
            PROP_STOCK_ZOOM_100,
            PROP_STOCK_ZOOM_FIT,
            PROP_STOCK_ZOOM_IN,
            PROP_STOCK_ZOOM_OUT,
            PROP_END
        };
        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }

        static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }
        JSStockID(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSStockID & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSStockID &>(JSStockID::getJSWrapper(cx,obj));
        }

    private:
        static void addStockIdProps(JSContext * cx, JSObject * obj, JSStockIdPropertySpec * cds);
        static JSStockIdPropertySpec * StockIdProperties();
};

template <>
struct JSClassTraits<JSStockID::NATIVE>
    : public JSClassTraitsWrapper<JSStockID::NATIVE, JSStockID> {};

Y60_JSGTK_DECL jsval as_jsval(JSContext *cx, JSStockID::OWNERPTR theOwner, JSStockID::NATIVE * theNative);

Y60_JSGTK_DECL bool convertFrom(JSContext *cx, jsval theValue, JSStockID::NATIVE * & theStockId);


} // namespace

#endif


