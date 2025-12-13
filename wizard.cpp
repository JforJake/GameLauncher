#include "wizard.h"
#include "ui_wizard.h"
#include <QVBoxLayout>
#include <qpushbutton.h>
#include <QFileDialog>

// Wizard menu for importing the games
// Option 1 automically imports installed games
// Option 2 manually imports a game from its path and Title

Wizard::Wizard(GameLibrary *gamelibrary, QWidget *parent)
    : QWizard(parent)
    , ui(new Ui::Wizard)
{
    // creates 4 pages to cycle through
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Steam, new SteamPage(gamelibrary));
    setPage(Page_Manual, new ManualPage(gamelibrary));
    setPage(Page_Conclusion, new ConclusionPage);

    ui->setupUi(this);

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

// Page 1 to select which method of importation
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Intro"));
    topLabel = new QLabel(tr("Select the Import Method"));
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
// which page to go next
int IntroPage::nextId() const
{
    if (steamRadioButton->isChecked()) {
        return Wizard::Page_Steam; // goes to steam import method
    } else {
        return Wizard::Page_Manual; // goes to manual import method
    }
}

// Page 2 to be as a buffer to automatically import the steam library once next button is clicked
SteamPage::SteamPage(GameLibrary *gamelibrary, QWidget *parent)
    : QWizardPage(parent)
    , mGameLibrary(gamelibrary)
{
    setTitle(tr("Steam Import"));
    topLabel = new QLabel(tr("Loading Steam Imports"));
    topLabel->setWordWrap(true);
}
// imports steam games once next is clicked
bool SteamPage::validatePage()
{
    scanner = new steamScanner(*mGameLibrary);

    return true;
}

int SteamPage::nextId() const
{
    return Wizard::Page_Conclusion; // goes to the final page
}


// Page 3 for manually import a game via file dialog for its path and manually enterable name
ManualPage::ManualPage(GameLibrary *gamelibrary, QWidget *parent)
    : QWizardPage(parent)
    , mGameLibrary(gamelibrary)
{
    setTitle(tr("Manual Import"));
    topLabel = new QLabel(tr("Enter the games path and name"));
    topLabel->setWordWrap(true);

    filePathEdit = new QLineEdit;
    filePathEdit->setReadOnly(true);
    // button for file dialog
    QPushButton *browseButton = new QPushButton(tr("Browse..."));
    connect(browseButton, &QPushButton::clicked, this, &ManualPage::browseFile);

    // section for game's name
    QLabel *nameLabel = new QLabel(tr("Game Name:"));
    gameNameEdit = new QLineEdit;
    gameNameEdit->setPlaceholderText(tr("Enter game name"));


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(filePathEdit);
    layout->addWidget(browseButton);
    layout->addWidget(nameLabel);
    layout->addWidget(gameNameEdit);
    setLayout(layout);


}

// opens a file dialog and returns its path
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

// makes sure both entries are filled out before next is clicked
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
    return Wizard::Page_Conclusion; // goes to the final page
}


// Page 4 to show the user the process is finished
ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Conclusion"));
    topLabel = new QLabel(tr("You have imported the games selected"));
    topLabel->setWordWrap(true);
}

int ConclusionPage::nextId() const
{
    return -1; //final page
}
