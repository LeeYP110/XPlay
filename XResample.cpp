#include "XResample.h"
#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

XResample::XResample() {

}

XResample::~XResample() {
}

bool XResample::Open(AVCodecParameters * para, bool isClear) {
    if (para == nullptr) {
        return false;
    }

    // 音频重采样
    mux.lock();
    //if (actx == nullptr)
    //{
    //	actx = swr_alloc();
    //}

    // 如果actx为null会分配空间
    std::cout << "actx 1" << std::endl;
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(para->channels),	// 输出格式
                              (AVSampleFormat)outFormat,			// 输出样本格式
                              para->sample_rate,					// 采样率
                              av_get_default_channel_layout(para->channels),
                              (AVSampleFormat)para->format,
                              para->sample_rate,
                              0,
                              nullptr);
    if (isClear == true) {
        avcodec_parameters_free(&para);
    }

    std::cout << "actx 2" << std::endl;
    int result = swr_init(actx);
    mux.unlock();

    if (result < 0) {
        fprintf(stderr, "swr_init failed\n");
    }
    unsigned char* pcm = nullptr;

    return true;
}

void XResample::Close() {
    mux.lock();
    if (actx != nullptr) {
        std::cout << "actx 4" << std::endl;
        swr_free(&actx);
    }
    mux.unlock();
}

int XResample::Resample(AVFrame * indata, unsigned char * d) {
    if (indata == nullptr) {
        return 0;
    }

    if (d == nullptr) {
        av_frame_free(&indata);
        return 0;
    }

    std::cout << "actx 3" << std::endl;
    uint8_t *data[8] = { 0 };
    data[0] = d;
    int result = swr_convert(actx,
                             data,
                             indata->nb_samples,
                             (const uint8_t**)indata->data,
                             indata->nb_samples);

    if (result <= 0) {
        return result;
    }
    int outSize = result * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    av_frame_free(&indata);
    return outSize;

}
