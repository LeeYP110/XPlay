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

    // ���ܳɹ��������
    virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
    //virtual bool Stop();
    void run();

    // ͬ��ʱ�����ⲿ����
    long long synpts = 0;
protected:
    std::mutex vmux;
    IVideoCall* call = nullptr;
};

