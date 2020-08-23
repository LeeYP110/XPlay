#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"

XAudioThread::XAudioThread()
{
    maxList = 1;

    if (res == nullptr)
    {
        res = new XResample();
    }

    if (ap == nullptr)
    {
        ap = XAudioPlay::Get();
    }
}

XAudioThread::~XAudioThread()
{
}

bool XAudioThread::Open(AVCodecParameters * para, int sampleRate, int channels)
{
    if (para == nullptr)
    {
        return false;
    }

    Clear();

    // TODO 还需要清理qio中数据

    amux.lock();
    pts = 0;
    bool re = res->Open(para, false);
    if (re == false)
    {
        std::cout << "res->Open(para, false) failed" << std::endl;
        //mux.unlock();
        //return false;
    }

    ap->sampleRate = sampleRate;
    ap->channels = channels;
    re = ap->Open();
    if (re == false)
    {
        std::cout << "ap->Open() failed" << std::endl;
        //mux.unlock();
        //return false;
    }

    re = decode->Open(para); // 函数中释放para
    if (re == false)
    {
        std::cout << "audio decode->Open(para) failed" << std::endl;
        //mux.unlock();
        //return false;
    }
    amux.unlock();

    return re;
}

void XAudioThread::Clear()
{
	XDecodeThread::Clear();
	mux.lock();
	if (ap)
	{
		ap->Clear();
	}
	mux.unlock();
}

void XAudioThread::Close()
{
    XDecodeThread::Close();

    if (res)
    {
        res->Close();
        amux.lock();
        delete res;
        res = nullptr;
        amux.unlock();
    }

    if (ap)
    {
        ap->Close();
        amux.lock();
        ap = nullptr;
        amux.unlock();
    }
}


void XAudioThread::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024];

    while (!isExit)
    {
        amux.lock();
		if (isPause)
		{
			amux.unlock();
			msleep(1);
			continue;
		}

        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if (re == false)
        {
            amux.unlock();
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

            // 减去缓冲中未播放的时间
            pts = decode->pts - ap->GetNoPlayMs();
            //std::cout << "audio pts = " << pts << std::endl;

            // 重采样
            int size = res->Resample(frame, pcm);

            // 播放音频
            while (!isExit)
            {
                if (size <= 0)
                {
                    break;
                }

                if (ap->GetFree() < size || isPause)
                {
                    msleep(1);
                    continue;
                }
                ap->WriteData((char*)pcm, size);
                break;
            }
        }
        amux.unlock();
    }
}

void XAudioThread::SetPause(bool isPause)
{
	amux.lock();
	this->isPause = isPause;
	if (ap)
	{
		ap->SetPause(isPause);
	}
	amux.unlock();
}
