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
    virtual AVPacket* Pop(); // ȡ��һ֡�������û�з���null

    virtual void Clear(); // �������
    virtual void Close();

protected:
    // ������
    int maxList = 100;
    bool isExit = false;

    std::list<AVPacket*> packs;
    std::mutex mux;
    XDecode*	decode = nullptr;
};
#endif // X_DECODE_THREAD_H_

