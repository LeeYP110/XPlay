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
	virtual void Clear();

	virtual void Seek(double pos);

    void run();
    bool isExit = false;

	long long pts = 0;
	long long totalMs = 0;

	void SetPause(bool isPause);
	bool isPause = false;
protected:
    std::mutex mux;
    XDemux* demux = nullptr;
    XVideoThread* vt = nullptr;
    XAudioThread* at = nullptr;
};

