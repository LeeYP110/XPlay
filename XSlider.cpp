#include "XSlider.h"

XSlider::XSlider(QWidget *parent)
	: QSlider(parent)
{
}

XSlider::~XSlider()
{
}

void XSlider::mousePressEvent(QMouseEvent * e)
{
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos * this->maximum());
	QSlider::mousePressEvent(e);
	//QSlider::sliderReleased();
}

void XSlider::mouseReleaseEvent(QMouseEvent * e)
{
	QSlider::mouseReleaseEvent(e);
}
