#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDebug>
#include <QLoggingCategory>
#include <QQmlDebuggingEnabler>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QQmlContext>

#include <QFileInfo>

#include "nucleo.h"
#include "barcode.h"
#include "face.h"
#include "hand.h"

#include "todo.h"
#include "todomodel.h"
#include "dbhelper.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QQmlDebuggingEnabler enabler;
    QLoggingCategory::setFilterRules(
                "*.debug=true\n"
                "qt.*.debug=false"
                );

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);


    //This function registers the C++ model with qml type system
    qmlRegisterType<TodoModel>("org.erturk.todo", 1, 0, "TodoModel");
    qmlRegisterUncreatableType<TodoList>("org.erturk.todo", 1, 0, "TodoList",
                                         QStringLiteral("List should not be created in QML"));

    QFileInfo db_file(connectionString);
    DbHelper dbh;
    if (db_file.exists() && db_file.isFile())
    {
        dbh.ConnectToDb();
    }
    else
    {
        qDebug() << "Database file not found. Please check the connection string.";
        return 0;
    }

    TodoList todoList;

	ZXingQt::registerQmlAndMetaTypes();

    qmlRegisterType<QCvDetectFilter>("org.erturk.face", 1, 0, "FaceDetectFilter");
    qRegisterMetaType<QList<QRect>>("QList<QRect>");
    qmlRegisterType<HandDetectFilter>("org.erturk.hand", 1, 0, "HandDetectFilter");
    qmlRegisterType<Nucleo>("org.erturk.nucleo", 1, 0, "Nucleo");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("todoList"), &todoList);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
