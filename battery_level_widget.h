#ifndef BATTERY_LEVEL_WIDGET_H
#define BATTERY_LEVEL_WIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

class BatteryLevelWidget : public QWidget {
    Q_OBJECT
public:
    BatteryLevelWidget(QProgressBar *progressBar, QWidget *parent = nullptr);

private slots:
    void updateBatteryLevel();

private:
    QLabel *label;
    QProgressBar *batteryIndicator;
    QString batteryPath;

    void findBatteryPath();
    void connectToDbus();
};

#endif // BATTERY_LEVEL_WIDGET_H
