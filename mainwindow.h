#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_listWidgetNotes_clicked(const QModelIndex &index);

    void refresh();

    void on_pushButtonSave_clicked();

    void on_pushButtonNew_clicked();

    void on_pushButtonRemove_clicked();

    void on_plainTextEditContent_textChanged();

    void on_pushButtonRename_clicked();

    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    QDir dir;
    QFile currnetFile;
    QString lastFile;
    bool isModified, change;
    QModelIndex lastIndex;
};

#endif // MAINWINDOW_H
