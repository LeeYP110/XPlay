#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVCodecParameters;
struct AVPacket;
class XDecode;
class XAudioPlay;
class XResample;

class XAudioThread :
    public QThread
{
public:
    XAudioThread();
    virtual ~XAudioThread();

    // 不管成功与否都清理
    virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
    virtual void Push(AVPacket* pkt);
    void run();

    // 最大队列
    int maxList = 1000;
    bool isExit = false;
protected:

    std::list<AVPacket*> packs;
    std::mutex mux;
    XDecode*	decode = nullptr;
    XAudioPlay* ap = nullptr;
    XResample*	res = nullptr;
};

