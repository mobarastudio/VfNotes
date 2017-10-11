#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isModified = change = false;
    refresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::refresh()
{
    ui->listWidgetNotes->clear();
    dir.setPath(dir.currentPath()+"\\notes\\");
    auto notesList = dir.entryList();
    notesList.erase(notesList.begin(), notesList.begin()+2);
    ui->listWidgetNotes->addItems(notesList);
}

void MainWindow::on_listWidgetNotes_clicked(const QModelIndex &index)
{
    ui->plainTextEditContent->setEnabled(true);
    change = false;
    if(isModified)
    {
        auto reply = QMessageBox::question(this, "Test", "Do you want save changes?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) on_pushButtonSave_clicked();
        else if(reply == QMessageBox::No) currnetFile.close();
        else
        {
            ui->listWidgetNotes->setCurrentIndex(lastIndex);
            return;
        }
    }
    isModified = false;
    lastFile = dir.currentPath()+"\\notes\\"+index.data().toString();
    currnetFile.setFileName(lastFile);
    ui->labelCurrentNote->setText(index.data().toString());
    currnetFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content;
    //while(!currnetFile.atEnd()) content+=currnetFile.readLine();
    QTextStream stream(&currnetFile);
    stream.setCodec("UTF-8");
    while(!stream.atEnd()) content+=currnetFile.readLine();
    ui->plainTextEditContent->setPlainText(content.toUtf8());
    currnetFile.close();
    lastIndex = index;
}

void MainWindow::on_pushButtonSave_clicked()
{
    if(lastFile.isEmpty())
    {
        QMessageBox::information(this, "info", "File is not open!");
        return;
    }
    QTextStream stream(&currnetFile);
    stream.setCodec("UTF-8");
    currnetFile.setFileName(lastFile);
    currnetFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    stream<<ui->plainTextEditContent->toPlainText().toUtf8();
    currnetFile.close();
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
    QFile file(dir.currentPath()+"\\notes\\"+ui->lineEditNew->text());
    if(isModified)
    {
        auto reply = QMessageBox::question(this, "Test", "Do you want save changes?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            on_pushButtonSave_clicked();
            file.close();
            ui->plainTextEditContent->clear();
        }
        else if(reply == QMessageBox::No) currnetFile.close();
        else return;
    }
    isModified = false;
    file.open(QIODevice::WriteOnly | QIODevice::ReadOnly | QIODevice::Text);
    file.close();
    refresh();
    ui->lineEditNew->clear();
}

void MainWindow::on_pushButtonRemove_clicked()
{
    if(lastFile.isEmpty())
    {
        QMessageBox::information(this, "info", "File is not open!");
        return;
    }
    auto reply = QMessageBox::question(this, "Test", "Do you want remove this note?", QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        currnetFile.remove();
        refresh();
        lastFile.clear();
        ui->labelCurrentNote->setText("...");
        ui->plainTextEditContent->clear();
        ui->plainTextEditContent->setEnabled(false);
    }
}

void MainWindow::on_plainTextEditContent_textChanged()
{
    if(!lastFile.isEmpty())
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
    currnetFile.close();
    currnetFile.rename("notes\\"+ui->lineEditNew->text());
    lastFile = "notes\\"+ui->lineEditNew->text();
    ui->labelCurrentNote->setText(ui->lineEditNew->text());
    ui->lineEditNew->clear();
    refresh();
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    if(isModified)
    {
        QMessageBox::StandardButton exitButton = QMessageBox::question(this, "VfNotes", tr("Do you want save changes?"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
        /*if(exitButton == QMessageBox::Yes)
        {
            on_pushButtonSave_clicked();
            event->ignore();
        }*/
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
