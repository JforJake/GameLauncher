#include "WIZARDINTRO.h"
#include "ui_wizardintro.h"
#include "WIZARD.h"

WizardIntro::WizardIntro(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::WizardIntro)
{
    ui->setupUi(this);

    // Safety-first default
    ui->ManualRadio->setChecked(true);
}

WizardIntro::~WizardIntro()
{
    delete ui;
}

int WizardIntro::nextId() const
{
    if (ui->SteamRadio->isChecked())
        return Wizard::Page_Steam;

    if (ui->EpicRadio->isChecked())
        return Wizard::Page_Epic;

    return Wizard::Page_Manual;
}
