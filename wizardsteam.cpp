#include "WIZARDSTEAM.h"
#include "ui_wizardsteam.h"
#include "WIZARD.h"

WizardSteam::WizardSteam(GameLibrary *gameLibrary, QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::WizardSteam)
    , mGameLibrary(gameLibrary)
{
    ui->setupUi(this);

    // Ensure progress bar is in busy mode
    ui->StatusLabel->setText(tr("Waiting to start import..."));
}

WizardSteam::~WizardSteam()
{
    delete ui;
}

bool WizardSteam::validatePage()
{
    // This is called when "Next" is clicked
    ui->StatusLabel->setText(tr("Scanning Steam libraries..."));
    startScan();
    return true; // allow wizard to proceed
}

void WizardSteam::startScan()
{
    // Blocking scan (same behavior you already had)
    steamScanner scanner(*mGameLibrary);

    ui->StatusLabel->setText(tr("Steam import complete"));
}

int WizardSteam::nextId() const
{
    return Wizard::Page_Conclusion;
}
