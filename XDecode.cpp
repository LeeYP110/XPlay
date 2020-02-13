#include "XDecode.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

XDecode::XDecode()
{
}

XDecode::~XDecode()
{
}

bool XDecode::Open(AVCodecParameters * para)
{
	Close();

	if (para == nullptr)
	{
		return false;
	}

	// 找到解码器
	AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
	if (vcodec == nullptr)
	{
		avcodec_parameters_free(&para);
		std::cout << "can't find the codec id: " << para->codec_id << std::endl;
		
		return false;
	}

	// 创建解码器上下文
	mux.lock();
	codec = avcodec_alloc_context3(vcodec);

	// 配置解码器上下文参数
	avcodec_parameters_to_context(codec, para);
	avcodec_parameters_free(&para);

	codec->thread_count = 8;

	// 打开解码器上下文
	int result = avcodec_open2(codec, 0, 0);
	if (result < 0)
	{
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

bool XDecode::Send(AVPacket * pkt)
{
	if (pkt == nullptr || pkt->size < 0 || pkt->data == nullptr)
	{
		return false;
	}

	mux.lock();
	if (codec == nullptr)
	{
		mux.unlock();
		return false;
	}

	int re = avcodec_send_packet(codec, pkt);
	mux.unlock();

	av_packet_free(&pkt);
	if (re != 0)
	{
		return false;
	}
	return true;
}

AVFrame * XDecode::Recv()
{
	mux.lock();
	if (codec == nullptr)
	{
		mux.unlock();
		return nullptr;
	}

	AVFrame* frame = av_frame_alloc();
	int re = avcodec_receive_frame(codec, frame);
	mux.unlock();

	if (re != 0)
	{
		av_frame_free(&frame);
		return nullptr;
	}

	//std::cout << frame->linesize[0] << " " << std::flush;
	return frame;
}

void XDecode::Clear()
{
	mux.lock();
	// 清理解码缓冲
	if (codec != nullptr)
	{
		avcodec_flush_buffers(codec);
	}
	mux.unlock();
}

void XDecode::Close()
{
	mux.lock();
	if (codec != nullptr)
	{
		avcodec_close(codec);
		avcodec_free_context(&codec);
	}
	mux.unlock();
}

