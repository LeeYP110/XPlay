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

    // ����������������һ�£��������s16,��Ҫ�ͷ�para
    virtual bool Open(AVCodecParameters* para, bool isClear = false);
    void Close();

    // �����ز������С,���ܳɹ�����ͷ�indata�ռ�
    virtual int Resample(AVFrame* indata, unsigned char* data);

protected:
    std::mutex mux;

private:
    SwrContext* actx = nullptr;

    // AVFormatSample
    int outFormat = 1;
};

