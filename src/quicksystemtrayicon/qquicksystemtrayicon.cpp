#include "qquicksystemtrayicon.h"
#include <QtGui/QIcon>
#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformmenu.h>
#include <QtGui/qpa/qplatformsystemtrayicon.h>
#include <QtGui/qpa/qplatformtheme.h>
#include <QtQml/QQmlFile>
#include <QtQuick/private/qquickimage_p.h>
#include <QtQuickTemplates2/private/qquickaction_p.h>
#include <QtQuickTemplates2/private/qquickmenu_p.h>
#include <QtQuickTemplates2/private/qquickmenuitem_p.h>
#include <QtQuickTemplates2/private/qquickmenuseparator_p.h>

QT_BEGIN_NAMESPACE
class QQuickSystemTrayIcon::Private
{
public:
    Private(QQuickSystemTrayIcon *parent);
    void showMessage(const QString &title, const QString &message, QQuickSystemTrayIcon::MessageIcon icon, int msecs);

private:
    void show();
    void hide();
    void updateIcon();
    void updateContextMenu();
    void updateContextMenu(QPlatformMenu *platformMenu, QQuickMenu *quickMenu);

private:
    QQuickSystemTrayIcon *q;
    QScopedPointer<QPlatformSystemTrayIcon> platformSystemTrayIcon;
    QScopedPointer<QPlatformMenu> platformMenu;

public:
    bool systemTrayAvailable;
    bool supportsMessages;
    bool quitOnLastWindowClosed = true;
    bool visible = false;
    QQuickImage *icon = nullptr;
    QList<QMetaObject::Connection> sysConnections;
    QList<QMetaObject::Connection> iconConnections;
    QList<QMetaObject::Connection> contextMenuConnections;
    QQuickMenu *contextMenu = nullptr;
    QString toolTip;
};

QQuickSystemTrayIcon::Private::Private(QQuickSystemTrayIcon *parent)
    : q(parent)
    , platformSystemTrayIcon(QGuiApplicationPrivate::platformTheme()->createPlatformSystemTrayIcon())
    , platformMenu(platformSystemTrayIcon->createMenu())
    , systemTrayAvailable(platformSystemTrayIcon->isSystemTrayAvailable())
    , supportsMessages(platformSystemTrayIcon->supportsMessages())
    , quitOnLastWindowClosed(QGuiApplication::quitOnLastWindowClosed())
{
    connect(q, &QQuickSystemTrayIcon::quitOnLastWindowClosedChanged, q, [](bool quitOnLastWindowClosed) {
        QGuiApplication::setQuitOnLastWindowClosed(quitOnLastWindowClosed);
    });
    connect(q, &QQuickSystemTrayIcon::visibleChanged, [this](bool visible) {
        if (visible)
            show();
        else
            hide();
    });
    connect(q, &QQuickSystemTrayIcon::iconChanged, [this](QQuickImage *icon) {
        for (const auto &connection : qAsConst(iconConnections))
            disconnect(connection);
        iconConnections.clear();
        if (icon) {
            iconConnections << connect(icon, &QQuickImage::statusChanged, [this]() { updateIcon(); });
            iconConnections << connect(icon, &QQuickImage::currentFrameChanged, [this]() { updateIcon(); });
        }
        updateIcon();
    });
    connect(q, &QQuickSystemTrayIcon::contextMenuChanged, [this](QQuickMenu *contextMenu) {
        for (const auto &connection : qAsConst(contextMenuConnections))
            disconnect(connection);
        contextMenuConnections.clear();
        if (contextMenu) {
            contextMenuConnections << connect(contextMenu, &QQuickMenu::titleChanged, [this]() { updateContextMenu(); });
        }
        updateContextMenu();
    });
    connect(q, &QQuickSystemTrayIcon::toolTipChanged, [this](const QString &toolTip) {
        platformSystemTrayIcon->updateToolTip(toolTip);
    });
}

void QQuickSystemTrayIcon::Private::show()
{
    platformSystemTrayIcon->init();
    sysConnections << connect(platformSystemTrayIcon.data(), &QPlatformSystemTrayIcon::activated,
                           [this](QPlatformSystemTrayIcon::ActivationReason reason) {
                   emit q->activated(static_cast<QQuickSystemTrayIcon::ActivationReason>(reason));
    });

    sysConnections << connect(platformSystemTrayIcon.data(), &QPlatformSystemTrayIcon::messageClicked,
                           q, &QQuickSystemTrayIcon::messageClicked);

    if (icon)
        platformSystemTrayIcon->updateIcon(QIcon(QPixmap::fromImage(icon->image())));
}

