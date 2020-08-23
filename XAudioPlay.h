#pragma once
class XAudioPlay
{
public:
    static XAudioPlay* Get();
    XAudioPlay();
    virtual ~XAudioPlay();

    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;
    virtual bool Open() = 0;
    virtual bool Close() = 0;
	virtual void Clear() = 0;

    // 返回缓冲中还没有播放的时间
    virtual long long GetNoPlayMs() = 0;
    virtual bool WriteData(char* buf, int size) = 0;
    virtual int GetFree() = 0;

	virtual void SetPause(bool isPause) = 0;
	bool isPause = false;
};

