#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QPropertyAnimation>
#include "json.hpp"
using json = nlohmann::json;

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
    QList<QScreen*> screens;
    QScreen *currScreen;
    QRect geometry;
    QPalette dark;
    QPalette light;
    std::string settingsPath = "settings.json";
    json j;
    int screenWidth;
    int currWidthIndex;
    int screenHeight;
    int windowWidth;
    int windowHeight;
    int currScreenIndex;
    int currSide; // 0 = left    1 = right
    int currOnTop;
    int numScreens;
    int currColorScheme; // 0 = Dark mode   1 = Light mode
    int currStartupIndex;
    bool startupApp;
    void applySettings();
    void setCurrIndex();
    void setColorSchemes();

private:
    Ui::Settings *ui;
    MainWindow *mw = nullptr;
    QPropertyAnimation *m_anim = nullptr;
    void onBackButtonClicked();
    void onApplyButtonClicked();
    void moveApplication(int screenIndex, int side);
    void enableStartup();
    void disableStartup();
};

#endif // SETTINGS_H
