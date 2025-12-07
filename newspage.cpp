#include "newspage.h"
#include "ui_newspage.h"
#include "mainwindow.h"
#include <QPushButton>

NewsPage::NewsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewsPage)
{
    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(parent);

    connect(ui->BackToMenuButton, &QPushButton::clicked, this, &NewsPage::onBackToMenuButtonClicked);
}

void NewsPage::onBackToMenuButtonClicked() {
    if (mw) {
        this->hide();
        mw->returnToMainUI();
    }
}

NewsPage::~NewsPage()
{
    delete ui;
}
