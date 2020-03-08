#pragma once
#include <qthread.h>
#include <mutex>
#include "IVideoCall.h"

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread :
    public QThread
{
public:
    XDemuxThread();
    virtual ~XDemuxThread();

    virtual bool Open(const char* url, IVideoCall* call);

    // 启动所有线程
    virtual bool Start();
    virtual void Close();

    void run();
    bool isExit = false;
protected:
    std::mutex mux;
    XDemux* demux = nullptr;
    XVideoThread* vt = nullptr;
    XAudioThread* at = nullptr;
};

