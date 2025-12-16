#include "WIZARDEPIC.h"
#include "ui_wizardepic.h"
#include "WIZARD.h"
#include "epicscanner.h"

WizardEpic::WizardEpic(GameLibrary *gameLibrary, QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::WizardEpic)
    , mGameLibrary(gameLibrary)
{
    ui->setupUi(this);
    ui->StatusLabel->setText(tr("Ready to scan Epic Games library"));
}

WizardEpic::~WizardEpic()
{
    delete ui;
}

bool WizardEpic::validatePage()
{
    ui->StatusLabel->setText(tr("Scanning installed Epic games…"));

    EpicScanner scanner(*mGameLibrary);

    ui->StatusLabel->setText(tr("Epic Games import complete"));
    return true;
}

int WizardEpic::nextId() const
{
    return Wizard::Page_Conclusion;
}
