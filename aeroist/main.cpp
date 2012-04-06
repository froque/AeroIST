#include <QtGui/QApplication>

#include "aeroistwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("IST");
    QCoreApplication::setApplicationName("AeroIST");

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("aeroist_" + QLocale::system().name());

    app.installTranslator(&myappTranslator);

    AeroISTWindow w;
    if (app.arguments().size() == 2){
        w.load_xml(app.arguments().at(1));
    }

    w.setWindowIcon(QIcon(":/resources/aeroist.svg"));

    w.show();
    return app.exec();
}
