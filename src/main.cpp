#include <KLocalizedString>
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("winver"));

    MainWindow win;
    win.show();

    return app.exec();
}
