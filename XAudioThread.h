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

    // ���ܳɹ��������
    virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
    virtual void Push(AVPacket* pkt);
    void run();

    // ������
    int maxList = 10000;
    bool isExit = false;

    // ��ǰ��Ƶ���ŵ�pts
    long long pts = 0;
protected:

    std::list<AVPacket*> packs;
    std::mutex mux;
    XDecode*	decode = nullptr;
    XAudioPlay* ap = nullptr;
    XResample*	res = nullptr;
};

