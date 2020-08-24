#pragma once
#include <QThread>
#include <list>
#include "IVideoCall.h"
#include "XDecodeThread.h"

class XVideoThread : public XDecodeThread {
  public:
    XVideoThread();
    virtual ~XVideoThread();

    // 不管成功与否都清理
    virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
    //virtual bool Stop();
    void run();

    // 解码pts，如果接受到的解码数据pts>=seekPts return true
    virtual bool RepaintPts(AVPacket* pkt, long long seekPts);

    // 同步时间由外部传入
    long long synpts = 0;
    long long pts = 0;
    virtual void Clear();

    void SetPause(bool isPause);
    bool isPause = false;
  protected:
    std::mutex vmux;
    IVideoCall* call = nullptr;
};

