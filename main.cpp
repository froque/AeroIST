#include <QtGui/QApplication>
#include <QSharedMemory>
#include "aeroistwindow.h"

#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("IST");
    QCoreApplication::setApplicationName("AeroIST");
//    QTranslator translator;
//    translator.load("aeroist");
//    app.installTranslator(&translator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("aeroist_" + QLocale::system().name());
//    qDebug() << QLocale::system().name();
    app.installTranslator(&myappTranslator);

    AeroISTWindow w;

#if SINGLE_SHARED
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

#endif //SINGLE_SHARED

#if SINGLE_SETTINGS
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


#if SINGLE_PID
    QString filename(QDir::homePath());
    filename.append("/.config/IST/aeroist.pid");
    QFile file(filename);

    if (file.exists()){
        qDebug() << "file exists";
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "can't open file to read" << file.fileName();
            exit(0);
        }

        QTextStream in(&file);
        QString aux;
        int pidnum;
        in >> aux;
        file.close();

        pidnum = aux.toInt();
        qDebug() <<  pidnum << app.applicationPid() << QFileInfo(app.applicationFilePath()).fileName();
        aux = QString("/proc/%1/cmdline").arg(pidnum);
//        aux.append(pidnum);
//        aux.append("/cmdline");
        QFile proc(aux);
        qDebug() << aux;
        if (proc.exists()){
            if (!proc.open(QIODevice::ReadOnly )){
                qDebug() << "can't open proc file to read" << proc.fileName();
                exit(0);
            }
            qDebug() << aux << "exists";
            QTextStream in(&proc);
            in >> aux;
            proc.close();
            qDebug() << aux;
            if (aux.contains(QFileInfo(app.applicationFilePath()).fileName())){
                exit (0);
            }
        }
    }

    // write pid to file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "can't open file to write" << file.fileName();
        exit(0);
    }
    QTextStream out(&file);
    out << app.applicationPid();
    file.close();

#endif //SINGLE_PID


#if SINGLE_SOCKET
    QLocalSocket socket;
    socket.connectToServer(AEROIST);
    if (socket.waitForConnected(500))
            return 0; // Exit already a process running

    QLocalServer *m_localServer;
    m_localServer = new QLocalServer(this);
    connect(m_localServer, SIGNAL(newConnection()), this, SLOT(newLocalSocketConnection()));
    m_localServer->listen(AEROIST);

#endif //SINGLE_SOCKET

    w.show();
    return app.exec();
}
