#include "XDecodeThread.h"


XDecodeThread::XDecodeThread() {
    if (decode == nullptr) {
        decode = new XDecode();
    }
    isExit = false;
}

XDecodeThread::~XDecodeThread() {
    isExit = true;
    wait();
}



void XDecodeThread::Push(AVPacket * pkt) {
    if (pkt == nullptr) {
        return;
    }

    // ×èÈû
    while (!isExit) {
        mux.lock();
        if (packs.size() < maxList) {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(5);
    }
}

AVPacket * XDecodeThread::Pop() {
    mux.lock();
    if (packs.empty() || !decode) {
        mux.unlock();
        return nullptr;
    }

    AVPacket* pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}

void XDecodeThread::Clear() {
    mux.lock();
    decode->Clear();
    while (!packs.empty()) {
        AVPacket* pkt = packs.front();
        XFreePacket(&pkt);
        packs.pop_front();
    }
    mux.unlock();
}

void XDecodeThread::Close() {
    Clear();
    isExit = true;
    wait();

    decode->Close();

    mux.lock();
    delete decode;
    decode = nullptr;
    mux.unlock();
}
