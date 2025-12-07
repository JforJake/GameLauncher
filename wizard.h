#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QRadioButton>
#include <QLabel>
#include "gamelibrary.h"
#include "steamscanner.h"
#include <QLineEdit>

namespace Ui {
class Wizard;
}

class Wizard : public QWizard
{
    Q_OBJECT

public:
    enum {Page_Intro, Page_Steam, Page_Manual, Page_Conclusion};
    explicit Wizard(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    ~Wizard();

private:
    Ui::Wizard *ui;
    GameLibrary *mGameLibrary;
};


class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);

    int nextId() const override;

private:
    QLabel *topLabel;
    QRadioButton *steamRadioButton;
    QRadioButton *manualRadioButton;
};



class SteamPage : public QWizardPage
{
    Q_OBJECT

public:
    SteamPage(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    bool validatePage() override;
    int nextId() const override;

private:
    QLabel *topLabel;
    GameLibrary *mGameLibrary;
    steamScanner* scanner;
};



class ManualPage : public QWizardPage
{
    Q_OBJECT

public:
    ManualPage(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    bool validatePage() override;

    int nextId() const override;

private slots:
    void browseFile();

private:
    QLabel *topLabel;
    GameLibrary *mGameLibrary;
    QLineEdit *filePathEdit;
    QLineEdit *gameNameEdit;
    QString selectedFilePath;
};



class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = nullptr);

    int nextId() const override;

private:
    QLabel *topLabel;
};


#endif // WIZARD_H
