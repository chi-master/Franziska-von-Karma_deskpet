#pragma once
#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <MilesEdgeworth.h>

class StartupAnimationWindow : public QWidget
{
    Q_OBJECT
public:
    static void playStartupAnimation(QWidget* parent = nullptr);

private:
    explicit StartupAnimationWindow(QWidget* parent = nullptr);
    ~StartupAnimationWindow();

    void setupAnimation();
    QLabel* gifLabel;
    QMovie* startupMovie;
};