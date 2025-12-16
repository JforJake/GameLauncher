#ifndef WIZARDINTRO_H
#define WIZARDINTRO_H

#include <QWizardPage>

namespace Ui {
class WizardIntro;
}

class WizardIntro : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardIntro(QWidget *parent = nullptr);
    ~WizardIntro();

    int nextId() const override;

private:
    Ui::WizardIntro *ui;
};

#endif
