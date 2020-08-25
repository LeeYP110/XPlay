#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay.h"

class XPlay : public QWidget {
    Q_OBJECT

  public:
    XPlay(QWidget *parent = Q_NULLPTR);
    ~XPlay();

    // 定时器, 滑动条显示
    void timerEvent(QTimerEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void SetPause(bool isPause);

  public slots:
    void OpenFile();
    void PlayOrPause();


    void SliderPress();
    void SliderRelease();

//public:
  private:
    bool isSliderPress;
    bool isOpen;
    Ui::XPlayClass ui;
};
