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

    // 不管成功与否都释放frame空间
    virtual void Repaint(AVFrame* frame);
  protected:
    // 初始化
    void initializeGL();

    // 刷新显示
    void paintGL();

    // 窗口变化
    void resizeGL(int width, int height);

    //void paintEvent(QPaintEvent *e);

  private:
    std::mutex mux;

    // shader程序
    QGLShaderProgram program;

    // shader中yuv变量地址
    GLuint unis[3] = { 0 };

    // opengl中texture地址
    GLuint texs[3] = { 0 };

    unsigned char* datas[3] = { 0 };

    int width = 240;
    int height = 160;
};
