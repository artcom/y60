#ifndef _ac_y60_AsyncDemuxer_h_
#define _ac_y60_AsyncDemuxer_h_

#include <asl/base/Ptr.h>
#include <asl/thread/concurrent_queue.h>

#include <boost/thread.hpp>

#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       define EMULATE_INTTYPES
#       pragma warning(push,1)
#   endif
    extern "C" {
#   include <avformat.h>
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif

#include <map>

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

    typedef asl::Ptr<boost::thread> DemuxThreadPtr;

    class AsyncDemuxer {
        public:
            AsyncDemuxer(AVFormatContext * theFormatContext);
            ~AsyncDemuxer();

            void enableStream(const int theStreamIndex);
            AVPacket * getPacket(const int theStreamIndex);
            void clearPacketCache();
            void clearPacketCache(const int theStreamIndex);
            void dump() const;

            void start();
            void stop();
            void join();

        private:
            AVFormatContext * _myFormatContext;
            DemuxThreadPtr _myDemuxThread;
            std::map<int, PacketQueuePtr > _myPacketQueues;
            //boost::condition _myQueueCondition;
            //boost::mutex _myMutex;
            void putPacket(AVPacket * thePacket);
            bool queuesFull() const;
            //threadfunc
            void run();
    };
    typedef asl::Ptr<AsyncDemuxer> AsyncDemuxerPtr;
}
#endif

