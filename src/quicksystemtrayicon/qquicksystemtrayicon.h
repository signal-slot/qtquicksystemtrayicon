#ifndef QQUICKSYSTEMTRAYICON_H
#define QQUICKSYSTEMTRAYICON_H

#include <QtCore/QObject>
#include <QtGui/QIcon>
#include <QtQuickSystemTrayIcon/qtquicksystemtrayiconglobal.h>

QT_BEGIN_NAMESPACE

class QQuickImage;
class QQuickMenu;

class Q_QUICKSYSTEMTRAYICON_EXPORT QQuickSystemTrayIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool systemTrayAvailable READ isSystemTrayAvailable CONSTANT)
    Q_PROPERTY(bool supportsMessages READ supportsMessages CONSTANT)
    Q_PROPERTY(bool quitOnLastWindowClosed READ quitOnLastWindowClosed WRITE setQuitOnLastWindowClosed NOTIFY quitOnLastWindowClosedChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged DESIGNABLE false)
    Q_PROPERTY(QQuickImage *icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QQuickMenu *contextMenu READ contextMenu WRITE setContextMenu NOTIFY contextMenuChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_ENUMS(ActivationReason)
    Q_ENUMS(MessageIcon)
public:
    enum ActivationReason {
        Unknown,
        Context,
        DoubleClick,
        Trigger,
        MiddleClick
    };
    enum MessageIcon {
        NoIcon,
        Information,
        Warning,
        Critical
    };

    explicit QQuickSystemTrayIcon(QObject *parent = nullptr);
    ~QQuickSystemTrayIcon() override;

    bool isSystemTrayAvailable() const;
    bool supportsMessages() const;
    bool quitOnLastWindowClosed() const;
    bool isVisible() const;
    QQuickImage *icon() const;
    QQuickMenu *contextMenu() const;
    QString toolTip() const;

public Q_SLOTS:
    void setQuitOnLastWindowClosed(bool quitOnLastWindowClosed);
    void setVisible(bool visible);
    void setIcon(QQuickImage *icon);
    void setContextMenu(QQuickMenu *contextMenu);
    void setToolTip(const QString &toolTip);

    inline void show() { setVisible(true); }
    inline void hide() { setVisible(false); }
    void showMessage(const QString &title, const QString &message, MessageIcon icon = Information, int msecs = 10000);

Q_SIGNALS:
    void quitOnLastWindowClosedChanged(bool quitOnLastWindowClosed);
    void visibleChanged(bool visible);
    void iconChanged(QQuickImage *icon);
    void contextMenuChanged(QQuickMenu *contextMenu);
    void toolTipChanged(const QString &toolTip);

    void activated(QQuickSystemTrayIcon::ActivationReason reason);
    void messageClicked();

private:
    class Private;
    QScopedPointer<Private> d;
    Q_DISABLE_COPY(QQuickSystemTrayIcon)
};

QT_END_NAMESPACE

#endif // QQUICKSYSTEMTRAYICON_H
