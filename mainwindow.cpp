#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowState(Qt::WindowMinimized);
    this->setWindowTitle("Serial port info");
    this->setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString progPath;
    progPath = settings.value("Serial port info", "").toString();

    if(progPath != "") {
        ui->chbAutorun->setChecked(true);
    } else {
        ui->chbAutorun->setChecked(false);
    }

    m_trayIcon = new QSystemTrayIcon(this);
    QIcon m_icon(":/pics/pic1.png");
    m_trayIcon->setIcon(m_icon);
    m_trayIcon->show();

    slot_ScanPorts();
    m_timer = new QTimer(this);
    m_timer->start(2000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::slot_ScanPorts);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(ui->chbAutorun, &QCheckBox::clicked, this, &MainWindow::slot_chbAutorun);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QString> MainWindow::f(QList<QString> &list1, QList<QString> &list2)
{
    return QSet<QString>::fromList(list2).subtract(QSet<QString>::fromList(list1)).toList();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick: {
        this->setWindowState(Qt::WindowActive);
        this->show();
    } break;
    case QSystemTrayIcon::MiddleClick:
        this->close(); break;
    default: break;
    }
}

void MainWindow::slot_chbAutorun()
{
    if(ui->chbAutorun->isChecked()) {
    #ifdef Q_OS_WIN32
        QSettings settings("\\HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.setValue("Serial port info", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        settings.sync();
    #endif
    } else {
    #ifdef Q_OS_WIN32
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.remove("Serial port info");
    #endif
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange && isMinimized())
    {
        QTimer::singleShot(0, this, SLOT(hide()));
        e->ignore();
    }
    else
        QMainWindow::changeEvent(e);
}

void MainWindow::slot_ScanPorts()
{
    ui->textEdit->clear();
    QString m_Ports = "";
    static QList<QString> prevPortList;
    QString allPorts;
    QList<QString> portList;
    QList<QString> deltaPortList;

    const auto portInfos = QSerialPortInfo::availablePorts();

    for(const QSerialPortInfo &portInfo : portInfos) {
        ui->textEdit->append(portInfo.portName() + " - " + portInfo.description());
        portList << (portInfo.portName());
        allPorts.append(portInfo.portName());
        allPorts.append("\r\n");
    }

    if(portList.count() != prevPortList.count()) {
        if(portList.count() > prevPortList.count()) {
            deltaPortList = f(prevPortList, portList);
            for(int i = 0; i < deltaPortList.count(); i++) {
                m_Ports.append(deltaPortList.at(i));
                m_Ports.append("\r\n");
            }
            m_trayIcon->showMessage("New COM-ports:", m_Ports);
        }
        m_trayIcon->setToolTip(allPorts);
        prevPortList = portList;
    }
}

void MainWindow::slot_ShowForm()
{
    this->setWindowState(Qt::WindowMaximized);
}
