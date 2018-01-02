#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include <QDir>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), iNotesFontSize(12), iNoteFontSize(12)
{
    ui->setupUi(this);
    isModified = change = false;
    ui->listWidgetNotes->addItems(notes.openDirectory(QDir::currentPath()+"\\notes\\"));
    save = new QShortcut(QKeySequence(QKeySequence::Save), this);
    connect(save, SIGNAL(activated()), this, SLOT(on_pushButtonSave_clicked()));
    remove = new QShortcut(QKeySequence(QKeySequence::Delete), this);
    connect(remove, SIGNAL(activated()), this, SLOT(on_pushButtonRemove_clicked()));
    connect(ui->actionsettings, SIGNAL(triggered(bool)), this, SLOT(showSettingsWindow()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAboutWindow()));
    connect(&s, &settingsWindow::notesFontSize, this, &MainWindow::setNotesFontSize);
    connect(&s, &settingsWindow::noteFontSize, this, &MainWindow::setNoteFontSize);
    loadConfig();//test
}

MainWindow::~MainWindow()
{
    delete ui;
    delete save;
    delete remove;
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
    saveConfig();
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

void MainWindow::showSettingsWindow()
{
    s.show();
}

void MainWindow::setNotesFontSize(int fSize)
{
    iNotesFontSize = fSize;
    ui->listWidgetNotes->setFont(QFont("", fSize));
}

void MainWindow::setNoteFontSize(int fSize)
{
    iNoteFontSize = fSize;
    ui->plainTextEditContent->setFont(QFont("", fSize));
}

void MainWindow::showAboutWindow()
{
    QMessageBox::information(this, "About", "VfNotes release 1.0. Written by Arkadiusz97.");
}

void MainWindow::loadConfig()//temporarily
{
    QString matched;
    QFile settingsFile("VfNotes_settings.txt");
    settingsFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QString settingsFileContent = settingsFile.readAll();
    QRegularExpression re1("NotesFontSize ([0-9]{1,})"), re2("NoteFontSize ([0-9]{1,})");
    QRegularExpressionMatch match = re1.match(settingsFileContent);
    if(match.hasMatch()) matched = match.captured(1);
    iNotesFontSize = matched.toInt();
    setNotesFontSize(iNotesFontSize);
    match = re2.match(settingsFileContent);
    if(match.hasMatch()) matched = match.captured(1);
    iNoteFontSize = matched.toInt();
    setNoteFontSize(iNoteFontSize);
    settingsFile.close();
}

void MainWindow::saveConfig()
{
    QFile settingsFile("VfNotes_settings.txt");
    QTextStream stream(&settingsFile);
    settingsFile.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    stream<<"NotesFontSize "<<QString::number(iNotesFontSize)<<"\n"<<"NoteFontSize "<<QString::number(iNoteFontSize)<<"\n";
    settingsFile.close();
}
