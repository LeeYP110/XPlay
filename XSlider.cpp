#include "XSlider.h"

XSlider::XSlider(QWidget *parent)
    : QSlider(parent) {
    Init();
}

XSlider::~XSlider() {
}

void XSlider::Init() {
    const QString normalColor = "#E8EDF2";             //正常颜色
    const QString grooveColor = "#1ABC9C";             //滑块颜色
    const QString handleBorderColor = "#1ABC9C";       //指示器边框颜色
    const QString handleColor = "#FFFFFF";
    int sliderHeight = 8;


    int sliderRadius = sliderHeight / 2;
    int handleWidth = (sliderHeight * 3) / 2 + (sliderHeight / 5);
    int handleRadius = handleWidth / 2;
    int handleOffset = handleRadius / 2;

    QStringList list;

    // 滑动条样式表
    list.append(QString("QSlider::horizontal{min-height:%1px;}").arg(sliderHeight * 2));
    list.append(QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:horizontal{width:%3px;margin-top:-%4px;margin-bottom:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
                .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    // 指示器样式表，偏移一个像素
    handleWidth = handleWidth + 1;
    list.append(QString("QSlider::vertical{min-width:%1px;}").arg(sliderHeight * 2));
    list.append(QString("QSlider::groove:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:vertical{height:%3px;margin-left:-%4px;margin-right:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
                .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    QString qss = list.join("");
    setStyleSheet(qss);
}

void XSlider::mousePressEvent(QMouseEvent * e) {
    double pos = (double)e->pos().x() / (double)width();
    setValue(pos * this->maximum());
    QSlider::mousePressEvent(e);
    //QSlider::sliderReleased();
}

void XSlider::mouseReleaseEvent(QMouseEvent * e) {
    QSlider::mouseReleaseEvent(e);
}
