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

    // �򿪽����������ͷſռ�
    bool isAudio = false;

    // ��ǰ���뵽��pts
    long long pts = 0;
    virtual bool Open(AVCodecParameters* para);

    // ���͵������̣߳�����pkg�ռ䣨�����ý�����ݣ�
    virtual bool Send(AVPacket* pkt);

    // ��ȡ�����̣߳�һ��send������Ҫ���Recv�����һ��send null��recv���
    // ÿ�θ���һ���ɵ������ͷ� av_frame_free
    virtual AVFrame* Recv();

    virtual void Clear();
    virtual void Close();

protected:
    AVCodecContext* codec = nullptr;
    std::mutex mux;
};

