#pragma once
#include <iostream>
#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
extern void XFreePacket(AVPacket** pkt);
extern void XFreeFrame(AVFrame ** frame);
class XDecode
{
public:
    XDecode();
    virtual ~XDecode();

    // 打开解码器，并释放空间
    bool isAudio = false;

    // 当前解码到的pts
    long long pts = 0;
    virtual bool Open(AVCodecParameters* para);

    // 发送到解码线程，清理pkg空间（对象和媒体内容）
    virtual bool Send(AVPacket* pkt);

    // 获取解码线程，一次send可能需要多次Recv，最后一次send null再recv多次
    // 每次复制一份由调用者释放 av_frame_free
    virtual AVFrame* Recv();

    virtual void Clear();
    virtual void Close();

protected:
    AVCodecContext* codec = nullptr;
    std::mutex mux;
};

