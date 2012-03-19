#ifndef _ac_y60_AsyncDemuxer_h_
#define _ac_y60_AsyncDemuxer_h_

#include <asl/base/Ptr.h>
#include <asl/thread/concurrent_queue.h>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include <map>

struct AVFormatContext;
struct AVPacket;

namespace y60 {
    
    class PacketMsg;
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
            void seek(double theDestTime);
            void seek(double theDestTime, const int theStreamIndex);
            void stop();
            void join();

        private:
            AVFormatContext * _myFormatContext;
            DemuxThreadPtr _myDemuxThread;
            std::map<int, PacketQueuePtr > _myPacketQueues;
            volatile bool _isSeeking;
            volatile bool _isEOF;
            boost::mutex _myMutex;
            boost::condition _myCondition;
            void putPacket(AVPacket * thePacket);
            bool queuesFull() const;
            //threadfunc
            void run();
    };
    typedef asl::Ptr<AsyncDemuxer> AsyncDemuxerPtr;
}
#endif

