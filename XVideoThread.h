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

    // ���ܳɹ��������
    virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
    virtual void Push(AVPacket* pkt);
    void run();

    // ������
    int maxList = 100;
    bool isExit = false;

    // ͬ��ʱ�����ⲿ����
    long long synpts = 0;
protected:

    std::list<AVPacket*> packs;
    std::mutex mux;
    XDecode*	decode = nullptr;
    IVideoCall* call = nullptr;
};

