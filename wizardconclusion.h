#ifndef WIZARDCONCLUSION_H
#define WIZARDCONCLUSION_H

#include <QWizardPage>

namespace Ui {
class WizardConclusion;
}

class WizardConclusion : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardConclusion(QWidget *parent = nullptr);
    ~WizardConclusion();

private:
    Ui::WizardConclusion *ui;
};

#endif // WIZARDCONCLUSION_H
