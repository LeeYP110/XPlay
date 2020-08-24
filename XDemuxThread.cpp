#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"

XDemuxThread::XDemuxThread() {
}

XDemuxThread::~XDemuxThread() {
    isExit = true;
    wait();
}

bool XDemuxThread::Open(const char* url, IVideoCall * call) {
    if (url == nullptr || url[0] == '\0') {
        return false;
    }

    mux.lock();
    if (demux == nullptr) {
        demux = new XDemux();
    }

    if (at == nullptr) {
        at = new XAudioThread();
    }

    if (vt == nullptr) {
        vt = new XVideoThread();
    }

    // 打开解封装
    bool re = demux->Open(url);
    if (!re) {
        mux.unlock();
        std::cout << "demux->Open(url) failed" << std::endl;
        return false;
    }

    // 打开解码器
    AVCodecParameters* vpara = demux->CopyVPara();
    if (vpara) {
        re = vt->Open(vpara, call, demux->width, demux->height);
        if (re == false) {
            std::cout << "vt->Open  failed" << std::endl;
        }
    }

    AVCodecParameters* apara = demux->CopyAPara();
    if (apara) {
        re = at->Open(apara, demux->sampleRate, demux->channels);
        if (re == false) {
            std::cout << "at->Open  failed" << std::endl;
        }
    }

    totalMs = demux->totalMs;
    mux.unlock();
    std::cout << "XDemuxThread::Open: " << re << std::endl;
    return re;
}

bool XDemuxThread::Start() {
    mux.lock();

    if (demux == nullptr) {
        demux = new XDemux();
    }

    if (at == nullptr) {
        at = new XAudioThread();
    }

    if (vt == nullptr) {
        vt = new XVideoThread();
    }

    // 启动线程
    QThread::start();

    if (vt) {
        vt->start();
    }

    if (at) {
        at->start();
    }

    mux.unlock();
    return false;
}

void XDemuxThread::Close() {
    isExit = true;
    wait();
    if (vt) {
        vt->Close();
    }
    if (at) {
        at->Close();
    }

    mux.lock();
    delete vt;
    delete at;
    vt = nullptr;
    at = nullptr;
    mux.unlock();
}

void XDemuxThread::Clear() {
    mux.lock();
    if (demux) {
        demux->Clear();
    }
    if (vt) {
        vt->Clear();
    }
    if (at) {
        at->Clear();
    }
    mux.unlock();
}

void XDemuxThread::Seek(double pos) {
    mux.lock();
    bool status = this->isPause;
    mux.unlock();

    // 必须先暂停再清理，否则容易造成，音视频数据包达到最大，阻塞???
    Clear();
    SetPause(true);

    // 暂停
    std::cout << "3" << std::endl;
    std::cout << "4" << std::endl;
    mux.lock();
    std::cout << "5" << std::endl;
    if (demux) {
        demux->Seek(pos);
    }
    std::cout << "6" << std::endl;
    // 实际的seek帧
    long long seekPts = pos * demux->totalMs;
    pts = seekPts;
    while (!isExit) {
        AVPacket* pkt = demux->ReadVideo();
        if (pkt == nullptr) {
            break;
        }
        bool result = vt->RepaintPts(pkt, seekPts);
        if (result) { // 如果解码到seekPts
            break;
        }
    }
    mux.unlock();
    std::cout << "7" << std::endl;
    SetPause(status);// seek保持seek时状态
    std::cout << "8" << std::endl;
}

void XDemuxThread::run() {
    while (!isExit) {
        mux.lock();
        if (isPause) {
            mux.unlock();
            msleep(5);
            continue;
        }


        if (demux == nullptr) {
            mux.unlock();
            msleep(5);
            continue;
        }

        // 音视频同步
        pts = vt->pts;
        if (at && vt) {
            if (at->pts > 0) {
                pts = at->pts;
            }
            vt->synpts = at->pts;
        }

        AVPacket* pkt = demux->Read();
        if (pkt == nullptr) {
            mux.unlock();
            msleep(5);
            continue;
        }

        // 判断音视频
        if (demux->IsAudio(pkt)) {
            if (at != nullptr) {
                //std::cout << "audio" << std::endl;
                at->Push(pkt);
                //std::cout << "audio over" << std::endl;
            }
        } else {
            if (vt != nullptr) {
                //std::cout << "video" << std::endl;
                vt->Push(pkt);
                //std::cout << "v over" << std::endl;
            }
        }

        mux.unlock();
    }
}

void XDemuxThread::SetPause(bool isPause) {
    mux.lock(); // 存在一定阻塞
    this->isPause = isPause;
    if (at) {
        at->SetPause(isPause);
    }

    if (vt) {
        vt->SetPause(isPause);
    }
    mux.unlock();
}
