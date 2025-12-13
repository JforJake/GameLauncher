#include "wizard.h"
#include "ui_wizard.h"
#include <QVBoxLayout>
#include <qpushbutton.h>
#include <QFileDialog>

Wizard::Wizard(GameLibrary *gamelibrary, QWidget *parent)
    : QWizard(parent)
    , ui(new Ui::Wizard)
{
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Steam, new SteamPage(gamelibrary));
    setPage(Page_Manual, new ManualPage(gamelibrary));
    setPage(Page_Conclusion, new ConclusionPage);

    ui->setupUi(this);

    // temporary till I figure out how to add the colors
    QString styleSheet = R"(
        QWizard, QWizardPage, QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QLabel {
            color: #ffffff;
            background-color: transparent;
        }
        QPushButton {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: ;
        }

        QPushButton:hover {
            background: palette(Midlight);
        }

    )";

    setStyleSheet(styleSheet);
}

Wizard::~Wizard()
{
    delete ui;
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("page 1"));
    topLabel = new QLabel(tr("this is a filler"));
    topLabel->setWordWrap(true);
    steamRadioButton = new QRadioButton(tr("&Import your Steam Library"));
    manualRadioButton = new QRadioButton(tr("&Import a single game manually"));

    manualRadioButton->setChecked(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(steamRadioButton);
    layout->addWidget(manualRadioButton);
    setLayout(layout);
}

int IntroPage::nextId() const
{
    if (steamRadioButton->isChecked()) {
        return Wizard::Page_Steam;
    } else {
        return Wizard::Page_Manual;
    }
}


SteamPage::SteamPage(GameLibrary *gamelibrary, QWidget *parent)
    : QWizardPage(parent)
    , mGameLibrary(gamelibrary)
{
    setTitle(tr("Steam Import"));
    topLabel = new QLabel(tr("Loading Steam Imports"));
    topLabel->setWordWrap(true);
}

bool SteamPage::validatePage()
{
    scanner = new steamScanner(*mGameLibrary);

    return true;
}

int SteamPage::nextId() const
{
    return Wizard::Page_Conclusion;
}



ManualPage::ManualPage(GameLibrary *gamelibrary, QWidget *parent)
    : QWizardPage(parent)
    , mGameLibrary(gamelibrary)
{
    setTitle(tr("page 3"));
    topLabel = new QLabel(tr("this is a filler"));
    topLabel->setWordWrap(true);

    filePathEdit = new QLineEdit;
    filePathEdit->setReadOnly(true);

    QPushButton *browseButton = new QPushButton(tr("Browse..."));
    connect(browseButton, &QPushButton::clicked, this, &ManualPage::browseFile);


    QLabel *nameLabel = new QLabel(tr("Game Name:"));
    gameNameEdit = new QLineEdit;
    gameNameEdit->setPlaceholderText(tr("Enter game name"));


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(filePathEdit);
    layout->addWidget(browseButton);
    layout->addWidget(nameLabel);
    layout->addWidget(gameNameEdit);
    setLayout(layout);\


}

void ManualPage::browseFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Select Game Executable"),
        QDir::homePath(),
        tr("Executables (*.exe);;All Files (*.*)")
        );

    if (!fileName.isEmpty()) {
        filePathEdit->setText(fileName);
        selectedFilePath = fileName;
    }
}

bool ManualPage::validatePage()
{
    if (selectedFilePath.isEmpty()) {
        return false;
    }
    if (gameNameEdit->text().trimmed().isEmpty()) {
        return false;
    }

        QFileInfo fileInfo(selectedFilePath);
        const std::string gameName = gameNameEdit->text().trimmed().toStdString();
        mGameLibrary->addGame(gameName, selectedFilePath.toStdString());

    return true;
}

int ManualPage::nextId() const
{
    return Wizard::Page_Conclusion;
}



ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("page 4"));
    topLabel = new QLabel(tr("this is a filler"));
    topLabel->setWordWrap(true);
}

int ConclusionPage::nextId() const
{
    return -1; //final page
}
