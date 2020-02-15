#pragma once
#include <qthread.h>
#include <list>
#include "IVideoCall.h"

struct AVCodecParameters;
struct AVPacket;
class XDecode;


class XVideoThread :
    public QThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();

    // 不管成功与否都清理
    virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
    virtual void Push(AVPacket* pkt);
    void run();

    // 最大队列
    int maxList = 100;
    bool isExit = false;

    // 同步时间由外部传入
    long long synpts = 0;
protected:

    std::list<AVPacket*> packs;
    std::mutex mux;
    XDecode*	decode = nullptr;
    IVideoCall* call = nullptr;
};

