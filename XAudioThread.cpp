#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"

XAudioThread::XAudioThread()
{

}

XAudioThread::~XAudioThread()
{
    isExit = true;
    wait();
}

bool XAudioThread::Open(AVCodecParameters * para, int sampleRate, int channels)
{
    if (para == nullptr)
    {
        return false;
    }

    mux.lock();
    pts = 0;
    if (decode == nullptr)
    {
        decode = new XDecode();
    }

    if (res == nullptr)
    {
        res = new XResample();
    }

    if (ap == nullptr)
    {
        ap = XAudioPlay::Get();
    }

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

    re = decode->Open(para); // �������ͷ�para
    if (re == false)
    {
        std::cout << "audio decode->Open(para) failed" << std::endl;
        //mux.unlock();
        //return false;
    }
    mux.unlock();

    return re;
}

void XAudioThread::Push(AVPacket * pkt)
{
    if (pkt == nullptr)
    {
        return;
    }

    // ����
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

void XAudioThread::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024 * 10];

    while (!isExit)
    {
        mux.lock();
        if (packs.empty() || !decode || !res || !ap)
        {
            mux.unlock();
            msleep(1);
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

        // һ��send ���recv
        while (!isExit)
        {
            AVFrame* frame = decode->Recv();
            if (!frame)
            {
                break;
            }

            // ��ȥ������δ���ŵ�ʱ��
            pts = decode->pts - ap->GetNoPlayMs();
            //std::cout << "audio pts = " << pts << std::endl;

            // �ز���
            int size = res->Resample(frame, pcm);

            // ������Ƶ
            while (!isExit)
            {
                if (size <= 0)
                {
                    break;
                }

                if (ap->GetFree() < size)
                {
                    msleep(1);
                    continue;
                }
                ap->WriteData((char*)pcm, size);
                break;
            }
        }

        mux.unlock();
    }
}
