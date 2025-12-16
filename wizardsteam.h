#ifndef WIZARDSTEAM_H
#define WIZARDSTEAM_H

#include <QWizardPage>
#include <QTimer>
#include "gamelibrary.h"
#include "steamscanner.h"

namespace Ui {
class WizardSteam;
}

class WizardSteam : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardSteam(GameLibrary *gameLibrary, QWidget *parent = nullptr);
    ~WizardSteam();
    bool validatePage() override;
    int nextId() const override;

private slots:
    void startScan();

private:
    Ui::WizardSteam *ui;
    GameLibrary *mGameLibrary;
};

#endif
