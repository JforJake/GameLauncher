#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QPropertyAnimation>

namespace Ui {
class Settings;
}

class MainWindow;

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    QScreen *currScreen;
    QRect geometry;
    QPalette dark;
    QPalette light;
    int screenWidth;
    int screenHeight;
    int windowWidth;
    int windowHeight;
    int currSide; // 0 = left    1 = right
    int currOnTop;
    int numScreens;
    void setCurrIndex();

private:
    Ui::Settings *ui;
    MainWindow *mw = nullptr;
    QList<QScreen*> screens;
    QPropertyAnimation *m_anim = nullptr;
    void setColorSchemes();
    void onBackButtonClicked();
    void onApplyButtonClicked();
    void applySettings();
    void moveApplication(int screenIndex, int side);
};

#endif // SETTINGS_H
