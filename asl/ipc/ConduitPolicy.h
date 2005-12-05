//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ConduitPolicy.h,v $
//   $Author: david $
//   $Revision: 1.5 $
//   $Date: 2005/01/07 16:27:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_ConduitPolicy_included
#define __asl_ConduitPolicy_included

#include <asl/Exception.h>
#include <asl/Ptr.h>
#include <asl/Block.h>
#include <vector>
#include <deque>
#include <sstream>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

DEFINE_EXCEPTION(ConduitException, asl::Exception);
DEFINE_EXCEPTION(ConduitInUseException, ConduitException);
DEFINE_EXCEPTION(ConduitRefusedException, ConduitException);

typedef std::vector<char> CharBuffer;
typedef asl::Ptr<CharBuffer> CharBufferPtr;
typedef std::deque<CharBufferPtr> BufferQueue;

//! Abstract base for Conduit Policy classes.
/**
  The use of policy classes for the low-level protocolls allow the use of mix-and-match conduits. Example policy 
  classes are TCPPolicy and NamedPipePolicy. They can be combined with application-level protocolls (e.g. framed 
  message-based communication with asl::Block based payloads) to provide Messages-over-tcp (MessageAcceptor<TCPPolicy>)
  or Messages-over-Named Pipes (MessageAcceptor<NamedPipePolicy>).
**/
class ConduitPolicy {
    public:
//        enum State { COMPLETED, PENDING, NO_CONNECTION };
        
};

/* @} */
}

#endif

