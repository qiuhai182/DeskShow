#ifndef OPENWIDGET_H
#define OPENWIDGET_H

#include <QtWidgets>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <string>
#include <QTimer>
#include "Type/csmVector.hpp"
#include "LAppAllocator.hpp"

class LAppTextureManager;

class OpenWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    OpenWidget(QOpenGLWidget* parent = 0);
    void update();
    std::string GetRootDirectory(){ return _rootDirectory;}
    void SetRootDirectory();
    Csm::csmVector<std::string> Split(const std::string& baseString, char delim);
    GLuint CreateShader();
    void InitializeCubism();
    ~OpenWidget();
protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;


private:
    QTimer* timer;
    std::string _rootDirectory;                  ///
    LAppAllocator _cubismAllocator;              ///< Cubism3 Allocator
    Csm::CubismFramework::Option _cubismOption;  ///< Cubism3 Option
    LAppTextureManager* _textureManager;
    bool _captured;                              ///< クリックしているか
    float _mouseX;                               ///< マウスX座標
    float _mouseY;                               ///< マウスY座標
    int _windowWidth;                            ///< Initialize関数で設定したウィンドウ幅
    int _windowHeight;                           ///< Initialize関数で設定したウィンドウ高さ
};

#endif // OPENWIDGET_H
