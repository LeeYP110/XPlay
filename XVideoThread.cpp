#include "XVideoThread.h"
#include "XDecode.h"

XVideoThread::XVideoThread() {
}

XVideoThread::~XVideoThread() {
    isExit = true;
    wait();
}

bool XVideoThread::Open(AVCodecParameters * para, IVideoCall* call, int width, int height) {
    if (para == nullptr) {
        return false;
    }

    Clear();

    vmux.lock();
    synpts = 0;
    this->call = call;
    if (call) {
        call->Init(width, height);
    }
    vmux.unlock();

    bool re = decode->Open(para); // �������ͷ�para
    if (re == false) {
        std::cout << "video decode->Open(para) failed" << std::endl;
        //mux.unlock();
        //return false;
    }

    return re;
}

void XVideoThread::run() {
    while (!isExit) {
        vmux.lock();

        if (isPause) {
            vmux.unlock();
            msleep(5);
            continue;
        }

        // ͬ���ж�
        pts = decode->pts;
        if (synpts > 0 && synpts < decode->pts) {
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
        static int sendNum = 0;
        static int recvNum = 0;

        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if (re == false) {
            vmux.unlock();
            msleep(1);
            continue;
        }
        sendNum++;
        // һ��send ���recv
        while (!isExit) {
            AVFrame* frame = decode->Recv();
            if (!frame) {
                break;
            }
            recvNum++;
            // ��ʾ��Ƶ
            if (call) {
                msleep(40);
                call->Repaint(frame);
            }
        }

        vmux.unlock();
    }
}

bool XVideoThread::RepaintPts(AVPacket * pkt, long long seekPts) {
    vmux.lock();
    bool result = decode->Send(pkt);
    if (!result) {
        vmux.unlock();
        return true; // ������������
    }

    AVFrame* frame = decode->Recv();
    if (frame == nullptr) {
        vmux.unlock();
        return false;
    }

    if (decode->pts >= seekPts) {
        if (call) {
            call->Repaint(frame);
        }
        vmux.unlock();
        return true;
    }
    XFreeFrame(&frame);
    vmux.unlock();

    return false;
}

void XVideoThread::Clear() {
    XDecodeThread::Clear();
    mux.lock();
    decode->pts = 0;
    mux.unlock();
}

void XVideoThread::SetPause(bool isPause) {
    //mux.lock();
    this->isPause = isPause;
    //mux.unlock();
}
