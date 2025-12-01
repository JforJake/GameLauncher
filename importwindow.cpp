#include "importwindow.h"
#include "ui_importwindow.h"

ImportWindow::ImportWindow(QWidget *parent, GameLibrary* library)
    : QDialog(parent)
    , ui(new Ui::ImportWindow),
    gameLib(library)

{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ImportWindow::onOkClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ImportWindow::~ImportWindow()
{
    delete ui;
}

// adds games to the database depending on the option clicked.
void ImportWindow::onOkClicked() {
    if (ui->steamImportButton->isChecked()) {
        qDebug() << "Scanning Steam Library";
        scanner = new steamScanner(*gameLib);

    } else if (ui->manualImportButton->isChecked()) {
        qDebug() << "Manual Import selected";
        // Do manual import stuff here
    }
}

