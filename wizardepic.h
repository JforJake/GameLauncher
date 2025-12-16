#ifndef WIZARDEPIC_H
#define WIZARDEPIC_H

#include <QWizardPage>
#include "gamelibrary.h"

namespace Ui {
class WizardEpic;
}

class WizardEpic : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardEpic(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    ~WizardEpic();
    bool validatePage() override;
    int nextId() const override;

private:
    Ui::WizardEpic *ui;
    GameLibrary *mGameLibrary;
};

#endif