void QQuickSystemTrayIcon::Private::hide()
{
    for (const auto &connection : qAsConst(sysConnections)) {
        disconnect(connection);
    }
    sysConnections.clear();
    platformSystemTrayIcon->cleanup();
}

void QQuickSystemTrayIcon::Private::updateIcon()
{
    platformSystemTrayIcon->updateIcon(icon ? QIcon(QPixmap::fromImage(icon->image())) : QIcon());
}

void QQuickSystemTrayIcon::Private::updateContextMenu()
{
    if (contextMenu) {
        updateContextMenu(platformMenu.get(), contextMenu);
    }
    platformSystemTrayIcon->updateMenu(platformMenu.get());
}

void QQuickSystemTrayIcon::Private::updateContextMenu(QPlatformMenu *platformMenu, QQuickMenu *quickMenu)
{
    auto setText = [=](const QString &title) { platformMenu->setText(title); };
    connect(quickMenu, &QQuickMenu::titleChanged, setText);
    setText(quickMenu->title());

    for (int i = 0; i < quickMenu->count(); i++) {
        auto platformMenuItem = platformMenu->createMenuItem();
        const auto item = quickMenu->itemAt(i);
        const auto menu = quickMenu->menuAt(i);
        const auto menuItem = qobject_cast<QQuickMenuItem *>(item);
        const auto menuSeparator = qobject_cast<QQuickMenuSeparator *>(item);
        const auto action = quickMenu->actionAt(i);
        if (menu) {
            auto subMenu = platformMenu->createSubMenu();
            updateContextMenu(subMenu, menu);
            platformMenuItem->setMenu(subMenu);
        } else if (menuSeparator) {
            platformMenuItem->setIsSeparator(true);
        } else if (action) {
            auto setText = [=](const QString &text) {
                platformMenuItem->setText(text);
                platformMenu->syncMenuItem(platformMenuItem);
            };
            connect(action, &QQuickAction::textChanged, setText);
            setText(action->text());

            auto setIcon = [=](const QQuickIcon &quickIcon) {
                qDebug() << quickIcon.source() << quickIcon.source().toLocalFile() << QQmlFile::urlToLocalFileOrQrc(quickIcon.source());
                const auto icon = QIcon::fromTheme(quickIcon.name(), QIcon(QQmlFile::urlToLocalFileOrQrc(quickIcon.source())));
                platformMenuItem->setIcon(icon);
                platformMenu->syncMenuItem(platformMenuItem);
            };
            connect(action, &QQuickAction::iconChanged, setIcon);
            setIcon(action->icon());

            auto setEnabled = [=](bool enabled) {
                platformMenuItem->setEnabled(enabled);
                platformMenu->syncMenuItem(platformMenuItem);
            };
            connect(action, &QQuickAction::enabledChanged, setEnabled);
            setEnabled(action->isEnabled());

            auto setCheckable = [=](bool checkable) {
                platformMenuItem->setCheckable(checkable);
                platformMenu->syncMenuItem(platformMenuItem);
            };
            connect(action, &QQuickAction::checkableChanged, setCheckable);
            setCheckable(action->isCheckable());

            auto setChecked = [=](bool checked) {
                platformMenuItem->setChecked(checked);
                platformMenu->syncMenuItem(platformMenuItem);
            };
            connect(action, &QQuickAction::checkedChanged, setChecked);
            setChecked(action->isChecked());

            auto setShortcut = [=](const QKeySequence &shortcut) {
                platformMenuItem->setShortcut(shortcut);
                platformMenu->syncMenuItem(platformMenuItem);
            };
            connect(action, &QQuickAction::shortcutChanged, setShortcut);
            setShortcut(action->shortcut());

            connect(platformMenuItem, &QPlatformMenuItem::activated, [=]() { action->trigger(); });
        } else if (menuItem) {
            auto setText = [=]() { platformMenuItem->setText(menuItem->text()); };
            connect(menuItem, &QQuickMenuItem::textChanged, setText);
            setText();

            auto setIcon = [=]() {
                const auto quickIcon = menuItem->icon();
                const auto icon = QIcon::fromTheme(quickIcon.name(), QIcon(QQmlFile::urlToLocalFileOrQrc(quickIcon.source())));
                platformMenuItem->setIcon(icon);
            };
            connect(menuItem, &QQuickMenuItem::iconChanged, setIcon);
            setIcon();

            auto setEnabled = [=]() { platformMenuItem->setEnabled(menuItem->isEnabled()); };
            connect(menuItem, &QQuickMenuItem::enabledChanged, setEnabled);
            setEnabled();

            auto setCheckable = [=]() { platformMenuItem->setCheckable(menuItem->isCheckable()); };
            connect(menuItem, &QQuickMenuItem::checkableChanged, setCheckable);
            setCheckable();

            auto setChecked = [=]() { platformMenuItem->setChecked(menuItem->isChecked()); };
            connect(menuItem, &QQuickMenuItem::checkedChanged, setChecked);
            setChecked();

            connect(platformMenuItem, &QPlatformMenuItem::activated, menuItem, &QQuickMenuItem::clicked);
        }
        platformMenu->insertMenuItem(platformMenuItem, nullptr);
    }
}

