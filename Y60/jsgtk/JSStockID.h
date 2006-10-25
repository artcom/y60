//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSTOCKID_INCLUDED_
#define _Y60_ACGTKSHELL_JSSTOCKID_INCLUDED_

#include "JSWindow.h"
#include <y60/JSWrapper.h>
#include <gtkmm/dialog.h>

#include <asl/string_functions.h>

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

jsval as_jsval(JSContext *cx, JSStockID::OWNERPTR theOwner, JSStockID::NATIVE * theNative);

bool convertFrom(JSContext *cx, jsval theValue, JSStockID::NATIVE * & theStockId);
 

} // namespace

#endif


