//==============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#include "MachOBacktrace.h"

#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <cxxabi.h>

#include <iostream>
#include <sstream>

using namespace std;

namespace asl {

static std::string getFunctionName(unsigned thePC, unsigned * theOffset);
static const mach_header * findOwnerOfPC( unsigned thePC);
static std::string beautify(const std::string & theSymbol);

struct NativeFrame {
    unsigned savedSP;
    unsigned savedCR;
    unsigned savedLR;
    unsigned reserved[2];
    unsigned savedRTOC;
};

MachOBacktrace::MachOBacktrace() {
};

void
MachOBacktrace::trace(std::vector<StackFrame> & theStack, int theMaxDepth) {
//    cerr << "MachOBacktrace::trace()" << endl;
    theStack.clear();

    NativeFrame * myFrame;
    __asm__ volatile("lwz %0,0(r1)" : "=r" (myFrame));
    int i;
    std::vector<StackFrame> myStack;
    for (i = 0; ((i < theMaxDepth) && (myFrame != 0)); i++,
            myFrame = (NativeFrame*)myFrame->savedSP)
    {
        if (((myFrame->savedLR & ~3) == 0) || ((~(myFrame->savedLR) & ~3) == 0)) {
            break;
        }

        StackFrame myItem;
        myItem.caller = (unsigned)myFrame->savedLR;
        myItem.frame = (unsigned)myFrame;
        myItem.name = getFunctionName(myFrame->savedLR, & myItem.offset);
        if (myItem.caller != 0) {
            myItem.caller -= 4;
        }
        myStack.push_back( myItem );
    }

    std::vector<StackFrame>::reverse_iterator myIt = myStack.rbegin();
    for (; myIt != myStack.rend(); ++myIt) {
        theStack.push_back(*myIt);
    }
};

std::string
getFunctionName(unsigned thePC, unsigned * theOffset) {
    std::string myResult;
    const mach_header * myHeader = findOwnerOfPC( thePC );
    if (myHeader != 0) {
        segment_command * mySegText(0);
        segment_command * mySegLinkEdit(0);
        symtab_command * mySymTab(0);

        load_command * myCmd = (load_command*)((char*)myHeader + sizeof( mach_header));
        for (unsigned i = 0; i < myHeader->ncmds; ++i,
                myCmd = (load_command*)((char*)myCmd + myCmd->cmdsize))
        {
            switch (myCmd->cmd) {
                case LC_SEGMENT:
                    if ( ! strcmp(((segment_command*)myCmd)->segname, SEG_TEXT)) {
                        mySegText = (segment_command*)myCmd;
                    } else if ( ! strcmp(((segment_command*)myCmd)->segname, SEG_LINKEDIT)) {
                        mySegLinkEdit = (segment_command*)myCmd;
                    }
                    break;
                case LC_SYMTAB:
                    mySymTab = (symtab_command*)myCmd;
                    break;
            }
        }

        if (( mySegText == 0) || (mySegLinkEdit == 0) || (mySymTab == NULL)) {
            *theOffset = 0;
            return myResult;
        }

        unsigned myVmSlide = (unsigned long)myHeader - (unsigned long) mySegText->vmaddr;
        unsigned myFileSlide = ((unsigned long) mySegLinkEdit->vmaddr - (unsigned long)mySegText->vmaddr) - mySegLinkEdit->fileoff;
        struct nlist * mySymBase = (struct nlist*)((unsigned long) myHeader + (mySymTab->symoff + myFileSlide));
        char * myStrings = (char*)((unsigned long)myHeader + (mySymTab->stroff + myFileSlide));

        struct nlist * mySym = mySymBase;
        char * myName;
        unsigned myBase;
        for (unsigned i = 0; i < mySymTab->nsyms; ++i, mySym += 1) {
            if (mySym->n_type != N_FUN) {
                continue;
            }

            myName = mySym->n_un.n_strx ? (myStrings + mySym->n_un.n_strx) : 0;
            myBase = mySym->n_value + myVmSlide;
            
            for (i += 1, mySym += 1; i < mySymTab->nsyms; ++i, ++mySym) {
                if (mySym->n_type == N_FUN) {
                    break;
                }
            }
            if ((thePC >= myBase) && (thePC <= (myBase + mySym->n_value)) &&
                    (myName != 0) && (strlen(myName) > 0))
            {
                *theOffset = thePC - myBase;
                myResult = myName;
                return myResult;
            }
        }


        myName = 0;
        myBase = 0xFFFFFFFF;
        mySym = mySymBase;
        for (unsigned i = 0; i < mySymTab->nsyms; ++i, ++mySym) {
            if ((mySym->n_type & 0x0E) != 0x0E) {
                continue;
            }
            if ((mySym->n_value + myVmSlide) > thePC) {
                continue;
            }
            if ((myBase != 0xFFFFFFFF) && ((thePC - (mySym->n_value + myVmSlide)) >= (thePC - myBase))) {
                continue;
            }

            myName = mySym->n_un.n_strx ? (myStrings + mySym->n_un.n_strx) : 0;
            myBase = mySym->n_value + myVmSlide;
        }

        *theOffset = thePC - myBase;
        if (myName) {
            myResult = myName;
        }
        return myResult;
    }
    *theOffset = 0;
    return 0;
}

const struct mach_header * 
findOwnerOfPC( unsigned thePC) {
    unsigned myCount = _dyld_image_count();
    for (unsigned i = 0; i < myCount; ++i) {
        const mach_header * myHeader = _dyld_get_image_header(i);
        unsigned myOffset = _dyld_get_image_vmaddr_slide(i);
        load_command * myCmd = (load_command*)((char*)myHeader + sizeof(mach_header));
        for (unsigned myCmdEx = 0; myCmdEx < myHeader->ncmds; ++myCmdEx,
                myCmd = (load_command*)((char*)myCmd + myCmd->cmdsize))
        {
            switch (myCmd->cmd) {
                case LC_SEGMENT:
                    segment_command * mySegment = (segment_command*) myCmd;
                    if ((thePC >= (mySegment->vmaddr + myOffset)) &&
                        (thePC < (mySegment->vmaddr + myOffset + mySegment->vmsize)))
                    {
                        return myHeader;
                    }
                    break;
            }
        }
    }
    return 0;
}

}

