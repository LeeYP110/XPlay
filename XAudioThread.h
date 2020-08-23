#pragma once
#include <QThread>
#include <mutex>
#include <list>
#include "XDecodeThread.h"

class XAudioPlay;
class XResample;

class XAudioThread :
    public XDecodeThread
{
public:
    XAudioThread();
    virtual ~XAudioThread();

    // ���ܳɹ��������
    virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
	virtual void Clear();
	virtual void Close();

    void run();

    // ��ǰ��Ƶ���ŵ�pts
    long long pts = 0;

	void SetPause(bool isPause);
	bool isPause = false;
protected:
    std::mutex amux;
    XAudioPlay* ap = nullptr;
    XResample*	res = nullptr;
};

