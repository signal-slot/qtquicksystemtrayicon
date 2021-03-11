#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/qqml.h>
#include <QtQuickSystemTrayIcon/QQuickSystemTrayIcon>

QT_BEGIN_NAMESPACE

class QtQuickSystemTrayIconDeclarativeModule: public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QtQuickSystemTrayIconDeclarativeModule(QObject *parent = nullptr)
        : QQmlExtensionPlugin(parent)
    {}

    void registerTypes(const char *uri) override
    {
        // @uri QtQuick.SystemTrayIcon
        qmlRegisterType<QQuickSystemTrayIcon>(uri, 2, 15, "SystemTrayIcon");
    }
};

QT_END_NAMESPACE

#include "main.moc"
