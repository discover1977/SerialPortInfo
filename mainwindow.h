#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QCoreApplication>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *m_timer;
    QSystemTrayIcon *m_trayIcon;
    QIcon *icon;

    QList<QString> f(QList<QString> &list1, QList<QString> &list2);
    void changeEvent(QEvent *e);

private slots:
    void slot_ScanPorts();
    void slot_ShowForm();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void slot_chbAutorun();
};

#endif // MAINWINDOW_H
