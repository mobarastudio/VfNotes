#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include <QDir>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QTextStream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), iNotesFontSize(12), iNoteFontSize(12)
{
    ui->setupUi(this);
    isModified = change = false;
    ui->listWidgetNotes->addItems(notes.openDirectory(QDir::currentPath()+"/notes/"));
    save = new QShortcut(QKeySequence(QKeySequence::Save), this);
    connect(save, SIGNAL(activated()), this, SLOT(on_pushButtonSave_clicked()));
    remove = new QShortcut(QKeySequence(QKeySequence::Delete), this);
    connect(remove, SIGNAL(activated()), this, SLOT(on_pushButtonRemove_clicked()));
    connect(ui->actionsettings, SIGNAL(triggered(bool)), this, SLOT(showSettingsWindow()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAboutWindow()));
    connect(&s, &settingsWindow::notesFontSize, this, &MainWindow::setNotesFontSize);
    connect(&s, &settingsWindow::noteFontSize, this, &MainWindow::setNoteFontSize);
    connect(&s, &settingsWindow::noteFontSize, this, &MainWindow::saveConfig);
    loadConfig();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete save;
    delete remove;
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
    ui->label_3->setText(ui->label_3->text() + " - Saved!");
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
        auto reply = QMessageBox::question(this, "Are you sure?", "Do you want save changes?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
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
    const QString newFileName = ui->lineEditNew->text();
    ui->listWidgetNotes->clear();
    ui->listWidgetNotes->addItems(notes.newFile(newFileName));
    ui->lineEditNew->clear();
    auto matchItems = ui->listWidgetNotes->findItems(newFileName, Qt::MatchExactly);
    ui->listWidgetNotes->setCurrentItem(matchItems[0]);
    ui->plainTextEditContent->setFocus();
}

void MainWindow::on_pushButtonRemove_clicked()
{
    if(notes.checkOpenFile())
    {
        QMessageBox::information(this, "info", "File is not open!");
        return;
    }
    auto reply = QMessageBox::question(this, "Are you sure?", "Do you want remove this note?", QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        ui->listWidgetNotes->clear();
        ui->listWidgetNotes->addItems(notes.removeFile());
        this->setWindowTitle("VfNotes 1.1");
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
    notes.renameFile(ui->lineEditNew->text());
    this->setWindowTitle(ui->lineEditNew->text()+" - VfNotes 1.1");

    QString nazwaPliku = ui->lineEditNew->text();
    ui->label_3->setText(nazwaPliku);

    ui->lineEditNew->clear();
    ui->listWidgetNotes->clear();
    ui->listWidgetNotes->addItems(notes.getFilesList());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isModified)
    {
        QMessageBox::StandardButton exitButton = QMessageBox::question(this, "VfNotes", tr("Do you want save changes?"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
        switch(exitButton)
        {
            case QMessageBox::Yes:
                on_pushButtonSave_clicked();
                QCoreApplication::quit();
                s.close();
                break;
            case QMessageBox::No:
                QCoreApplication::quit();
                s.close();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
        }
    }
    else
    {
        s.close();
    }
}

void MainWindow::showSettingsWindow()
{
    s.fontsInWindow(ui->plainTextEditContent->fontInfo().pointSize(), ui->listWidgetNotes->fontInfo().pointSize());
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
    QMessageBox::information(this, "About", "VfNotes release 1.1. Written by Arkadiusz97.");
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

void MainWindow::on_listWidgetNotes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)//Test!
{
    if(current != NULL)
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
                QTimer::singleShot(0, qApp, [this, previous]
                {
                    ui->listWidgetNotes->blockSignals(true);
                    ui->listWidgetNotes->setCurrentItem(previous);
                    ui->listWidgetNotes->blockSignals(false);
                });
                return;
            }
        }
        isModified = false;
        this->setWindowTitle(current->text()+" - VfNotes 1.1");

        QString nazwaPliku = current->text();
        ui->label_3->setText(nazwaPliku);

        ui->plainTextEditContent->setPlainText(notes.openFile(current->text()));
    }
}

void MainWindow::on_listWidgetNotes_clicked(const QModelIndex &index)
{
    ui->plainTextEditContent->setFocus();
    QTextCursor cursor(ui->plainTextEditContent->textCursor());
    cursor.movePosition(QTextCursor::End);
    ui->plainTextEditContent->setTextCursor(cursor);
}

 QString kompozycja = "dark";



void MainWindow::on_pushButton_clicked()
{
    if(kompozycja == "dark")
    {
        kompozycja = "light";



        ui->plainTextEditContent->setStyleSheet("background-color: white; color:black;");
        ui->listWidgetNotes->setStyleSheet("background-color: white; color:black;");
        ui->lineEditNew->setStyleSheet("background-color: white; color:black;");
        setStyleSheet("background-color: lightgray;");
        ui->pushButton->setText("Dark");
        ui->pushButton->setStyleSheet("color: black;");
        ui->pushButtonSave->setStyleSheet("color:black;");
        ui->pushButtonNew->setStyleSheet("color: black;");
        ui->label->setStyleSheet("color: black");
        ui->label_2->setStyleSheet("color: black");
        ui->label_3->setStyleSheet("color: black");
        ui->pushButtonRemove->setStyleSheet("color: black;");
        ui->pushButtonRename->setStyleSheet("color: black;");
    } else if(kompozycja == "light")
    {
        kompozycja = "dark";

        ui->plainTextEditContent->setStyleSheet("background-color: #474747; color:white;");
        ui->listWidgetNotes->setStyleSheet("background-color: #474747; color:white;");
        ui->lineEditNew->setStyleSheet("background-color: #474747; color:white;");
        setStyleSheet("background-color: #303739;");
        ui->pushButton->setStyleSheet("color: white;");
        ui->pushButton->setText("Light");
        ui->pushButtonSave->setStyleSheet("background-color: #314d68;");
        ui->pushButtonNew->setStyleSheet("background-color: #00682f;");
        ui->label->setStyleSheet("color: white");
        ui->label_2->setStyleSheet("color: white");
        ui->label_3->setStyleSheet("color: white");
        ui->pushButtonRemove->setStyleSheet("background-color: #ce0a1b;");
        ui->pushButtonRename->setStyleSheet("background-color: #b24f1e;");

    } else
    {
        QMessageBox::information(this, "info", "Error!");
        return;
    }
}