void QQuickSystemTrayIcon::Private::showMessage(const QString &title, const QString &message, QQuickSystemTrayIcon::MessageIcon icon, int msecs)
{
    QIcon i;
    switch (icon) {
    case NoIcon:
        break;
    case Information:
        i = QIcon::fromTheme(QStringLiteral("dialog-information"));
        break;
    case Warning:
        i = QIcon::fromTheme(QStringLiteral("dialog-warning"));
        break;
    case Critical:
        i = QIcon::fromTheme(QStringLiteral("dialog-error"));
        break;
    default:
        break;
    }

    platformSystemTrayIcon->showMessage(title, message, i, static_cast<QPlatformSystemTrayIcon::MessageIcon>(icon), msecs);
}


QQuickSystemTrayIcon::QQuickSystemTrayIcon(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

QQuickSystemTrayIcon::~QQuickSystemTrayIcon() = default;

bool QQuickSystemTrayIcon::isSystemTrayAvailable() const
{
    return d->systemTrayAvailable;
}

bool QQuickSystemTrayIcon::supportsMessages() const
{
    return d->supportsMessages;
}

bool QQuickSystemTrayIcon::quitOnLastWindowClosed() const
{
    return d->quitOnLastWindowClosed;
}

void QQuickSystemTrayIcon::setQuitOnLastWindowClosed(bool quitOnLastWindowClosed)
{
    if (d->quitOnLastWindowClosed == quitOnLastWindowClosed) return;
    d->quitOnLastWindowClosed = quitOnLastWindowClosed;
    emit quitOnLastWindowClosedChanged(quitOnLastWindowClosed);
}

bool QQuickSystemTrayIcon::isVisible() const
{
    return d->visible;
}

void QQuickSystemTrayIcon::setVisible(bool visible)
{
    if (d->visible == visible) return;
    d->visible = visible;
    emit visibleChanged(visible);
}

QQuickImage *QQuickSystemTrayIcon::icon() const
{
    return d->icon;
}

void QQuickSystemTrayIcon::setIcon(QQuickImage *icon)
{
    if (d->icon == icon) return;
    d->icon = icon;
    emit iconChanged(icon);
}

QQuickMenu *QQuickSystemTrayIcon::contextMenu() const
{
    return d->contextMenu;
}

void QQuickSystemTrayIcon::setContextMenu(QQuickMenu *contextMenu)
{
    if (d->contextMenu == contextMenu) return;
    d->contextMenu = contextMenu;
    emit contextMenuChanged(contextMenu);
}

QString QQuickSystemTrayIcon::toolTip() const
{
    return d->toolTip;
}

void QQuickSystemTrayIcon::setToolTip(const QString &toolTip)
{
    if (d->toolTip == toolTip) return;
    d->toolTip = toolTip;
    emit toolTipChanged(toolTip);
}

void QQuickSystemTrayIcon::showMessage(const QString &title, const QString &message, QQuickSystemTrayIcon::MessageIcon icon, int msecs)
{
    d->showMessage(title, message, icon, msecs);
}

QT_END_NAMESPACE
