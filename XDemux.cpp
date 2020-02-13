#include "XDemux.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

XDemux::XDemux()
{
}


XDemux::~XDemux()
{
}

bool XDemux::Open(const char * url)
{
    Close();

    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if (isFirst)
    {
        // ��ʼ����װ��
        //avcodec_register_all();

        // ��ʼ������⣨rtsp��rtmp��http��
        avformat_network_init();

        isFirst = false;
    }
    dmux.unlock();

    // ���ò���
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    av_dict_set(&opts, "max_delay", "500", 0);

    mux.lock();
    int result = avformat_open_input(
                     &ic,
                     url,
                     nullptr,// nullptr ��ʾ�Զ�ѡ����װ��
                     &opts
                 );

    if (result != 0)
    {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(result, buf, sizeof(buf) - 1);
        std::cout << buf << std::endl;
        return false;
    }

    std::cout << "open " << url << " success" << std::endl;

    // retrieve stream information
    result = avformat_find_stream_info(ic, NULL);
    if (result < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
    }

    // ��ʱ�� ����
    totalMs = ic->duration / (AV_TIME_BASE / 1000);
    std::cout << "TotalMs: " << totalMs << std::endl;
    //av_dump_format(ic, 0, ic->url, 0);

    videoIndex = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    AVStream* as = ic->streams[videoIndex];
    std::cout << "///////////////////video//////////////////" << std::endl;
    std::cout << "format:" << as->codecpar->format << std::endl;
    std::cout << "codec_id:" << as->codecpar->codec_id << std::endl;
    std::cout << "width:" << as->codecpar->width << std::endl;// width��height�п��ܲ�����
    std::cout << "height:" << as->codecpar->height << std::endl;
    std::cout << "video fps:" << r2d(as->avg_frame_rate) << std::endl;// ֡�� fps ����ת��

    width = as->codecpar->width;
    height = as->codecpar->height;

    std::cout << "///////////////////audio//////////////////" << std::endl;
    audioIndex = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    as = ic->streams[audioIndex];
    std::cout << "sample_rate:" << as->codecpar->sample_rate << std::endl;
    std::cout << "format:" << as->codecpar->format << std::endl;
    std::cout << "channels:" << as->codecpar->channels << std::endl;
    std::cout << "codec_id:" << as->codecpar->codec_id << std::endl;
    std::cout << "audio fps:" << r2d(as->avg_frame_rate) << std::endl;
    sampleRate = as->codecpar->sample_rate;
    channels = as->codecpar->channels;

    // һ֡���� ��ͨ����������
    std::cout << "audio frame_size:" << as->codecpar->frame_size << std::endl;
    // as->codecpar->frame_size * as->codecpar->channels * ������ʽռ���ֽ�

    mux.unlock();

    return true;
}

AVPacket * XDemux::Read()
{
    mux.lock();
    if (ic == nullptr)
    {
        mux.unlock();
        return nullptr;
    }

    AVPacket* pkt = av_packet_alloc();

    // ��ȡһ֡������ռ�
    int re = av_read_frame(ic, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return nullptr;
    }

    // ptsת��Ϊms
    pkt->pts = pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);
    pkt->dts = pkt->dts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);
    mux.unlock();

    //std::cout << "pkt->pts" <<  pkt->pts << std::flush << std::endl;
    return pkt;
}

void XDemux::FreeAVPacket(AVPacket ** pkt)
{
    if (*pkt == nullptr)
    {
        return;
    }
    av_packet_free(pkt);
}

bool XDemux::IsAudio(AVPacket * pkt)
{
    if (pkt == nullptr)
    {
        return false;
    }

    if (pkt->stream_index == videoIndex)
    {
        return false;
    }
    return true;
}

AVCodecParameters * XDemux::CopyVPara()
{
    mux.lock();
    if (ic == nullptr)
    {
        mux.unlock();
        return nullptr;
    }

    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoIndex]->codecpar);
    mux.unlock();
    return pa;
}

AVCodecParameters * XDemux::CopyAPara()
{
    mux.lock();
    if (ic == nullptr)
    {
        mux.unlock();
        return nullptr;
    }

    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioIndex]->codecpar);
    mux.unlock();
    return pa;
}

bool XDemux::Seek(double pos)
{
    mux.lock();
    if (ic == nullptr)
    {
        mux.unlock();
        return false;
    }

    // �����ȡ����
    avformat_flush(ic);

    long long seekPos = 0;
    //int64_t pos = (double)ms / ((double)1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    seekPos = ic->streams[videoIndex]->duration * pos; //TODO  ��Ҫ�ݴ�dur����Ϊ0
    int re = av_seek_frame(ic, videoIndex, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);

    mux.unlock();
    if (re < 0)
    {
        return false;
    }
    return true;
}

void XDemux::Clear()
{
    mux.lock();
    if (ic == nullptr)
    {
        mux.unlock();
        return;
    }

    // �����ȡ����
    avformat_flush(ic);
    mux.unlock();
}

void XDemux::Close()
{
    mux.lock();
    if (ic == nullptr)
    {
        mux.unlock();
        return;
    }

    // ��������
    avformat_close_input(&ic);
    totalMs = 0;
    mux.unlock();
}

