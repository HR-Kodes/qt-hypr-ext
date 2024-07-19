#include "widget.h"
#include "battery_level_widget.h"

#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QProgressBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWindow>

#include <QTime>
#include <QDateTime>
#include <QTimer>
#include <QCalendarWidget>
#include <QDialog>
#include <QVBoxLayout>

void updateButtonText(QPushButton *button) {
    // Get current date and time
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // QString dateTimeString = currentDateTime.toString("dddd, MMMM d yyyy, hh:mm:ss");
    QString dateTimeString = currentDateTime.toString("hh:mm - dddd d.");

    // Set button text
    button->setText(dateTimeString);

    // Schedule the next update after 1 second
    QTimer::singleShot(1000, [button]() { updateButtonText(button); });
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
    // Create the left section layout
    QHBoxLayout *leftLayout = new QHBoxLayout;
    leftLayout->setSpacing(6);


    // pushButton to check if the panel gets mouse events
    QPushButton *applicationBtn = new QPushButton("Applications", &w);
    applicationBtn->setFixedSize(140,24);
    applicationBtn->setToolTip("it doesn't even show up");
    applicationBtn->setObjectName("application-btn");
    applicationBtn->setStyleSheet(styleSheet);
    applicationBtn->setIcon(QIcon("./nixos.png"));
    applicationBtn->setIconSize(QSize(18, 17));
    // mainLayout->addWidget(applicationBtn);
    leftLayout->addWidget(applicationBtn);
    QObject::connect(applicationBtn, &QPushButton::clicked, [&]() {
        qDebug() << "Application Button clicked!";
    });



    // Create the center section layout
    QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addStretch(); // Spacer before the center widget


    QPushButton *calendarBtn = new QPushButton("Loading...");
    calendarBtn->setMinimumSize(140, 24);
    calendarBtn->setObjectName("calendar-btn");
    calendarBtn->setStyleSheet(styleSheet);
    centerLayout->addWidget(calendarBtn);
    updateButtonText(calendarBtn);
    centerLayout->addStretch();
    // centerLayout->addSpacing(100);


    // Create the right section layout
    QHBoxLayout *rightLayout = new QHBoxLayout;


    QProgressBar *batteryIndicator = new QProgressBar();
    batteryIndicator->setFixedSize(100, 24);
    batteryIndicator->setMinimum(0);
    batteryIndicator->setMaximum(100);
    batteryIndicator->setValue(0);
    batteryIndicator->setObjectName("battery-indicator");
    batteryIndicator->setStyleSheet(styleSheet);
    rightLayout->addWidget(batteryIndicator);



    // Add sections to the main layout
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(rightLayout);


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
        lsh->setExclusiveZone(barSize.height() + 6);
        lsh->setAnchors(LayerShellQt::Window::AnchorBottom);
        lsh->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    }

    BatteryLevelWidget widget(batteryIndicator);


    
    QDialog *calendarDialog = new QDialog(&w, Qt::Tool);
    QVBoxLayout *dialogLayout = new QVBoxLayout(calendarDialog);
    QCalendarWidget *calendarWidget = new QCalendarWidget();
    calendarWidget->setFirstDayOfWeek(Qt::Wednesday);
    calendarWidget->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    dialogLayout->addWidget(calendarWidget);
    calendarDialog->setLayout(dialogLayout);
    calendarDialog->setWindowFlags(calendarDialog->windowFlags() | Qt::FramelessWindowHint);
    calendarDialog->setStyleSheet("background-color: white;");

    QObject::connect(calendarBtn, &QPushButton::clicked, [calendarDialog, &w]() {
        QPoint globalPos = w.mapToGlobal(QPoint(0, 0));
        int yOffset = w.height();
        calendarDialog->move(globalPos.x(), globalPos.y() - yOffset);
        calendarDialog->setVisible(!calendarDialog->isVisible());
    });



    w.show();


    return a.exec();
}

#include "main.moc"
