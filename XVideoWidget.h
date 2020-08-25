#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QGLShaderProgram>
#include <QTimer>
#include <mutex>
#include "IVideoCall.h"

struct AVFrame;

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall {
    Q_OBJECT

  public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();

    void Init(int width, int height);

    // ���ܳɹ�����ͷ�frame�ռ�
    virtual void Repaint(AVFrame* frame);
  protected:
    // ��ʼ��
    void initializeGL();

    // ˢ����ʾ
    void paintGL();

    // ���ڱ仯
    void resizeGL(int width, int height);

    //void paintEvent(QPaintEvent *e);

  private:
    std::mutex mux;

    // shader����
    QGLShaderProgram program;

    // shader��yuv������ַ
    GLuint unis[3] = { 0 };

    // opengl��texture��ַ
    GLuint texs[3] = { 0 };

    unsigned char* datas[3] = { 0 };

    int width = 240;
    int height = 160;
};
