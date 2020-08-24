#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QSlider>

class XSlider : public QSlider {
    Q_OBJECT

  public:
    XSlider(QWidget *parent = nullptr);
    ~XSlider();

    void Init();
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent *e);

};
