#pragma once
#include <QThread>
#include <list>
#include "IVideoCall.h"
#include "XDecodeThread.h"

class XVideoThread : public XDecodeThread {
  public:
    XVideoThread();
    virtual ~XVideoThread();

    // ���ܳɹ��������
    virtual bool Open(AVCodecParameters* para, IVideoCall* call, int width, int height);
    //virtual bool Stop();
    void run();

    // ����pts��������ܵ��Ľ�������pts>=seekPts return true
    virtual bool RepaintPts(AVPacket* pkt, long long seekPts);

    // ͬ��ʱ�����ⲿ����
    long long synpts = 0;
    long long pts = 0;
    virtual void Clear();

    void SetPause(bool isPause);
    bool isPause = false;
  protected:
    std::mutex vmux;
    IVideoCall* call = nullptr;
};

