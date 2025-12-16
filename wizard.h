#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include "gamelibrary.h"

// Forward declarations
class WizardIntro;
class WizardSteam;
class WizardEpic;
class WizardManual;
class WizardConclusion;

namespace Ui {
class Wizard;
}

class Wizard : public QWizard
{
    Q_OBJECT

public:
    enum PageId {
        Page_Intro = 0,
        Page_Steam,
        Page_Epic,
        Page_Manual,
        Page_Conclusion
    };

    explicit Wizard(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    ~Wizard();

private:
    Ui::Wizard *ui;
    GameLibrary *mGameLibrary;
};

#endif
