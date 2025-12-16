#include "wizard.h"
#include "ui_wizard.h"

#include "wizardintro.h"
#include "wizardsteam.h"
#include "wizardepic.h"
#include "wizardmanual.h"
#include "wizardconclusion.h"

Wizard::Wizard(GameLibrary *gameLibrary, QWidget *parent)
    : QWizard(parent)
    , ui(new Ui::Wizard)
    , mGameLibrary(gameLibrary)
{
    ui->setupUi(this);

    // Wizard behavior
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::NoCancelButton, false);

    // Register pages
    setPage(Page_Intro, new WizardIntro(this));
    setPage(Page_Steam, new WizardSteam(mGameLibrary, this));
    setPage(Page_Epic, new WizardEpic(mGameLibrary, this));
    setPage(Page_Manual, new WizardManual(mGameLibrary, this));
    setPage(Page_Conclusion, new WizardConclusion(this));

    // Start page
    setStartId(Page_Intro);

    // Optional sizing
    resize(500, 350);
}

Wizard::~Wizard()
{
    delete ui;
}
