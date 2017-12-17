#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isModified = change = false;
    ui->listWidgetNotes->addItems(notes.openDirectory(QDir::currentPath()+"\\notes\\"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listWidgetNotes_clicked(const QModelIndex &index)
{
    ui->plainTextEditContent->setEnabled(true);
    change = false;
    if(isModified)
    {
        auto reply = QMessageBox::question(this, "Test", "Do you want save changes?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) on_pushButtonSave_clicked();
        else if(reply == QMessageBox::No) notes.closeFile();
        else
        {
            ui->listWidgetNotes->setCurrentIndex(lastIndex);
            return;
        }
    }
    isModified = false;
    this->setWindowTitle(index.data().toString()+" - VfNotes 1.0");
    ui->plainTextEditContent->setPlainText(notes.openFile(index.data().toString()));
    lastIndex = index;
}

void MainWindow::on_pushButtonSave_clicked()
{
    if(notes.checkOpenFile())
    {
        QMessageBox::information(this, "info", "File is not open!");
        return;
    }
    notes.saveCurrentFile(ui->plainTextEditContent->toPlainText().toUtf8());
    isModified = false;
}

void MainWindow::on_pushButtonNew_clicked()
{
    if(ui->lineEditNew->text().isEmpty())
    {
        QMessageBox::information(this, "info", "Name is empty!");
        return;
    }
    change = false;
    if(isModified)
    {
        auto reply = QMessageBox::question(this, "Test", "Do you want save changes?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            on_pushButtonSave_clicked();
            notes.closeFile();
            ui->plainTextEditContent->clear();
        }
        else if(reply == QMessageBox::No) notes.closeFile();
        else return;
    }
    isModified = false;
    ui->listWidgetNotes->clear();
    ui->listWidgetNotes->addItems(notes.newFile(ui->lineEditNew->text()));
    ui->lineEditNew->clear();
}

void MainWindow::on_pushButtonRemove_clicked()
{
    if(notes.checkOpenFile())
    {
        QMessageBox::information(this, "info", "File is not open!");
        return;
    }
    auto reply = QMessageBox::question(this, "Test", "Do you want remove this note?", QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        ui->listWidgetNotes->clear();
        ui->listWidgetNotes->addItems(notes.removeFile());
        this->setWindowTitle("VfNotes 1.0");
        ui->plainTextEditContent->clear();
        ui->plainTextEditContent->setEnabled(false);
        isModified = change = false;
    }
}

void MainWindow::on_plainTextEditContent_textChanged()
{
    if(!notes.checkOpenFile())
    {
        if(!change) change = true;
        else isModified = true;
    }
}

void MainWindow::on_pushButtonRename_clicked()
{
    if(ui->lineEditNew->text().isEmpty())
    {
        QMessageBox::information(this, "info", "Name is empty!");
        return;
    }
    notes.renameFile(lastIndex.data().toString(), ui->lineEditNew->text());
    this->setWindowTitle(ui->lineEditNew->text()+" - VfNotes 1.0");
    ui->lineEditNew->clear();
    ui->listWidgetNotes->clear();
    ui->listWidgetNotes->addItems(notes.getFilesList());
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    if(isModified)
    {
        QMessageBox::StandardButton exitButton = QMessageBox::question(this, "VfNotes", tr("Do you want save changes?"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
        switch(exitButton)
        {
            case QMessageBox::Yes:
                on_pushButtonSave_clicked();
                QCoreApplication::quit();
                break;
            case QMessageBox::No:
                QCoreApplication::quit();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
        }
    }
}
