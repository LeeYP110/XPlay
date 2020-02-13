#pragma once
#include <mutex>

struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class XResample
{
public:
    XResample();
    virtual ~XResample();

    // 输出参数与输入参数一致，除了输出s16,需要释放para
    virtual bool Open(AVCodecParameters* para, bool isClear = false);
    void Close();

    // 返回重采样后大小,不管成功与否都释放indata空间
    virtual int Resample(AVFrame* indata, unsigned char* data);

protected:
    std::mutex mux;

private:
    SwrContext* actx = nullptr;

    // AVFormatSample
    int outFormat = 1;
};

