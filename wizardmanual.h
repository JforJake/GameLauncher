#ifndef WIZARDMANUAL_H
#define WIZARDMANUAL_H

#include <QWizardPage>
#include "gamelibrary.h"

namespace Ui {
class WizardManual;
}

class WizardManual : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardManual(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    ~WizardManual();
    bool validatePage() override;
    int nextId() const override;

private slots:
    void browseFile();

private:
    Ui::WizardManual *ui;
    GameLibrary *mGameLibrary;
};

#endif
