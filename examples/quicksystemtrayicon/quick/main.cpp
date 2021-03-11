#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlFile>

class IconSetter : public QObject
{
    Q_OBJECT
public:
    IconSetter(QWindow *window)
        : QObject(window)
        , window(window)
    {
        connect(window, SIGNAL(iconUrlChanged()), this, SLOT(updateWindowIcon()));
        updateWindowIcon();
    }

private slots:
    void updateWindowIcon() {
        window->setIcon(QIcon(QQmlFile::urlToLocalFileOrQrc(window->property("iconUrl").toUrl())));
    }

private:
    QWindow *window = nullptr;
};


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        new IconSetter(qobject_cast<QWindow *>(obj));
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

#include "main.moc"
