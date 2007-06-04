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

#include <dlfcn.h>
#include <iostream>
#include <sstream>

using namespace std;

namespace asl {

MachOBacktrace::MachOBacktrace() {
};


#define RETURN_ADDRESS_FOR_LEVEL( level )             \
    case level:                                       \
        mySavedReturnAddress = __builtin_return_address( level ); \
        break;

#define FETCH_RETURN_ADDRESS( level ) \
    switch ( level ) {     \
        RETURN_ADDRESS_FOR_LEVEL( 0 );  \
        RETURN_ADDRESS_FOR_LEVEL( 1 );  \
        RETURN_ADDRESS_FOR_LEVEL( 2 );  \
        RETURN_ADDRESS_FOR_LEVEL( 3 );  \
        RETURN_ADDRESS_FOR_LEVEL( 4 );  \
        RETURN_ADDRESS_FOR_LEVEL( 5 );  \
        RETURN_ADDRESS_FOR_LEVEL( 6 );  \
        RETURN_ADDRESS_FOR_LEVEL( 7 );  \
        RETURN_ADDRESS_FOR_LEVEL( 8 );  \
        RETURN_ADDRESS_FOR_LEVEL( 9 );  \
        RETURN_ADDRESS_FOR_LEVEL( 11 ); \
        RETURN_ADDRESS_FOR_LEVEL( 12 ); \
        RETURN_ADDRESS_FOR_LEVEL( 13 ); \
        RETURN_ADDRESS_FOR_LEVEL( 14 ); \
        RETURN_ADDRESS_FOR_LEVEL( 15 ); \
        RETURN_ADDRESS_FOR_LEVEL( 16 ); \
        RETURN_ADDRESS_FOR_LEVEL( 17 ); \
        RETURN_ADDRESS_FOR_LEVEL( 18 ); \
        RETURN_ADDRESS_FOR_LEVEL( 19 ); \
        RETURN_ADDRESS_FOR_LEVEL( 20 ); \
        RETURN_ADDRESS_FOR_LEVEL( 21 ); \
        RETURN_ADDRESS_FOR_LEVEL( 22 ); \
        RETURN_ADDRESS_FOR_LEVEL( 23 ); \
        RETURN_ADDRESS_FOR_LEVEL( 24 ); \
        RETURN_ADDRESS_FOR_LEVEL( 25 ); \
        RETURN_ADDRESS_FOR_LEVEL( 26 ); \
        RETURN_ADDRESS_FOR_LEVEL( 27 ); \
        RETURN_ADDRESS_FOR_LEVEL( 28 ); \
        RETURN_ADDRESS_FOR_LEVEL( 29 ); \
        RETURN_ADDRESS_FOR_LEVEL( 30 ); \
        RETURN_ADDRESS_FOR_LEVEL( 31 ); \
        RETURN_ADDRESS_FOR_LEVEL( 32 ); \
        RETURN_ADDRESS_FOR_LEVEL( 33 ); \
        RETURN_ADDRESS_FOR_LEVEL( 34 ); \
        RETURN_ADDRESS_FOR_LEVEL( 35 ); \
        RETURN_ADDRESS_FOR_LEVEL( 36 ); \
        RETURN_ADDRESS_FOR_LEVEL( 37 ); \
        RETURN_ADDRESS_FOR_LEVEL( 38 ); \
        RETURN_ADDRESS_FOR_LEVEL( 39 ); \
        RETURN_ADDRESS_FOR_LEVEL( 40 ); \
        RETURN_ADDRESS_FOR_LEVEL( 41 ); \
        RETURN_ADDRESS_FOR_LEVEL( 42 ); \
        RETURN_ADDRESS_FOR_LEVEL( 43 ); \
        RETURN_ADDRESS_FOR_LEVEL( 44 ); \
        RETURN_ADDRESS_FOR_LEVEL( 45 ); \
        RETURN_ADDRESS_FOR_LEVEL( 46 ); \
        RETURN_ADDRESS_FOR_LEVEL( 47 ); \
        RETURN_ADDRESS_FOR_LEVEL( 48 ); \
        RETURN_ADDRESS_FOR_LEVEL( 49 ); \
        RETURN_ADDRESS_FOR_LEVEL( 50 ); \
        RETURN_ADDRESS_FOR_LEVEL( 51 ); \
        RETURN_ADDRESS_FOR_LEVEL( 52 ); \
        RETURN_ADDRESS_FOR_LEVEL( 53 ); \
        RETURN_ADDRESS_FOR_LEVEL( 54 ); \
        RETURN_ADDRESS_FOR_LEVEL( 55 ); \
        RETURN_ADDRESS_FOR_LEVEL( 56 ); \
        RETURN_ADDRESS_FOR_LEVEL( 57 ); \
        RETURN_ADDRESS_FOR_LEVEL( 58 ); \
        RETURN_ADDRESS_FOR_LEVEL( 59 ); \
        RETURN_ADDRESS_FOR_LEVEL( 60 ); \
        RETURN_ADDRESS_FOR_LEVEL( 61 ); \
        RETURN_ADDRESS_FOR_LEVEL( 62 ); \
        RETURN_ADDRESS_FOR_LEVEL( 63 ); \
    }



void
MachOBacktrace::trace(std::vector<StackFrame> & theStack, int theMaxDepth) {
//    cerr << "MachOBacktrace::trace()" << endl;
    theStack.clear();


    unsigned int myLevel( 0 );
    void *  mySavedReturnAddress( __builtin_return_address(0) );
    void ** myFramePtr( (void **)__builtin_frame_address(0) );
    void *  mySavedFramePtr( __builtin_frame_address(1) );
    StackFrame myItem;

    myItem.frame = (ptrdiff_t)myFramePtr;
    myItem.name = getFunctionName( mySavedReturnAddress );
    theStack.push_back( myItem );
    myLevel++;
    myFramePtr = (void**) mySavedFramePtr;
    while (myFramePtr) {
        mySavedFramePtr = * myFramePtr;
        myFramePtr = (void**) mySavedFramePtr;
        if ( * myFramePtr == NULL) {
            break;
        }
    
        FETCH_RETURN_ADDRESS( myLevel );

        myItem.frame = (ptrdiff_t)myFramePtr;
        myItem.name = getFunctionName( mySavedReturnAddress );
        theStack.push_back( myItem );
        myLevel++;
    }

};

#undef RETURN_ADDRESS_FOR_LEVEL
#undef FETCH_RETURN_ADDRESS

std::string
MachOBacktrace::getFunctionName( void * theReturnAddress) {
    Dl_info myInfo;
    int myResult = dladdr( theReturnAddress, & myInfo );
    if (myResult) {
        return string( myInfo.dli_sname );
    }
    return string("??");
}

}

