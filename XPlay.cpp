#include "XPlay.h"
#include "QFileDialog"
#include "QDebug"
#include "QMessageBox"
#include "XDemuxThread.h"

static XDemuxThread dt;

XPlay::XPlay(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    //dt.Start();
    ui.playPos->hide();
    ui.isPlay->hide();

    isOpen = false;
    isSliderPress = false;
    startTimer(40);
}

XPlay::~XPlay() {
    dt.Close();
}

void XPlay::timerEvent(QTimerEvent * e) {
    if (isSliderPress || !isOpen) {
        return;
    }

    long long total = dt.totalMs;

    if (total > 0) {
        double pos = (double)dt.pts / (double)total;
        int v =ui.playPos->maximum() * pos;
        ui.playPos->setValue(v);
    }
}

void XPlay::resizeEvent(QResizeEvent * e) {
    ui.playPos->move(25, this->height() - 25);
    ui.playPos->resize(this->width() - 50, ui.playPos->height());
    ui.openFile->move(this->width() / 2 - 80, this->height() - 50);
    ui.isPlay->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());

    ui.video->resize(this->size());

}

void XPlay::mouseDoubleClickEvent(QMouseEvent * e) {
    if (isFullScreen()) {
        this->showNormal();
    } else {
        this->showFullScreen();
    }
}

void XPlay::SetPause(bool isPause) {
    if (isPause) {
        ui.isPlay->setText(QString::fromLocal8Bit("����"));
    } else {
        ui.isPlay->setText(QString::fromLocal8Bit("��ͣ"));
    }
}

void XPlay::PlayOrPause() {
    bool isPause = !dt.isPause;
    SetPause(isPause);
    dt.SetPause(isPause);
}

void XPlay::SliderPress() {
    isSliderPress = true;
}

void XPlay::SliderRelease() {
    isSliderPress = false;
    double pos = 0.0;
    pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();

    dt.Seek(pos);
}


void XPlay::OpenFile() {
    if (isOpen) {
        isOpen = false;
        dt.Close();
        ui.openFile->setText(QString::fromLocal8Bit("���ļ�"));

        ui.playPos->setValue(0);
        ui.playPos->hide();

        ui.isPlay->hide();
        ui.isPlay->setText(QString::fromLocal8Bit("��ͣ"));
        return;
    }

    // ѡ���ļ�
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));
    //name = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
    if (name.isEmpty()) {
        qDebug() << QString("no select file");
        return;
    }
    //qDebug() << name;
    this->setWindowTitle(name);
    dt.Start();
    if (!dt.Open(name.toLocal8Bit(), ui.video)) {
        QMessageBox::information(0, "error", "open file failed");
        return;
    }
    isOpen = true;
    ui.openFile->setText(QString::fromLocal8Bit("�ر���Ƶ"));
    ui.playPos->show();
    ui.isPlay->show();
    dt.SetPause(false);
    SetPause(dt.isPause);
}
