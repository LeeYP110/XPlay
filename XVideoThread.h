#pragma once
#include <QThread>
#include <list>
#include "IVideoCall.h"
#include "XDecodeThread.h"

class XVideoThread : public XDecodeThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();

    // 不管成功与否都清理
    virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
    //virtual bool Stop();
    void run();

    // 同步时间由外部传入
    long long synpts = 0;
protected:
    std::mutex vmux;
    IVideoCall* call = nullptr;
};

