//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef ACGTK_CONSOLE_VIEW_INCLUDED
#define ACGTK_CONSOLE_VIEW_INCLUDED

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4413 4244 4512)
#endif //defined(_MSC_VER)
#include <gtkmm/textview.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/math/Vector234.h>

namespace acgtk {

class ConsoleView : public Gtk::TextView {
    public:
        ConsoleView();
        virtual ~ConsoleView();

        void append(const Glib::ustring & theText, const Glib::ustring & theTagName = "");

        void setScrollback(unsigned theLineCount);
        unsigned getScrollback() const;

        void addTag(const Glib::ustring & theTagName, const asl::Vector3f & theTextColor,
                const asl::Vector3f & theBackgroundColor);
    private:
        unsigned _myScrollback;
        unsigned _myCurrentLineCount;
};

} // end of namespace 

#endif // ACGTK_CONSOLE_VIEW_INCLUDED

