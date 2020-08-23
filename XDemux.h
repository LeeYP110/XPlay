#pragma once
#include <iostream>
#include <thread>
#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
    XDemux();
    virtual ~XDemux();

    // ��ý���ļ�����ý��
    virtual bool Open(const char* url);

    // �ռ���Ҫ�������ͷţ��ͷ�AVPacket�ռ�����ݿռ� av_packet_free
    virtual AVPacket* Read();
	virtual AVPacket* ReadVideo();
    virtual void FreeAVPacket(AVPacket** pkt);

    virtual bool IsAudio(AVPacket* pkt);

    // ��ȡ��Ƶ����,���ؿռ���Ҫ���� avcodec_paramters_free
    AVCodecParameters *CopyVPara();

    // ��ȡ��Ƶ����,���ؿռ���Ҫ����
    AVCodecParameters *CopyAPara();

    // seek λ�� pos 0.0~1.0
    virtual bool Seek(double pos);

    // ��ն�ȡ����
    virtual void Clear();
    virtual void Close();

    // ý����ʱ��
    int64_t totalMs = 0;
    int width = 0;
    int height = 0;

    int sampleRate = 44100;
    int channels = 2;
protected:
    std::mutex mux;
    AVFormatContext* ic = nullptr;

    int videoIndex = 0;
    int audioIndex = 1;
};

