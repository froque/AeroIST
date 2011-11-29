#include <QtGui/QApplication>
#include <QSharedMemory>
#include "aeroistwindow.h"

#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    QTranslator translator;
//    translator.load("aeroist");
//    app.installTranslator(&translator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("aeroist_" + QLocale::system().name());
    qDebug() << QLocale::system().name();
    app.installTranslator(&myappTranslator);

    AeroISTWindow w;

#if SINGLE
    //GUID : Generated once for your application
    // you could get one GUID here: http://www.guidgenerator.com/online-guid-generator.aspx
    QSharedMemory shared("5812860d-8064-47b5-92d7-98326bafa7f7");

    if( !shared.create( 512, QSharedMemory::ReadWrite) ){

        QMessageBox msgBox;
        msgBox.setText( QObject::tr("Can't start more than one instance of the application.") );
        msgBox.setIcon( QMessageBox::Critical );
        msgBox.exec();
        exit(0);
    }
#endif //SINGLE

#if SINGLE_PID
    QSettings settings("IST");
    qDebug() << settings.fileName();
    QVariant pidVariant = settings.value("pid");
    if (pidVariant.isNull()){                  // run normal
        qDebug() << "pid is null";
        settings.setValue("pid",a.applicationPid());
    } else {                            // don't run
        qDebug() << "pid is not null";


        int pid = pidVariant.toInt();

        // TODO: test pid is running

        exit(0);
    }
#endif //SINGLE_PID

    w.show();
    return app.exec();
}
