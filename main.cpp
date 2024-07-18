#include "widget.h"

#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QWidget>
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


class BatteryLevelWidget : public QWidget {
    Q_OBJECT
public:
    BatteryLevelWidget(QProgressBar *progressBar, QWidget *parent = nullptr) : QWidget(parent), batteryPath("") {
        label = new QLabel("Battery Level: N/A", this);
        batteryIndicator = progressBar;


        findBatteryPath();
        if (!batteryPath.isEmpty()) {
            connectToDbus();
            updateBatteryLevel(); // Initial update
        } else {
            label->setText("Battery Level: N/A (Battery not found)");
        }
    }

private slots:
    void updateBatteryLevel() {
        if (batteryPath.isEmpty()) {
            label->setText("Battery Level: N/A (Battery not found)");
            return;
        }

        QDBusInterface upower("org.freedesktop.UPower",
                              batteryPath,
                              "org.freedesktop.UPower.Device",
                              QDBusConnection::systemBus());

        if (upower.isValid()) {
            QVariant percentageVariant = upower.property("Percentage");
            if (percentageVariant.isValid()) {
                double batteryPercentage = percentageVariant.toDouble();
                label->setText(QString("Battery Level: %1%").arg(batteryPercentage));
                batteryIndicator->setValue(static_cast<int>(batteryPercentage));
            } else {
                qDebug() << "Failed to get battery percentage";
                label->setText("Battery Level: N/A (Error getting percentage)");
            }
        } else {
            qDebug() << "UPower interface is not valid";
            label->setText("Battery Level: N/A (Invalid UPower interface)");
        }
    }

private:
    QLabel *label;
    QProgressBar *batteryIndicator;
    QString batteryPath;

    void findBatteryPath() {
        QDBusInterface upower("org.freedesktop.UPower",
                              "/org/freedesktop/UPower",
                              "org.freedesktop.UPower",
                              QDBusConnection::systemBus());

        if (upower.isValid()) {
            QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
            if (reply.isValid()) {
                const auto &devices = reply.value();
                for (const auto &device : devices) {
                    QDBusInterface deviceInterface("org.freedesktop.UPower",
                                                   device.path(),
                                                   "org.freedesktop.UPower.Device",
                                                   QDBusConnection::systemBus());
                    if (deviceInterface.isValid()) {
                        QVariant typeVariant = deviceInterface.property("Type");
                        if (typeVariant.isValid() && typeVariant.toUInt() == 2) { // Type 2 indicates Battery
                            batteryPath = device.path();
                            qDebug() << "Found battery at path:" << batteryPath;
                            break;
                        }
                    }
                }
            } else {
                qDebug() << "Failed to enumerate devices:" << reply.error().message();
            }
        } else {
            qDebug() << "UPower interface is not valid";
        }
    }

    void connectToDbus() {
        if (batteryPath.isEmpty()) return;

        QDBusConnection::systemBus().connect(
            "org.freedesktop.UPower",
            batteryPath,
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(updateBatteryLevel())
        );
    }
};


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


    BatteryLevelWidget widget(batteryIndicator);


    w.show();


    // QTimer::singleShot(5000, &a, &QGuiApplication::quit);
    return a.exec();
}





#include "main.moc"
