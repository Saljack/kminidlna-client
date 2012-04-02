#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QtCore/QDir>
#include <QDeclarativeEngine>
#include <QtDeclarative>
#include <QDeclarativeContext>
#include "networkclient.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());
#ifdef Q_OS_ANDROID
     viewer->addImportPath("/imports/");
     viewer->engine()->addPluginPath(QDir::homePath()+"/../lib");
#endif

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer->setMainQmlFile(QLatin1String("qml/kminidlna/main.qml"));
    viewer->showExpanded();


    NetworkClient cli;

    viewer->rootContext()->setContextProperty("client", &cli);

    return app->exec();
}
