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
std::string path = "../../bin/win64/Onion.mp4";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XPlay w;
    w.show();

    XDemuxThread dt;
    dt.Open(path.c_str(), w.ui.video);
    dt.Start();

    return a.exec();
}
