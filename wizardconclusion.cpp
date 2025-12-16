#include "WIZARDCONCLUSION.h"
#include "ui_wizardconclusion.h"

WizardConclusion::WizardConclusion(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::WizardConclusion)
{
    ui->setupUi(this);
}

WizardConclusion::~WizardConclusion()
{
    delete ui;
}
