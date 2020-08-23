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

    // 打开媒体文件或流媒体
    virtual bool Open(const char* url);

    // 空间需要调用者释放，释放AVPacket空间和数据空间 av_packet_free
    virtual AVPacket* Read();
	virtual AVPacket* ReadVideo();
    virtual void FreeAVPacket(AVPacket** pkt);

    virtual bool IsAudio(AVPacket* pkt);

    // 获取视频参数,返回空间需要清理 avcodec_paramters_free
    AVCodecParameters *CopyVPara();

    // 获取音频参数,返回空间需要清理
    AVCodecParameters *CopyAPara();

    // seek 位置 pos 0.0~1.0
    virtual bool Seek(double pos);

    // 清空读取数据
    virtual void Clear();
    virtual void Close();

    // 媒体总时长
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

