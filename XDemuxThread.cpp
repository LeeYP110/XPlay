#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"

XDemuxThread::XDemuxThread()
{
}

XDemuxThread::~XDemuxThread()
{
    isExit = true;
    wait();
}

bool XDemuxThread::Open(const char* url, IVideoCall * call)
{
    if (url == nullptr || url[0] == '\0')
    {
        return false;
    }

    mux.lock();
    if (demux == nullptr)
    {
        demux = new XDemux();
    }

    if (at == nullptr)
    {
        at = new XAudioThread();
    }

    if (vt == nullptr)
    {
        vt = new XVideoThread();
    }

    // 打开解封装
    bool re = demux->Open(url);
    if (!re)
    {
        mux.unlock();
        std::cout << "demux->Open(url) failed" << std::endl;
        return false;
    }

    // 打开解码器
    re = vt->Open(demux->CopyVPara(), call, demux->width, demux->height);
    if (re == false)
    {
        std::cout << "vt->Open  failed" << std::endl;
    }

    re = at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels);
    if (re == false)
    {
        std::cout << "at->Open  failed" << std::endl;
    }
    mux.unlock();
    std::cout << "XDemuxThread::Open: " << re << std::endl;
    return re;
}

bool XDemuxThread::Start()
{
    mux.lock();
    QThread::start();

    if (vt)
    {
        vt->start();
    }

    if (at)
    {
        at->start();
    }

    mux.unlock();
    return false;
}

void XDemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (demux == nullptr)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        // 音视频同步
        if (at && vt)
        {
            vt->synpts = at->pts;
        }

        AVPacket* pkt = demux->Read();
        if (pkt == nullptr)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        // 判断音视频
        if (demux->IsAudio(pkt))
        {
            if (at != nullptr)
            {
                at->Push(pkt);
            }
        }
        else
        {
            if (vt != nullptr)
            {
                vt->Push(pkt);
            }
        }

        mux.unlock();
    }
}
