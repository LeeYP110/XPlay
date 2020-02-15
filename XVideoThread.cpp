#include "XVideoThread.h"
#include "XDecode.h"

XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{
    isExit = true;
    wait();
}

bool XVideoThread::Open(AVCodecParameters * para, IVideoCall* call, int width, int height)
{
    if (para == nullptr)
    {
        return false;
    }

    this->call = call;
    if (call)
    {
        call->Init(width, height);
    }

    mux.lock();
    synpts = 0;
    if (decode == nullptr)
    {
        decode = new XDecode();
    }

    bool re = decode->Open(para); // 函数中释放para
    if (re == false)
    {
        std::cout << "video decode->Open(para) failed" << std::endl;
        //mux.unlock();
        //return false;
    }

    mux.unlock();
    return re;
}

void XVideoThread::Push(AVPacket * pkt)
{
    if (pkt == nullptr)
    {
        return;
    }

    // 阻塞
    while (!isExit)
    {
        mux.lock();
        if (packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}

void XVideoThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (packs.empty() || !decode)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        // 同步判断
        if (synpts < decode->pts)
        {
            mux.unlock();
            msleep(5);
            continue;
        }


        AVPacket* pkt = packs.front();
        packs.pop_front();

        bool re = decode->Send(pkt);
        if (re == false)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        // 一次send 多次recv
        while (!isExit)
        {
            AVFrame* frame = decode->Recv();
            if (!frame)
            {
                break;
            }

            // 显示视频
            if (call)
            {
                call->Repaint(frame);
            }
        }

        mux.unlock();
    }
}
