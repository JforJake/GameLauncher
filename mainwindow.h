#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QPoint>
#include <QNetworkAccessManager>
#include <QGridLayout>
#include "settings.h"
#include "gamelibrary.h"

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
    void returnToMainUI();

protected:
    void onExitButtonClicked();
    void onMinimizeButtonClicked();
    void onGameButtonClicked();
    void onPlayButtonClicked();
    void onImportButtonClicked();
    void onSettingsButtonClicked();

private:
    Ui::MainWindow *ui;
    Settings *sw;
    QString appId;
    GameLibrary* gameLibrary;
    QNetworkAccessManager* manager;
    void loadGameLibrary(QGridLayout* grid);
    QGridLayout* libgrid; // grid to display games

};
#endif
