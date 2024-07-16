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
    // necessary because of hiding
    mainLayout->setContentsMargins(6, 2, 6, 2);


    // pushButton to check if the panel gets mouse events
    QPushButton *button = new QPushButton("Applications", &w);
    button->setFixedSize(125,24);
    button->setToolTip("it doesn't even show up");
    button->setStyleSheet(styleSheet);
    mainLayout->addWidget(button);
    QObject::connect(button, &QPushButton::clicked, [&]() {
        qDebug() << "Application Button clicked!";
    });

    QProgressBar *progressBar = new QProgressBar();
    progressBar->setFixedSize(100, 24);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(70);
    progressBar->setStyleSheet(styleSheet);
    mainLayout->addWidget(progressBar);

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

    w.show();

    // QTimer::singleShot(5000, &a, &QGuiApplication::quit);
    return a.exec();
}
