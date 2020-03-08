#include "XPlay.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"

#include "XAudioThread.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"

std::string url = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";

#define V_THREAD 1
#define A_THREAD 1

#if 0
class TestThread : public QThread
{
public:
    XVideoThread vt;
    XAudioThread at;
    void Init()
    {
        demux.Open(url.c_str());

#if V_THREAD
        vt.Open(demux.CopyVPara(), video, demux.width, demux.height);
        vt.start();
#else
        vd.Open(demux.CopyVPara());
#endif // V_THREAD

#if A_THREAD
        at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels);
        at.start();
#else
        ad.Open(demux.CopyAPara());
        res.Open(demux.CopyAPara(), true);

        XAudioPlay::Get()->sampleRate = demux.sampleRate;
        XAudioPlay::Get()->channels = demux.channels;
        XAudioPlay::Get()->Open();
#endif
    }

    void run()
    {
        unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
        while (true)
        {
            auto pkt = demux.Read();
            if (demux.IsAudio(pkt))
            {
#if A_THREAD
                at.Push(pkt);
#else
                ad.Send(pkt);
                auto frame = ad.Recv();
                auto size = res.Resample(frame, pcm);

                while (size > 0)
                {
                    if (XAudioPlay::Get()->GetFree() >= size)
                    {
                        XAudioPlay::Get()->WriteData((char*)pcm, size);
                        break;
                    }
                    msleep(1);
                }
#endif
            }
            else
            {
#if V_THREAD
                vt.Push(pkt);
#else
                vd.Send(pkt);
                auto frame = vd.Recv();
                video->Repaint(frame);
#endif
            }
        }
    }

public:
    XDemux demux;
    XDecode vd;
    XDecode ad;
    XResample res;
    //XAudioPlay* ap;
    XVideoWidget* video;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XPlay w;
    w.show();

    url = "../../bin/win64/test.mp4";
    TestThread tt;
    tt.video = w.ui.video;

    tt.Init();
    w.ui.video->Init(tt.demux.width, tt.demux.height);

    tt.start();

    return a.exec();
}
#else
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XPlay w;
    w.show();

    //XDemuxThread dt;
    //url = "../../bin/win64/Onion.mp4";
    //dt.Open(url.c_str(), w.ui.video);
    //dt.Start();

    return a.exec();
}
#endif // 1