#ifndef NEWSPAGE_H
#define NEWSPAGE_H

#include <QWidget>

namespace Ui {
class NewsPage;
}

class MainWindow;

class NewsPage : public QWidget
{
    Q_OBJECT

public:
    explicit NewsPage(QWidget *parent = nullptr);
    ~NewsPage();

private:
    Ui::NewsPage *ui;
    MainWindow *mw = nullptr;
    void onBackToMenuButtonClicked();
};

#endif // NEWSPAGE_H
