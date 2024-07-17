#include "widget.h"

#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QProgressBar>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QStyle>
#include <QVBoxLayout>
#include <QTimer>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariantMap>

#include "main.moc" 



// Function to read battery level from Linux sysfs
int readBatteryLevel() {
    QFile file("/sys/class/power_supply/BAT0/capacity"); // Adjust BAT0 based on your system
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    QString line = in.readLine();
    file.close();

    bool ok;
    int batteryLevel = line.toInt(&ok);
    if (!ok)
        return -1;

    return batteryLevel;
}


int main(int argc, char *argv[])
{
    LayerShellQt::Shell::useLayerShell();
    
    QFile file("main.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    QSize barSize = QSize(1366, 30);

    QApplication a(argc, argv);


    Widget w;

    // hacky way to get QWindow
    w.show();
    w.hide();
    w.setFixedWidth(barSize.width());
    w.setStyleSheet(styleSheet);

    QHBoxLayout *mainLayout = new QHBoxLayout(&w);
    mainLayout->setContentsMargins(6, 2, 6, 2);


    // pushButton to check if the panel gets mouse events
    QPushButton *button = new QPushButton("Applications", &w);
    button->setFixedSize(140,24);
    button->setToolTip("it doesn't even show up");
    button->setObjectName("application-btn");
    button->setStyleSheet(styleSheet);
    button->setIcon(QIcon("./nixos.png"));
    button->setIconSize(QSize(18, 17));
    mainLayout->addWidget(button);
    QObject::connect(button, &QPushButton::clicked, [&]() {
        qDebug() << "Application Button clicked!";
    });



    QProgressBar *batteryIndicator = new QProgressBar();
    batteryIndicator->setFixedSize(100, 24);
    batteryIndicator->setMinimum(0);
    batteryIndicator->setMaximum(100);
    batteryIndicator->setValue(0);
    // batteryIndicator->setValue(readBatteryLevel());
    batteryIndicator->setObjectName("battery-indicator");
    batteryIndicator->setStyleSheet(styleSheet);
    mainLayout->addWidget(batteryIndicator);

    w.setLayout(mainLayout);
    w.setStyleSheet("QWidget { background-color: \"#171717\"; }");
    w.resize(barSize);
    qDebug() << w.size();

    QWindow *lwin = w.windowHandle();
    qDebug() << lwin;

    if (LayerShellQt::Window *lsh = LayerShellQt::Window::get(lwin)) {
        qDebug() << lsh;
        lsh->setScope("atombar");
        lsh->setLayer(LayerShellQt::Window::LayerTop);
        // lsh->setExclusiveZone(barSize.width());
        lsh->setExclusiveZone(barSize.height() + 6);
        lsh->setAnchors(LayerShellQt::Window::AnchorBottom);
        lsh->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    }
    QTimer batteryTimer;
    batteryTimer.setInterval(1000); // Update interval: 1000 ms = 1 second
    QObject::connect(&batteryTimer, &QTimer::timeout, [&]() {
        int batteryLevel = readBatteryLevel();
        batteryIndicator->setValue(batteryLevel);
    });
    batteryTimer.start();


    w.show();
    
    // QTimer::singleShot(5000, &a, &QGuiApplication::quit);
    return a.exec();
}
