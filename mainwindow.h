#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QCloseEvent>
#include <QShortcut>
#include "QFilesContainer.h"
#include "settingswindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setNotesFontSize(int fSize);
    void setNoteFontSize(int fSize);
private slots:
    void on_listWidgetNotes_clicked(const QModelIndex &index);

    void on_pushButtonSave_clicked();

    void on_pushButtonNew_clicked();

    void on_pushButtonRemove_clicked();

    void on_plainTextEditContent_textChanged();

    void on_pushButtonRename_clicked();

    void closeEvent(QCloseEvent *event);

    void showSettingsWindow();

private:
    Ui::MainWindow *ui;
    QFilesContainer notes;
    bool isModified, change;
    QModelIndex lastIndex;
    QShortcut *save;
    QShortcut *remove;
    settingsWindow s;
};

#endif // MAINWINDOW_H
