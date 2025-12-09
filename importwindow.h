#ifndef IMPORTWINDOW_H
#define IMPORTWINDOW_H

#include <QDialog>
#include "gamelibrary.h"
#include "steamscanner.h"

namespace Ui {
class ImportWindow;
}

class ImportWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ImportWindow(QWidget *parent, GameLibrary *library);
    ~ImportWindow();

protected:
    void onOkClicked();

private:
    Ui::ImportWindow *ui;
    GameLibrary* gameLib;
    steamScanner* scanner;
};

#endif // IMPORTWINDOW_H
