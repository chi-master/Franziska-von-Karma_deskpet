// ExitAnimationWindow.cpp
#include "ExitAnimationWindow.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <MilesEdgeworth.h>


ExitAnimationWindow::ExitAnimationWindow(QWidget* parent)
    : QWidget(parent), gifLabel(new QLabel(this)), exitMovie(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    setupAnimation();
}

ExitAnimationWindow::~ExitAnimationWindow()
{
    if (exitMovie) exitMovie->stop();
}

void ExitAnimationWindow::setupAnimation()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    gifLabel->setAlignment(Qt::AlignCenter);
    gifLabel->setScaledContents(false);
    layout->addWidget(gifLabel);

    // 使用当前对象的 isForm2 状态

// 修改副文件的这行代码：
// 修改后的副文件代码（改动最少）
    QString path = QString(":/gifs%1/special/bowbig0.gif")
        .arg(MilesEdgeworth::currentInstance->isForm2 ? 2 : 1);  // 通过静态指针访问
    exitMovie = new QMovie(path);

    connect(exitMovie, &QMovie::frameChanged, [this](int frame) {
        if (frame >= 0) {
            QImage img = exitMovie->currentImage();
            img = img.scaled(gifLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            gifLabel->setPixmap(QPixmap::fromImage(img));
        }
        });

    connect(exitMovie, &QMovie::finished, this, &QWidget::close);
}
void ExitAnimationWindow::playExitAnimation(QWidget* parent)
{
    ExitAnimationWindow* window = new ExitAnimationWindow(nullptr);

    // 原始GIF尺寸和宽高比
    const int originalWidth = 1800;
    const qreal originalHeight = 1011.6;
    const qreal aspectRatio = originalWidth / originalHeight;

    // 获取屏幕尺寸
    QScreen* screen = QApplication::primaryScreen();
    QRect screenRect = screen->geometry();

    // 计算自适应尺寸（保持宽高比）
    int maxWidth = screenRect.width() - 100;         // 横向留出边距
    int maxHeight = static_cast<int>((screenRect.height() - 45) * 0.95); // 底部留45像素空间

    int newWidth = qMin(originalWidth, maxWidth);
    int newHeight = static_cast<int>(newWidth / aspectRatio);

    // 如果高度超出限制则重新计算
    if (newHeight > maxHeight) {
        newHeight = maxHeight;
        newWidth = static_cast<int>(newHeight * aspectRatio);
    }

    window->setFixedSize(newWidth, newHeight);

    // 保持左下对齐（左边缘对齐，底部留45像素）
    int x = 0;
    int y = screenRect.height() - newHeight - 45;
    window->move(x, y);

    window->show();
    window->exitMovie->start();

    QTimer::singleShot(2500, window, &QWidget::close);

    // 事件循环保持阻塞直到动画完成
    QEventLoop loop;
    QObject::connect(window, &ExitAnimationWindow::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}