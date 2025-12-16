#include "WIZARDMANUAL.h"
#include "ui_wizardmanual.h"
#include "WIZARD.h"
#include "steamfetcher.h"
#include <QFileDialog>
#include <QDir>

WizardManual::WizardManual(GameLibrary *gameLibrary, QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::WizardManual)
    , mGameLibrary(gameLibrary)
{
    ui->setupUi(this);

    connect(ui->BrowseButton, &QPushButton::clicked,
            this, &WizardManual::browseFile);
}

WizardManual::~WizardManual()
{
    delete ui;
}

void WizardManual::browseFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Select Game Executable"),
        QDir::homePath(),
        tr("Executables (*.exe);;All Files (*.*)")
        );

    if (!fileName.isEmpty()) {
        ui->FilePathEdit->setText(fileName);
    }
}

bool WizardManual::validatePage()
{
    const QString path = ui->FilePathEdit->text();
    const QString name = ui->GameNameEdit->text().trimmed();

    if (path.isEmpty() || name.isEmpty())
        return false;

    // 🔹 Fetch metadata via SteamFetcher
    SteamFetcher fetcher;
    GameMetadata meta =
        fetcher.fetchGameData(name.toStdString());

    mGameLibrary->addGame(
        name.toStdString(),
        path.toStdString(),
        meta.description,
        meta.imagePath
        );

    return true;
}

int WizardManual::nextId() const
{
    return Wizard::Page_Conclusion;
}
