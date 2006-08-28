//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef AC_Y60_INPUT_EVENT_WRAPPER_H_
#define AC_Y60_INPUT_EVENT_WRAPPER_H_


#include <dom/Nodes.h>
#include <asl/Exception.h>

#include <string>
#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


#include <linux/input.h>

#define NUM_VALID_PREFIXES 2
#define BUFFER_SIZE 32
#define NUM_EVENT_DEVICES 16    


namespace y60 {

    DEFINE_EXCEPTION(InputEventWrapperException, asl::Exception);


    class InputEventWrapper {
        public:
            InputEventWrapper() : abs_offset(0) {}
            virtual ~InputEventWrapper();

            
        
            int open_powermate(const char *dev, int mode);
            int find_powermate(int mode);

            void process_event(struct input_event *ev);
            void watch_powermate(int fd);
        
            int abs_offset;
        private:
    };
    typedef asl::Ptr<InputEventWrapper, dom::ThreadingModel> InputEventWrapperPtr;
}

#endif // AC_Y60_INPUT_EVENT_WRAPPER_H_
