/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *ExitButton;
    QFrame *CurrentGame;
    QGraphicsView *CurrentGameLogo;
    QPushButton *OptimizeButton;
    QTextBrowser *CurrentGameInfo;
    QTabWidget *Library;
    QWidget *tab;
    QWidget *tab_3;
    QWidget *tab_4;
    QWidget *tab_2;
    QFrame *News;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QFrame *Downloads;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(480, 1081);
        MainWindow->setMaximumSize(QSize(100000, 100000));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(0, 156, 148, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        QBrush brush2(QColor(42, 42, 42, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        QBrush brush3(QColor(75, 75, 75, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush3);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush3);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush2);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush3);
        MainWindow->setPalette(palette);
        MainWindow->setMouseTracking(false);
        MainWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        ExitButton = new QPushButton(centralwidget);
        ExitButton->setObjectName("ExitButton");
        ExitButton->setGeometry(QRect(160, 1025, 160, 48));
        QPalette palette1;
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        QBrush brush4(QColor(51, 51, 51, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight, brush4);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Highlight, brush4);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Highlight, brush4);
        ExitButton->setPalette(palette1);
        CurrentGame = new QFrame(centralwidget);
        CurrentGame->setObjectName("CurrentGame");
        CurrentGame->setGeometry(QRect(30, 615, 420, 240));
        QPalette palette2;
        QBrush brush5(QColor(0, 0, 0, 0));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush5);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush5);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush5);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush5);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush5);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush5);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush5);
        CurrentGame->setPalette(palette2);
        CurrentGame->setStyleSheet(QString::fromUtf8("#CurrentGame {\n"
"    background-color: transparent;\n"
"    border: 3px solid #4B4B4B;\n"
"    border-radius: 15px;\n"
"}\n"
""));
        CurrentGame->setFrameShape(QFrame::Shape::StyledPanel);
        CurrentGame->setFrameShadow(QFrame::Shadow::Raised);
        CurrentGameLogo = new QGraphicsView(CurrentGame);
        CurrentGameLogo->setObjectName("CurrentGameLogo");
        CurrentGameLogo->setGeometry(QRect(30, 10, 120, 180));
        OptimizeButton = new QPushButton(CurrentGame);
        OptimizeButton->setObjectName("OptimizeButton");
        OptimizeButton->setGeometry(QRect(20, 200, 140, 30));
        QPalette palette3;
        QBrush brush6(QColor(31, 155, 93, 255));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight, brush6);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Highlight, brush6);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Highlight, brush6);
        OptimizeButton->setPalette(palette3);
        CurrentGameInfo = new QTextBrowser(CurrentGame);
        CurrentGameInfo->setObjectName("CurrentGameInfo");
        CurrentGameInfo->setGeometry(QRect(170, 10, 241, 221));
        CurrentGameInfo->setStyleSheet(QString::fromUtf8("background-color: transparent;\n"
"border: 2px solid #4B4B4B;\n"
"border-radius: 10px;"));
        Library = new QTabWidget(centralwidget);
        Library->setObjectName("Library");
        Library->setGeometry(QRect(30, 140, 420, 460));
        Library->setStyleSheet(QString::fromUtf8("#Library {\n"
"    background-color: transparent;\n"
"    border: 3px solid #4B4B4B;\n"
"    border-radius: 15px;\n"
"}\n"
""));
        Library->setTabShape(QTabWidget::TabShape::Triangular);
        Library->setMovable(true);
        tab = new QWidget();
        tab->setObjectName("tab");
        tab->setStyleSheet(QString::fromUtf8("#scrollArea {\n"
"    background-color: transparent;\n"
"    border: 3px solid #4B4B4B;\n"
"    border-radius: 15px;\n"
"}\n"
""));
        Library->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        Library->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName("tab_4");
        Library->addTab(tab_4, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        Library->addTab(tab_2, QString());
        News = new QFrame(centralwidget);
        News->setObjectName("News");
        News->setGeometry(QRect(30, 20, 420, 100));
        News->setStyleSheet(QString::fromUtf8("#News {\n"
"    background-color: transparent;\n"
"    border: 3px solid #4B4B4B;\n"
"    border-radius: 15px;\n"
"}\n"
""));
        News->setFrameShape(QFrame::Shape::StyledPanel);
        News->setFrameShadow(QFrame::Shadow::Raised);
        line = new QFrame(centralwidget);
        line->setObjectName("line");
        line->setGeometry(QRect(16, 0, 2, 1016));
        line->setFrameShape(QFrame::Shape::VLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);
        line_2 = new QFrame(centralwidget);
        line_2->setObjectName("line_2");
        line_2->setGeometry(QRect(464, 0, 2, 1016));
        line_2->setFrameShape(QFrame::Shape::VLine);
        line_2->setFrameShadow(QFrame::Shadow::Sunken);
        line_3 = new QFrame(centralwidget);
        line_3->setObjectName("line_3");
        line_3->setGeometry(QRect(0, 1016, 480, 2));
        line_3->setFrameShape(QFrame::Shape::HLine);
        line_3->setFrameShadow(QFrame::Shadow::Sunken);
        Downloads = new QFrame(centralwidget);
        Downloads->setObjectName("Downloads");
        Downloads->setGeometry(QRect(30, 870, 420, 140));
        Downloads->setStyleSheet(QString::fromUtf8("#Downloads {\n"
"    background-color: transparent;\n"
"    border: 3px solid #4B4B4B;\n"
"    border-radius: 15px;\n"
"}\n"
""));
        Downloads->setFrameShape(QFrame::Shape::StyledPanel);
        Downloads->setFrameShadow(QFrame::Shadow::Raised);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        Library->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "GameLauncher", nullptr));
        ExitButton->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        OptimizeButton->setText(QCoreApplication::translate("MainWindow", "Optimize Game Settings", nullptr));
        CurrentGameInfo->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:18pt; font-weight:700;\">Current Game</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">This is the description of the game, this is easily changable hopefully.</span></p></body></html>", nullptr));
        Library->setTabText(Library->indexOf(tab), QCoreApplication::translate("MainWindow", "Favorites", nullptr));
        Library->setTabText(Library->indexOf(tab_3), QCoreApplication::translate("MainWindow", "Action", nullptr));
        Library->setTabText(Library->indexOf(tab_4), QCoreApplication::translate("MainWindow", "Survival", nullptr));
        Library->setTabText(Library->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Retro", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
