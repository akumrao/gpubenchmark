#include "base/packetstream.h"

#include "base/logger.h"

#include <assert.h>

namespace base {

    PacketStream::PacketStream() {
    }

    PacketStream::~PacketStream() {


        for (auto source : _sources) {
            std::lock_guard<std::mutex> guard(_procMutex);
            if (freeSources)
                delete source;
        }


        for (auto processor : _processors) {
            std::lock_guard<std::mutex> guard(_procMutex);
            if (freeProcessors)
                delete processor;

        }
        
        
        _sources.clear();
        _processors.clear();

    }
    
    
    void PacketStream::attachSource(PacketProcessor* source, bool freePointer) {
        LTrace("Attach source: ", source)

        freeSources = freePointer;                 
        std::lock_guard<std::mutex> guard(_mutex);
        _sources.push_back(source);

    }

   void PacketStream::attach(PacketProcessor* proc, bool freePointer) {
        // LTrace("Attach processor: ", proc)
        freeProcessors = freePointer; 
        std::lock_guard<std::mutex> guard(_mutex);
        _processors.push_back(proc);


    }
    void PacketStream::start() {
        using std::placeholders::_1;

        for (auto source : _sources) {
            std::lock_guard<std::mutex> guard(_procMutex);

            source->cbProcess = std::bind(&PacketStream::process, this, _1);



        }

    }

    void PacketStream::stop() {

        for (auto source : _sources) {
            std::lock_guard<std::mutex> guard(_procMutex);


        }
    }



    void PacketStream::process(IPacket& packet) {
       // STrace << "Processing packet: " << ": " << packet.className();
        // assert(Thread::currentID() == _runner->tid());
         
        LTrace(packet.className());

        for (auto processor : _processors) {
            std::lock_guard<std::mutex> guard(_procMutex);

            if (processor->accepts(&packet))
                processor->process(packet);
            else
                processor->emit(packet);

        }
        
      // emit(packet);

    }

} // namespace base


