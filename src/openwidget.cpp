#include "openwidget.h"
//#include "LAppDelegate.hpp"
#include <QtGui>

#include <iostream>
#include <sstream>
#include <unistd.h>
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

OpenWidget::OpenWidget(QOpenGLWidget* parent):
    QOpenGLWidget(parent)
{
    this->setWindowFlag(Qt::WindowType::MSWindowsOwnDC,false);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::Tool);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);

    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setAttribute(Qt::WA_QuitOnClose);
    this->setMouseTracking(true);

    this->makeCurrent();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _windowWidth = this->width();
    _windowHeight = this->height();

    InitializeCubism();

    SetRootDirectory();

    LAppLive2DManager::GetInstance();

    timer = new QTimer(this);
    timer->setInterval(1000 / 48);
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

}

void OpenWidget::initializeGL()
{
    this->initializeOpenGLFunctions();
}

OpenWidget::~OpenWidget()
{
    // リソースを解放
    LAppLive2DManager::ReleaseInstance();

    //Cubism3の解放
    CubismFramework::Dispose();
}

void OpenWidget::resizeGL(int width, int height)
{
    if ((_windowWidth!=width || _windowHeight!=height) && width>0 && height>0){

    }
}

void OpenWidget::paintGL()
{

}

void OpenWidget::InitializeCubism()
{
    //setup cubism
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    //default proj
    CubismMatrix44 projection;

    LAppPal::UpdateTime();
}

GLuint OpenWidget::CreateShader()
{
    //バーテックスシェーダのコンパイル
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShader =
        "#version 120\n"
        "attribute vec3 position;"
        "attribute vec2 uv;"
        "varying vec2 vuv;"
        "void main(void){"
        "    gl_Position = vec4(position, 1.0);"
        "    vuv = uv;"
        "}";
    glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderId);

    //フラグメントシェーダのコンパイル
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShader =
        "#version 120\n"
        "varying vec2 vuv;"
        "uniform sampler2D texture;"
        "uniform vec4 baseColor;"
        "void main(void){"
        "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
        "}";
    glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderId);

    //プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // リンク
    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}

void OpenWidget::SetRootDirectory()
{
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, 1024 - 1);

    if (len != -1)
    {
        path[len] = '\0';
    }

    std::string pathString(path);

    pathString = pathString.substr(0, pathString.rfind("qmlLive2D"));
    Csm::csmVector<string> splitStrings = this->Split(pathString, '/');

    this->_rootDirectory = "";

    for(int i = 0; i < splitStrings.GetSize(); i++)
    {
        this->_rootDirectory = this->_rootDirectory + "/" +splitStrings[i];
    }

    this->_rootDirectory += "/";
}

Csm::csmVector<string> OpenWidget::Split(const std::string& baseString, char delimiter)
{
    Csm::csmVector<string> elems;
    stringstream ss(baseString);
    string item;

    while(getline(ss, item, delimiter))
    {
        if(!item.empty())
        {
            elems.PushBack(item);
        }
    }

    return elems;
}
