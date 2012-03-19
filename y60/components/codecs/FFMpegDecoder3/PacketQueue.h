#ifndef _ac_y60_PacketQueue_h_
#define _ac_y60_PacketQueue_h_


#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       undef INTMAX_C
#       undef UINTMAX_C
#       pragma warning(push,1)
#   endif
    extern "C" {
#   include <avformat.h>
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif

#include <asl/base/Ptr.h>
#include <asl/thread/concurrent_queue.h>

namespace y60 {
    class PacketMsg {
	    public:
	        PacketMsg() : _myPacket(0) {};
	        PacketMsg(AVPacket * thePacket) : _myPacket(thePacket) {};
	        ~PacketMsg() {/*av_free_packet(_myPacket); delete _myPacket;*/};
	
	        AVPacket * getPacket() { return _myPacket;}
	        void freePacket() {
                if(_myPacket) {
                    av_free_packet(_myPacket);
                    delete _myPacket;
                    _myPacket = 0;
                }
            }
	    private:
	        AVPacket * _myPacket;
	};
	typedef asl::Ptr<PacketMsg> PacketMsgPtr;
	typedef asl::thread::concurrent_queue<PacketMsgPtr> PacketQueue;
	typedef asl::Ptr<PacketQueue> PacketQueuePtr;

}
#endif

