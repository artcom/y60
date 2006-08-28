//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================


#include "InputEventWrapper.h"


using namespace std;
using namespace asl;

namespace y60 {

    static const char *valid_prefix[NUM_VALID_PREFIXES] = {
        "Griffin PowerMate",
        "Griffin SoundKnob"
    };
    
    int
    InputEventWrapper::find_powermate(int mode)
    {
        char devname[256];
        int i, r;
        
        for(i=0; i<NUM_EVENT_DEVICES; i++){
            sprintf(devname, "/dev/input/event%d", i);
            r = open_powermate(devname, mode);
            if(r >= 0)
                return r;
        }

        return -1;
    }

    int
    InputEventWrapper::open_powermate(const char *dev, int mode)
    {
        int fd = open(dev, mode);
        int i;
        char name[255];
        
        if(fd < 0){
            fprintf(stderr, "Unable to open \"%s\": %s\n", dev, strerror(errno));
            return -1;
        }
        
        if(ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0){
            fprintf(stderr, "\"%s\": EVIOCGNAME failed: %s\n", dev, strerror(errno));
            close(fd);
            return -1;
        }
        
        // it's the correct device if the prefix matches what we expect it to be:
        for(i=0; i<NUM_VALID_PREFIXES; i++)
            if(!strncasecmp(name, valid_prefix[i], strlen(valid_prefix[i])))
                return fd;
        
        close(fd);
        return -1;
    }

    void
    InputEventWrapper::process_event(struct input_event *ev)
    {
        switch(ev->type){
        case EV_MSC:
            printf("The LED pulse settings were changed; code=0x%04x, value=0x%08x\n", ev->code, ev->value);
            break;
        case EV_REL:
            if(ev->code != REL_DIAL)
                fprintf(stderr, "Warning: unexpected rotation event; ev->code = 0x%04x\n", ev->code);
            else{
                abs_offset += (int)ev->value;
                printf("Button was rotated %d units; Offset from start is now %d units\n", (int)ev->value, abs_offset);
            }
            break;
        case EV_KEY:
            if(ev->code != BTN_0)
                fprintf(stderr, "Warning: unexpected key event; ev->code = 0x%04x\n", ev->code);
            else
                printf("Button was %s\n", ev->value? "pressed":"released");
            break;
        default:
            fprintf(stderr, "Warning: unexpected event type; ev->type = 0x%04x\n", ev->type);
        }

        fflush(stdout);
    }


    
    void
    InputEventWrapper::watch_powermate(int fd)
    {
        struct input_event ibuffer[BUFFER_SIZE];
        int r, events, i;
        
        while(1){
            r = read(fd, ibuffer, sizeof(struct input_event) * BUFFER_SIZE);
            if( r > 0 ){
                events = r / sizeof(struct input_event);
                for(i=0; i<events; i++)
                    process_event(&ibuffer[i]);
            }else{
                fprintf(stderr, "read() failed: %s\n", strerror(errno));
                return;
            }
        }
    }

    
}
