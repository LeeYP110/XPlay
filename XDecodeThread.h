#ifndef X_DECODE_THREAD_H_
#define X_DECODE_THREAD_H_
#include <QThread>
#include <list>
#include "XDecode.h"
#include "IVideoCall.h"

struct AVCodecParameters;
struct AVPacket;
class XDecode;

class XDecodeThread : public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();

    virtual void Push(AVPacket* pkt);
    virtual AVPacket* Pop(); // 取出一帧数据如果没有返回null

    virtual void Clear(); // 清理队列
    virtual void Close();

protected:
    // 最大队列
    int maxList = 100;
    bool isExit = false;

    std::list<AVPacket*> packs;
    std::mutex mux;
    XDecode*	decode = nullptr;
};
#endif // X_DECODE_THREAD_H_

