#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QPoint>
#include <QPropertyAnimation>
#include "steamscanner.h"
#include <QNetworkAccessManager>
#include <QGridLayout>
#include "importwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setWindow();

protected:
    void onExitButtonClicked();
    void onMinimizeButtonClicked();
    void onGameButtonClicked();
    void onPlayButtonClicked();
    void onImportButtonClicked();
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    Ui::MainWindow *ui;
    QPoint m_dragPos;
    void snapToEdge();
    bool m_dragging = false;
    QString appId;
    QPropertyAnimation *m_anim = nullptr;
    QScreen *screen;
    QRect geometry;
    int screenWidth;
    int screenHeight;
    int windowWidth;
    int windowHeight;
    int screenSide; // 0-3 in clockwise order starting from top
    GameLibrary* gameLibrary;
    QNetworkAccessManager* manager;
    void loadGameLibrary(QGridLayout* grid);
    QGridLayout* libgrid; // grid to display games

};
#endif
