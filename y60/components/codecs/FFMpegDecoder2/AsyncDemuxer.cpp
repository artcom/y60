#include "AsyncDemuxer.h"

#include <asl/base/Logger.h>

#define DB(x) x

namespace {
    const unsigned int PACKET_QUEUE_SIZE = 50;
}

namespace y60 {

AsyncDemuxer::AsyncDemuxer(AVFormatContext * myFormatContext)
    : _myFormatContext(myFormatContext) {
}

AsyncDemuxer::~AsyncDemuxer() {
    stop();
}

void
AsyncDemuxer::join() {
    _myDemuxThread->interrupt();
    _myDemuxThread->join();
}

void
AsyncDemuxer::stop() {
    _myDemuxThread->interrupt();
    _myDemuxThread->join();
    clearPacketCache();
}
void
AsyncDemuxer::start() {
    _myDemuxThread = DemuxThreadPtr(new boost::thread( boost::bind( &AsyncDemuxer::run, this)));
}

void
AsyncDemuxer::run() {
    try {
        while(true) {
            DB(AC_TRACE << "---AsyncDemuxer::run: ");
            boost::this_thread::interruption_point();
            //XXX: rethink that: add condition: wait as long as queues are full
            if (queuesFull()) {
                //boost::this_thread::sleep(boost::posix_time::millisec(10));
                boost::this_thread::yield();
                continue;
            }
            AVPacket * myPacket = new AVPacket;
            memset(myPacket, 0, sizeof(AVPacket));

            int ret = av_read_frame(_myFormatContext, myPacket);
            if (ret < 0) {
                if (ret == AVERROR_EOF) {
                    AC_DEBUG << "---AsyncDemuxer::run: end of file.";
                }
                av_free_packet(myPacket);
                delete myPacket;
                myPacket = 0;
                putPacket(myPacket);
                break;
            }
            putPacket(myPacket);
        }
    } catch (boost::thread_interrupted &) {
        AC_DEBUG << "---AsyncDemuxer::stop run";
    }
}

void
AsyncDemuxer::putPacket(AVPacket * thePacket) {
    std::map<int, PacketQueuePtr>::iterator it = _myPacketQueues.find(thePacket->stream_index);
    if ( it != _myPacketQueues.end()) {
        DB(AC_TRACE << "---AsyncDemuxer::run: caching packet. " <<it->second->size());
        // Without av_dup_packet, ffmpeg reuses myPacket->data at first
        // opportunity and trashes our memory.
        if (thePacket) {
            av_dup_packet(thePacket);
        }
        PacketQueue & myQueue = *(it->second);
        PacketMsgPtr p = PacketMsgPtr(new PacketMsg(thePacket));
        myQueue.push(p);
    } else if (thePacket) {
        DB(AC_DEBUG << "---AsyncDemuxer::run: rejecting packet.");
        av_free_packet(thePacket);
        delete thePacket;
        thePacket = 0;
    }
}

AVPacket*
AsyncDemuxer::getPacket(const int theStreamIndex) {
    std::map<int, PacketQueuePtr>::iterator it = _myPacketQueues.find(theStreamIndex);
    if (it == _myPacketQueues.end()) {
        AC_ERROR << "AsyncDemuxer::getPacket called with nonexistent stream index " << theStreamIndex << ".";
    }
    PacketMsgPtr p;
    if(!it->second->timed_wait_and_pop<boost::posix_time::millisec>(p, boost::posix_time::millisec(5000))) {
        stop();
        throw asl::Exception("demuxer can't deliver anymore packets", PLUS_FILE_LINE);
    }
    return p->getPacket();
}

void
AsyncDemuxer::enableStream(const int theStreamIndex) {
    AC_INFO<<"AsyncDemuxer::enableStream with index: "<<theStreamIndex;
    _myPacketQueues[theStreamIndex] = PacketQueuePtr(new PacketQueue());
}

void
AsyncDemuxer::clearPacketCache() {
    for (std::map<int, PacketQueuePtr>::iterator it = _myPacketQueues.begin(); it != _myPacketQueues.end(); ++it) {
        PacketMsgPtr p;
        while(it->second->try_pop(p)) {
            p->freePacket();
        }
    }
}

void
AsyncDemuxer::clearPacketCache(const int theStreamIndex) {
    std::map<int, PacketQueuePtr>::iterator it = _myPacketQueues.find(theStreamIndex);
    if (it == _myPacketQueues.end()) {
        AC_ERROR << "AsyncDemuxer::clearPacketCache called with nonexistent stream index " << theStreamIndex << ".";
    }
    PacketMsgPtr p;
    while(it->second->try_pop(p)) {
        p->freePacket();
    }
}

void
AsyncDemuxer::dump() const {
    AC_DEBUG << "Demux list sizes: ";
    for (std::map<int, PacketQueuePtr>::const_iterator it = _myPacketQueues.begin(); it != _myPacketQueues.end(); ++it) {
        AC_DEBUG << "  " << it->second->size();
    }
}

bool
AsyncDemuxer::queuesFull() const {
    bool queuesFull = true;
    for (std::map<int, PacketQueuePtr>::const_iterator it = _myPacketQueues.begin(); it != _myPacketQueues.end(); ++it) {
        queuesFull &= (it->second->size() >= PACKET_QUEUE_SIZE);
    }
    return queuesFull;
}

}
