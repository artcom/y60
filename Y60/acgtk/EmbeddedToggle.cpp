#include "EmbeddedToggle.h"

namespace acgtk {

EmbeddedToggle::EmbeddedToggle() :
    EmbeddedButton(),
    _myActiveFlag(false)
{
}

EmbeddedToggle::EmbeddedToggle(const std::string & theDefaultIcon, const std::string & theActiveIcon) : 
    EmbeddedButton(theDefaultIcon),
    _myActiveFlag(false)
{
    _myActiveIcon = Gdk::Pixbuf::create_from_file(theActiveIcon);
}

void
EmbeddedToggle::on_clicked() {
    set_active(!_myActiveFlag);
    EmbeddedButton::on_clicked();
}

bool
EmbeddedToggle::is_active() {
    return _myActiveFlag;
}

void
EmbeddedToggle::set_active(bool theFlag) {
    if (theFlag != _myActiveFlag) {
        _myActiveFlag = theFlag;
        if (_myActiveFlag) {
            _myImage.set( _myActiveIcon );
        } else {
            _myImage.set(_myIcon);
        }
        _myToggledSignal.emit();
    }
}

}

