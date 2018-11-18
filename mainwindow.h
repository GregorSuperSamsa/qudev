#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdev.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QUdev *udev;

private slots:
    void onUdevDeviceFount(QUdev::UdevDevice device);
    void on_buttonEnumerate_clicked();
    void on_buttonStartMonitoring_clicked();
    void on_buttonStopMonitoring_clicked();
};

#endif // MAINWINDOW_H
