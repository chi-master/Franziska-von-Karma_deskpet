#include "MilesEdgeworth.h"
#include <QtWidgets/QApplication>
#include "ExitAnimationWindow.h"
#include "StartupAnimationWindow.h" // ✅ 只包含头文件

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);         // MilesEdgewort设置为Qt::Tool时, 如果不加这行会导致关闭图片查看器时自动退出程序
    MilesEdgeworth w;
    //StartupAnimationWindow::playStartupAnimation();
    QObject::connect(&w, &MilesEdgeworth::exitProgram, &a, &QApplication::quit);    // 单击退出按钮时退出程序
    w.show();
    return a.exec();
}