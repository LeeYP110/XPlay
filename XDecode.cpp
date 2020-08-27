#include "XDecode.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}


void XFreePacket(AVPacket ** pkt) {
    if (pkt == nullptr || *pkt == nullptr) {
        return;
    }
    av_packet_free(pkt);
    *pkt = nullptr;
}

void XFreeFrame(AVFrame ** frame) {
    if (frame == nullptr || *frame == nullptr) {
        return;
    }
    av_frame_free(frame);
    *frame = nullptr;
}

XDecode::XDecode() {
}

XDecode::~XDecode() {
}

bool XDecode::Open(AVCodecParameters * para) {
    Close();

    if (para == nullptr) {
        return false;
    }

    // �ҵ�������
    AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
    if (vcodec == nullptr) {
        avcodec_parameters_free(&para);
        std::cout << "can't find the codec id: " << para->codec_id << std::endl;

        return false;
    }

    // ����������������
    mux.lock();
    codec = avcodec_alloc_context3(vcodec);

    // ���ý����������Ĳ���
    avcodec_parameters_to_context(codec, para);
    avcodec_parameters_free(&para);

    codec->thread_count = 8;

    // �򿪽�����������
    int result = avcodec_open2(codec, 0, 0);
    if (result < 0) {
        avcodec_free_context(&codec);
        mux.unlock();
        fprintf(stderr, "avcodec_open2 failed\n");
        return false;
    }
    std::cout << "avcodec_open2 success" << std::endl;

    mux.unlock();
    //avcodec_parameters_free(&para);
    return true;
}

bool XDecode::Send(AVPacket * pkt) {
    if (pkt == nullptr || pkt->size < 0 || pkt->data == nullptr) {
        return false;
    }

    mux.lock();
    if (codec == nullptr) {
        mux.unlock();
        return false;
    }

    int re = avcodec_send_packet(codec, pkt);
    mux.unlock();

    av_packet_free(&pkt);
    if (re != 0) {
        return false;
    }
    return true;
}

AVFrame * XDecode::Recv() {
    mux.lock();
    if (codec == nullptr) {
        mux.unlock();
        return nullptr;
    }

    AVFrame* frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    mux.unlock();

    if (re != 0) {
        //char buf[1024] = { 0 };
        //av_strerror(re, buf, sizeof(buf) - 1);
        //std::cout << buf << std::endl;
        av_frame_free(&frame);
        return nullptr;
    }

    //std::cout << frame->linesize[0] << " " << std::flush;

    pts = frame->pts;
    key_frame = frame->key_frame;
    return frame;
}

void XDecode::Clear() {
    mux.lock();
    // ������뻺��
    if (codec != nullptr) {
        avcodec_flush_buffers(codec);
    }
    mux.unlock();
}

void XDecode::Close() {
    mux.lock();
    if (codec != nullptr) {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    pts = 0;
    mux.unlock();
}

