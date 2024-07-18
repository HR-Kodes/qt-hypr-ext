#include "battery_level_widget.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

BatteryLevelWidget::BatteryLevelWidget(QProgressBar *progressBar, QWidget *parent)
    : QWidget(parent), batteryIndicator(progressBar), batteryPath("") {
    label = new QLabel("Battery Level: N/A", this);

    findBatteryPath();
    if (!batteryPath.isEmpty()) {
        connectToDbus();
        updateBatteryLevel(); // Initial update
    } else {
        label->setText("Battery Level: N/A (Battery not found)");
    }
}

void BatteryLevelWidget::updateBatteryLevel() {
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

void BatteryLevelWidget::findBatteryPath() {
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

void BatteryLevelWidget::connectToDbus() {
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
