#include "settingswindow.h"
#include "ui_settingswindow.h"

settingsWindow::settingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settingsWindow)
{
    ui->setupUi(this);
}

settingsWindow::~settingsWindow()
{
    delete ui;
}

void settingsWindow::on_pushButton_clicked()
{
    emit notesFontSize(ui->editNotesFontSize->text().toInt());
    emit noteFontSize(ui->editNoteFontSize->text().toInt());
}
