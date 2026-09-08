#pragma once

#include <QtQml/QQmlAbstractUrlInterceptor>

#include "QGCCorePlugin.h"
#include "QGCOptions.h"

class CustomOptions;
class CustomOverrideInterceptor;

/// The custom Fly view supplies its own telemetry strip and attitude cluster, so QGC's
/// instrument panel is turned off to keep the right edge clear.
class CustomFlyViewOptions : public QGCFlyViewOptions
{
    Q_OBJECT

public:
    explicit CustomFlyViewOptions(CustomOptions *options, QObject *parent = nullptr);

    // Overrides from QGCFlyViewOptions

    bool showInstrumentPanel() const final { return false; }
};

/*===========================================================================*/

class CustomOptions : public QGCOptions
{
    Q_OBJECT

public:
    explicit CustomOptions(QObject *parent = nullptr);

    // Overrides from QGCOptions

    const QGCFlyViewOptions *flyViewOptions() const final { return _flyViewOptions; }

private:
    CustomFlyViewOptions *_flyViewOptions = nullptr;
};

/*===========================================================================*/

/// Core plugin for the DJI-style skin: recolors the QGC palette and swaps in a Pilot-2-style
/// Fly view overlay. Vehicle, firmware and mission behavior are unchanged.
class CustomPlugin : public QGCCorePlugin
{
    Q_OBJECT

public:
    explicit CustomPlugin(QObject *parent = nullptr);

    static QGCCorePlugin *instance();

    // Overrides from QGCCorePlugin

    QGCOptions *options() final { return _options; }
    void paletteOverride(const QString &colorName, QGCPalette::PaletteColorInfo_t &colorInfo) final;
    /// Attaches the url interceptor that redirects core QML to this overlay's replacements.
    QQmlApplicationEngine *createQmlApplicationEngine(QObject *parent) final;
    /// Detaches the interceptor before the engine goes away.
    void destroyQmlApplicationEngine(QQmlApplicationEngine *qmlEngine) final;

private:
    CustomOptions *_options = nullptr;
    QQmlApplicationEngine *_qmlEngine = nullptr;
    CustomOverrideInterceptor *_urlInterceptor = nullptr;
};

/*===========================================================================*/

/// Redirects `qrc:/qml/<path>` to `qrc:/Custom/qml/<path>` when this overlay ships a replacement,
/// which is how a custom build restyles core QML without editing it.
class CustomOverrideInterceptor : public QQmlAbstractUrlInterceptor
{
public:
    QUrl intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type) final;
};
