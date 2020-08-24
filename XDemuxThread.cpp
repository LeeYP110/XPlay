#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"

// extern "C" {
// #include "libavformat/avformat.h"
// }

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
    // 清理缓冲
    Clear();

    mux.lock();
    bool status = this->isPause;
    mux.unlock();

    // 暂停
    SetPause(true);

    mux.lock();
    if (demux) {
        demux->Seek(pos);
    }

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

    SetPause(status);// seek保持seek时状态
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
                at->Push(pkt);
            }
        } else {
            if (vt != nullptr) {
                vt->Push(pkt);
            }
        }

        mux.unlock();
    }
}

void XDemuxThread::SetPause(bool isPause) {
    //mux.lock(); // 存在一定阻塞
    this->isPause = isPause;
    if (at) {
        at->SetPause(isPause);
    }

    if (vt) {
        vt->SetPause(isPause);
    }
    //mux.unlock();
}
