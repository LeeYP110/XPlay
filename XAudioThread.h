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

    // 不管成功与否都清理
    virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
	virtual void Clear();
	virtual void Close();

    void run();

    // 当前音频播放的pts
    long long pts = 0;

	void SetPause(bool isPause);
	bool isPause = false;
protected:
    std::mutex amux;
    XAudioPlay* ap = nullptr;
    XResample*	res = nullptr;
};

