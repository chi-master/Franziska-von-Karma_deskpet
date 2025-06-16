#pragma execution_character_set("utf-8")
#pragma once
#include <QtWidgets/QWidget>
#include "ui_MilesEdgeworth.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMovie>
#include <QMenu>
#include <QActionGroup>
#include <QSoundEffect>
#include <QTimer>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QThread>
#include <QSystemTrayIcon>
#include "PicViewer.h"
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QString>
#include <windows.h>
#include <QtGlobal>  // 这个头文件定义了qintptr类型
#include <QSettings>
#include <psapi.h>
#include <uiautomation.h>
#include "ExitAnimationWindow.h"
#include <comutil.h>
#include <iostream>
#include <comdef.h>
#pragma comment(lib, "uiautomationcore.lib")
#define WIN32_LEAN_AND_MEAN

const int ONCE_NUM = 8;
const int WALK_NUM = 8;
const int RUN_NUM = 8;
const double MINI = 1.5;
const double SMALL = 3;
const double MEDIAN = 4;
const double BIG = 6.5;
const int MAXVIEWER = 30;


class MilesEdgeworth : public QWidget
{
    Q_OBJECT

public:

    enum Type {
        RUN = 0,
        WALK,
        STAND,
        ONCE,
        TAKETHAT,
        TAKETHATING,
        BRIEFCASEIN,
        BRIEFCASESTOP,
        CROUCH,
        SLEEP,
        SLEEPING
    };

