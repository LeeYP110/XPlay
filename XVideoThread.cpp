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

    Clear();

    vmux.lock();
    synpts = 0;
    this->call = call;
    if (call)
    {
        call->Init(width, height);
    }
    vmux.unlock();

    bool re = decode->Open(para); // 函数中释放para
    if (re == false)
    {
        std::cout << "video decode->Open(para) failed" << std::endl;
        //mux.unlock();
        //return false;
    }

    return re;
}



void XVideoThread::run()
{
    while (!isExit)
    {
        vmux.lock();

        // 同步判断
        if (synpts > 0 && synpts < decode->pts)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }

//         if (packs.empty() || !decode)
//         {
//             vmux.unlock();
//             msleep(1);
//             continue;
//         }
//
//         AVPacket* pkt = packs.front();
//         packs.pop_front();

        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if (re == false)
        {
            vmux.unlock();
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

        vmux.unlock();
    }
}
