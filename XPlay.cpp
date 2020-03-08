#include "XPlay.h"
#include "QFileDialog"
#include "QDebug"
#include "QMessageBox"
#include "XDemuxThread.h"

static XDemuxThread dt;

XPlay::XPlay(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    dt.Start();
}

XPlay::~XPlay()
{
    dt.Close();
}

void XPlay::OpenFile()
{
    // 选择文件
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
    if (name.isEmpty())
    {
        qDebug() << QString("no select file");
        return;
    }
    //qDebug() << name;
    this->setWindowTitle(name);
    if (!dt.Open(name.toLocal8Bit(), ui.video))
    {
        QMessageBox::information(0, "error", "open file failed");
        return;
    }
}