    explicit MilesEdgeworth(QWidget* parent = nullptr);
    ~MilesEdgeworth();
    virtual void moveEvent(QMoveEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void enterEvent(QEnterEvent* event) override;
    bool isForm2 = QSettings().value("PetForm", false).toBool(); // 初始化时读取存储值
    void createContextMenu();
    void createTrayIcon();
    void createClickTimer();

    void createShakeTimer();
    void createPicViewer();
    void showContextMenu();
    // 自动随机切换Gif
    void autoChangeGif()
    {
        Type next_type = MilesEdgeworth::STAND; // 下一动作, 默认为STAND
        int next_direct = 0;    // 下一方向, 默认为向右
        switch (type) {
        case MilesEdgeworth::RUN:
        {

            double randnum = QRandomGenerator::global()->generateDouble();
            if (randnum <= 0.2) { // 0.2概率接同向走路
                next_type = MilesEdgeworth::WALK;
                next_direct = direction;
                petMovie->setFileName(QString(":/gifs%1/walk/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            else if (randnum < 0.4) {  // 0.2概率接随机跑步
                next_type = MilesEdgeworth::RUN;
                next_direct = QRandomGenerator::global()->bounded(RUN_NUM);
                petMovie->setFileName(QString(":/gifs%1/run/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            else {  // 0.6概率接站立
                next_type = MilesEdgeworth::STAND;
                next_direct = direction % 2;
                petMovie->setFileName(QString(":/gifs%1/stand/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            break;
        }
        case MilesEdgeworth::WALK:
        {
            double randnum = QRandomGenerator::global()->generateDouble();
            if (randnum <= 0.2) { // 0.2概率接同向跑步
                next_type = MilesEdgeworth::RUN;
                next_direct = direction;
                petMovie->setFileName(QString(":/gifs%1/run/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            else if (randnum < 0.5) {  // 0.3概率接随机走路
                next_type = MilesEdgeworth::WALK;
                next_direct = QRandomGenerator::global()->bounded(WALK_NUM);
                petMovie->setFileName(QString(":/gifs%1/walk/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            else {  // 0.5概率接站立
                next_type = MilesEdgeworth::STAND;
                next_direct = direction % 2;
                petMovie->setFileName(QString(":/gifs%1/stand/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            break;
        }
        case MilesEdgeworth::ONCE:
        {
            petMovie->setSpeed(110);
            // 若当前动作为单次动作, 则停止后立刻切换为站立, 方向保持不变
            next_type = MilesEdgeworth::STAND;
            next_direct = direction % 2;
            petMovie->setFileName(QString(":/gifs%1/stand/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            break;
        }
        case MilesEdgeworth::STAND:
        {
            petMovie->setSpeed(110);
            double randnum = QRandomGenerator::global()->generateDouble();
            if (randnum < 0.08) {
                // 0.08的概率直接切换为反向站立
                next_type = MilesEdgeworth::STAND;
                next_direct = 1 - direction;
                petMovie->setFileName(QString(":/gifs%1/stand/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            else if (randnum < 0.012) {
                // 0.12的概率切换转身动画
                petMovie->stop();
                specifyChangeGif(QString(":/gifs%1/stand/%2.gif").arg(isForm2 ? 2 : 1).arg(direction),
                    MilesEdgeworth::ONCE, 1 - direction);
            }
            else if (randnum < 0.8) {  //0.8
                // 0.6的概率切换为单次动作, 方向不变
                next_type = MilesEdgeworth::ONCE;
                next_direct = direction;
                int num = QRandomGenerator::global()->bounded(ONCE_NUM);
                num = (direction + 2 * num) % ONCE_NUM;
                petMovie->setFileName(QString(":/gifs%1/once/%2.gif").arg(isForm2 ? 2 : 1).arg(num));
            }
            else if (randnum < 0.9) {  //0.9
                // 0.1的概率切换为走路
                next_type = MilesEdgeworth::WALK;
                next_direct = QRandomGenerator::global()->bounded(WALK_NUM);
                petMovie->setFileName(QString(":/gifs%1/walk/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            else {
                // 0.1的概率切换为跑步
                next_type = MilesEdgeworth::RUN;
                next_direct = QRandomGenerator::global()->bounded(RUN_NUM);
                petMovie->setFileName(QString(":/gifs%1/run/%2.gif").arg(isForm2 ? 2 : 1).arg(next_direct));
            }
            break;
        }
        }
        type = next_type;
        direction = next_direct;
        petMovie->start();

    }
    // 切换为指定的Gif. 参数为: 下一动作文件名, 下一动作类型, 下一动作方向
    void specifyChangeGif(const QString& filename, Type next_type, int next_direct);

    void runMove();
    void walkMove();
    void doubleClickEvent();
    void singleClickEvent();
    void setObjectionMode(bool enabled);    // 设置异议模式
    // 语言设置函数
    void saveLanguageSetting(int lang);
    void loadLanguageSetting();
    QAction* actionChange;  // 切换形态
    void saveFormSetting(bool form);  // 保存形态设置
    void loadFormSetting();          // 加载形态设置

    static MilesEdgeworth* currentInstance;  // 新增静态指针
signals:
    void soundPlay();
    void soundStop();
    void exitProgram();

public slots:
    void on_viewerClosing(bool prevIsNull, PicViewer* nextPtr);
    void on_trayActivated(QSystemTrayIcon::ActivationReason reason);
    // 切换异议模式的槽函数
    void toggleObjectionMode(bool enabled);
    // 处理全局双击事件的槽函数
    void handleGlobalDoubleClick(const QPoint& globalPos);
private slots:


protected:
    //异议模式重写鼠标双击
        // 重写鼠标双击事件处理函数
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    // 重写原生事件处理函数，用于处理底层Windows消息
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    Ui::MilesEdgeworthClass ui;
    QMovie* petMovie;
    QTimer* clickTimer;         // 用于判断单击或双击
    QTimer* badgeTimer;         // 用于让检察官徽章消失
    QTimer* shakeTimer;         // 用于判断是否触发害怕地震动作
    QSoundEffect* soundEffect;  // 用于播放语音
    bool m_dragging;	        // 是否正在拖动
    QPoint m_startPosition;     // 拖动开始前的鼠标位置, 是全局坐标
    QPoint m_framePosition;	    // 窗体的原始位置
    QPointF m_pressPos;         // 鼠标按下时的位置, 是widget的坐标 
    Type type;                  // 类型: run,walk,stand,once
    int direction;              // 方向: 左为1,右为0. 在走/跑时更细分为8个方向
    double scale = MEDIAN;         // 缩放倍数
    int language;           // 0为日语,1为英语,2为汉语
    bool doubleClick = false;
    int shakeX = 0;                 // 用于判断是否触发害怕地震动作
    int shakeDirect = 1;            // 用于判断是否触发害怕地震动作
    int shakeCount = 0;             // 用于判断是否触发害怕地震动作, 计数move更换方向的次数

    QScreen* nowScreen = nullptr;

    QMenu* menu;
    QMenu* sizeMenu;     // 设置大小
    QMenu* languageMenu; // 设置语音语言
    QMenu* screenMenu;   // 双屏的有关设置
    QAction* actionTop;  // 是否置于顶层
    QAction* actionMove; // 是否禁止自动移动
    QAction* actionMute; // 是否静音
    QAction* actionPic;  // 显示参考图
    QAction* actionExit; // 退出
    QAction* actionTie;  // 束缚
    QAction* actionTurn;  // 转向
    QAction* actionWhip;  // 抽鞭子


    // 子菜单的action
    QActionGroup* sizeGroup;
    QAction* miniSize;
    QAction* smallSize;
    QAction* medianSize;
    QAction* bigSize;
    QActionGroup* lanGroup;
    QAction* chLanguage;
    QAction* engLanguage;
    QAction* jpLanguage;
    QActionGroup* screenGroup;
    QAction* leftPrimary;
    QAction* rightPrimary;
    QAction* singleScreen;

    QThread* thread;

    // 参考图查看器多窗口使用链表结构
    PicViewer* firstViewer = nullptr;
    int numViewer = 0;

    // 系统托盘
    QSystemTrayIcon* tray;

    //设置大小
    void applyScale(double scaleValue, QAction* action);
    void saveSettings();
    void loadSettings();
    // 在头文件中添加固定位置的方法
    void setToDefaultPosition() {
        // 获取当前屏幕
        nowScreen = QGuiApplication::primaryScreen();
        QRect desktopRect = nowScreen->availableGeometry();

        // 计算左下角位置（根据当前的缩放比例）
        // 左下角位置：x为左边界加一点偏移，y为底部减去桌宠高度
        int x = desktopRect.x() + 20; // 距离左边界20像素
        int y = desktopRect.y() + desktopRect.height() - 100 * scale - 20; // 距离底部20像素

        // 移动桌宠到计算出的位置
        move(x, y);
    }

    bool objectionModeEnabled;  // 异议模式开关
    QAction* actionObjection; // 异议模式菜单项
    // 注册原始输入设备
    bool RegisterRawInput();

    // 处理原始输入数据
    void ProcessRawInput(HRAWINPUT hRawInput);



    // 在类定义中添加（需包含头文件 #include <QSettings>）


    bool isClosing = false;

    void startExitSequence() {
        isClosing = true;
        this->hide();
        ExitAnimationWindow::playExitAnimation(this);
        QTimer::singleShot(1, this, &MilesEdgeworth::delayedClose);
    }

    void delayedClose() {
        this->close();
    }

    QImage sharpenImage(const QImage& original) {
        if (original.isNull()) return QImage();

        // 保持ARGB32格式处理透明度
        QImage sharpened = original.convertToFormat(QImage::Format_ARGB32);
        const int width = sharpened.width();
        const int height = sharpened.height();

        // 微调后的卷积核（总强度提升约20%）
        const float kernel[3][3] = {
            { 0.0f,  -0.25f, 0.0f  },  // 边缘权重从-0.2→-0.25
            { -0.25f, 2.0f,  -0.25f },  // 中心权重从1.8→2.0
            { 0.0f,  -0.25f, 0.0f  }
        };

        QImage buffer = sharpened.copy();

        for (int y = 0; y < height; ++y) {
            const QRgb* srcLine = reinterpret_cast<const QRgb*>(buffer.constScanLine(y));
            QRgb* dstLine = reinterpret_cast<QRgb*>(sharpened.scanLine(y));

            for (int x = 0; x < width; ++x) {
                const QRgb srcPixel = srcLine[x];
                const int origA = qAlpha(srcPixel);

                // 透明像素快速通道
                if (origA == 0) {
                    dstLine[x] = srcPixel;
                    continue;
                }

                float r = 0.0f, g = 0.0f, b = 0.0f;
                const int origR = qRed(srcPixel);
                const int origG = qGreen(srcPixel);
                const int origB = qBlue(srcPixel);

                // 卷积计算
                for (int ky = -1; ky <= 1; ++ky) {
                    const int sy = qBound(0, y + ky, height - 1);
                    const QRgb* line = reinterpret_cast<const QRgb*>(buffer.constScanLine(sy));

                    for (int kx = -1; kx <= 1; ++kx) {
                        const int sx = qBound(0, x + kx, width - 1);
                        const QRgb pixel = line[sx];
                        const float weight = kernel[ky + 1][kx + 1];
                        const float alphaFactor = qAlpha(pixel) / 255.0f;

                        r += qRed(pixel) * weight * alphaFactor;
                        g += qGreen(pixel) * weight * alphaFactor;
                        b += qBlue(pixel) * weight * alphaFactor;
                    }
                }

                // 调整混合比例（55%原始 + 45%锐化）
                int finalR = qBound(0, static_cast<int>(origR * 0.30f + r * 0.70f), 255);
                int finalG = qBound(0, static_cast<int>(origG * 0.30f + g * 0.70f), 255);
                int finalB = qBound(0, static_cast<int>(origB * 0.30f + b * 0.70f), 255);

                // 边缘保护（防止过冲）
                if (qAbs(finalR - origR) > 40) finalR = origR;
                if (qAbs(finalG - origG) > 40) finalG = origG;
                if (qAbs(finalB - origB) > 40) finalB = origB;

                dstLine[x] = qRgba(finalR, finalG, finalB, origA);
            }
        }
        return sharpened;
    }
    // 添加这两个私有成员
    QPointer<QSoundEffect> activeWhipSound;  // 智能指针管理音效对象
    void playComboSound();                   // 音效组合播放函数
    int audioPlayCount = 0; // 新增：音频播放计数器

};