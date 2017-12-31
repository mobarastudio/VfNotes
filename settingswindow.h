#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>

namespace Ui {
class settingsWindow;
}

class settingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit settingsWindow(QWidget *parent = 0);
    ~settingsWindow();
signals:
    void notesFontSize(int fSize);
    void noteFontSize(int fSize);
private slots:
    void on_pushButton_clicked();

private:
    Ui::settingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
