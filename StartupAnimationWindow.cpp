#include "StartupAnimationWindow.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>

StartupAnimationWindow::StartupAnimationWindow(QWidget* parent)
    : QWidget(parent), gifLabel(new QLabel(this)), startupMovie(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    setupAnimation();
}

StartupAnimationWindow::~StartupAnimationWindow()
{
    if (startupMovie) startupMovie->stop();
}

void StartupAnimationWindow::setupAnimation()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    gifLabel->setAlignment(Qt::AlignCenter);
    gifLabel->setScaledContents(false);
    layout->addWidget(gifLabel);

    // 根据当前形态选择开场动画路径
    QString path = QString(":/gifs%1/special/start.gif") // 假设开场动画命名为 startup.gif
        .arg(MilesEdgeworth::currentInstance->isForm2 ? 2 : 1);
    startupMovie = new QMovie(path);

    connect(startupMovie, &QMovie::frameChanged, [this](int frame) {
        if (frame >= 0) {
            QImage img = startupMovie->currentImage();
            img = img.scaled(gifLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            gifLabel->setPixmap(QPixmap::fromImage(img));
        }
        });

    connect(startupMovie, &QMovie::finished, this, &QWidget::close);
}

void StartupAnimationWindow::playStartupAnimation(QWidget* parent)
{
    StartupAnimationWindow* window = new StartupAnimationWindow(nullptr);

    // 原始GIF尺寸和宽高比
    const int originalWidth = 1800;
    const qreal originalHeight = 1011.6;
    const qreal aspectRatio = originalWidth / originalHeight;

    // 获取屏幕尺寸
    QScreen* screen = QApplication::primaryScreen();
    QRect screenRect = screen->geometry();

    // 计算自适应尺寸（保持宽高比）
    int maxWidth = screenRect.width() - 100;  // 留出边距
    int maxHeight = static_cast<int>((screenRect.height() - 45) * 0.95); // 留出底部空间

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
    window->startupMovie->start();

    QTimer::singleShot(2000, window, &QWidget::close);

    QEventLoop loop;
    QObject::connect(window, &StartupAnimationWindow::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}