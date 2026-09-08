#include "CustomPlugin.h"

#include <QtCore/QApplicationStatic>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtQml/QQmlApplicationEngine>

Q_APPLICATION_STATIC(CustomPlugin, _customPluginInstance);

namespace {

/// One palette entry: the four cells QGCPalette tracks per color name.
struct SkinColor {
    const char *darkEnabled;
    const char *darkDisabled;
    const char *lightEnabled;
    const char *lightDisabled;
};

/// Dark theme is the design target: near-black neutral chrome, a single blue
/// accent, and status colors that stay legible over video. The light theme is a
/// plain high-contrast counterpart for outdoor use, sharing the same accent.
///
/// Names must match the DECLARE_QGC_* entries in QGCPalette::_buildMap(); an
/// unrecognized name here is silently ignored, so keep the two in sync.
const QHash<QString, SkinColor> &skinColors()
{
    static const QHash<QString, SkinColor> colors = {
        //                            dark enabled  dark disabled  light enabled  light disabled
        { "window",                 { "#101317",    "#101317",     "#ffffff",     "#ffffff"   } },
        // Alpha-prefixed: chrome floats over the map/video rather than boxing it in.
        { "windowTransparent",      { "#c4101317",  "#c4101317",   "#e6ffffff",   "#e6ffffff" } },
        { "windowShade",            { "#1a1e24",    "#1a1e24",     "#f2f4f7",     "#f2f4f7"   } },
        { "windowShadeDark",        { "#0a0c0f",    "#0a0c0f",     "#e4e7ec",     "#e4e7ec"   } },
        { "windowShadeLight",       { "#272c34",    "#272c34",     "#d0d5dd",     "#d0d5dd"   } },
        { "text",                   { "#ffffff",    "#6b7280",     "#101317",     "#98a2b3"   } },
        { "warningText",            { "#ff4d4f",    "#ff4d4f",     "#d92d20",     "#d92d20"   } },
        { "button",                 { "#1f242b",    "#171b21",     "#ffffff",     "#f2f4f7"   } },
        { "buttonBorder",           { "#2e353f",    "#232830",     "#d0d5dd",     "#e4e7ec"   } },
        { "buttonText",             { "#e6e9ed",    "#6b7280",     "#101317",     "#98a2b3"   } },
        { "buttonHighlight",        { "#2e8cff",    "#2a3038",     "#2e8cff",     "#e4e7ec"   } },
        { "buttonHighlightText",    { "#ffffff",    "#8a9099",     "#ffffff",     "#98a2b3"   } },
        { "primaryButton",          { "#2e8cff",    "#24303f",     "#2e8cff",     "#c7d7ee"   } },
        { "primaryButtonText",      { "#ffffff",    "#8a9099",     "#ffffff",     "#ffffff"   } },
        { "textField",              { "#1a1e24",    "#15181d",     "#ffffff",     "#f2f4f7"   } },
        { "textFieldText",          { "#ffffff",    "#6b7280",     "#101317",     "#98a2b3"   } },
        // Stock QGC leaves this fully transparent; DJI's top bar is a scrim that
        // keeps white indicator text readable over bright sky.
        { "toolbarBackground",      { "#cc0f1216",  "#cc0f1216",   "#ccffffff",   "#ccffffff" } },
        { "groupBorder",            { "#2a313a",    "#21262d",     "#d0d5dd",     "#e4e7ec"   } },
        { "toolStripHoverColor",    { "#2a313a",    "#1f242b",     "#e4e7ec",     "#f2f4f7"   } },
        { "missionItemEditor",      { "#1a1e24",    "#1a1e24",     "#f2f4f7",     "#f2f4f7"   } },
        { "mapButton",              { "#101317",    "#101317",     "#ffffff",     "#ffffff"   } },
        { "mapButtonHighlight",     { "#2e8cff",    "#2e8cff",     "#2e8cff",     "#2e8cff"   } },
        { "mapIndicator",           { "#2e8cff",    "#2e8cff",     "#2e8cff",     "#2e8cff"   } },
        { "mapIndicatorChild",      { "#1f5fb0",    "#1f5fb0",     "#1f5fb0",     "#1f5fb0"   } },
        { "colorGreen",             { "#22c55e",    "#22c55e",     "#15803d",     "#15803d"   } },
        { "colorYellow",            { "#facc15",    "#facc15",     "#a16207",     "#a16207"   } },
        { "colorYellowGreen",       { "#a3e635",    "#a3e635",     "#4d7c0f",     "#4d7c0f"   } },
        { "colorOrange",            { "#ff8a3d",    "#ff8a3d",     "#c2410c",     "#c2410c"   } },
        { "colorRed",               { "#ff4d4f",    "#ff4d4f",     "#d92d20",     "#d92d20"   } },
        { "colorGrey",              { "#9ba3ae",    "#9ba3ae",     "#667085",     "#667085"   } },
        { "colorBlue",              { "#2e8cff",    "#2e8cff",     "#1565d8",     "#1565d8"   } },
        { "alertBackground",        { "#facc15",    "#facc15",     "#facc15",     "#facc15"   } },
        { "alertBorder",            { "#b08900",    "#b08900",     "#b08900",     "#b08900"   } },
        { "alertText",              { "#101317",    "#101317",     "#101317",     "#101317"   } },
        // Stock QGC renders these as plain text; color-coding matches how DJI
        // signals check state at a glance.
        { "statusFailedText",       { "#ff4d4f",    "#6b7280",     "#d92d20",     "#98a2b3"   } },
        { "statusPassedText",       { "#22c55e",    "#6b7280",     "#15803d",     "#98a2b3"   } },
        { "statusPendingText",      { "#facc15",    "#6b7280",     "#a16207",     "#98a2b3"   } },
        { "modifiedParamValue",     { "#ff8a3d",    "#ff8a3d",     "#c2410c",     "#c2410c"   } },
        // Non-themed and single-value roles: same value across both themes.
        { "toolStripFGColor",       { "#ffffff",    "#6b7280",     "#ffffff",     "#6b7280"   } },
        { "photoCaptureButtonColor",{ "#ffffff",    "#6b7280",     "#ffffff",     "#6b7280"   } },
        { "videoCaptureButtonColor",{ "#ff4d4f",    "#7a3b3d",     "#ff4d4f",     "#7a3b3d"   } },
        { "brandingBlue",           { "#2e8cff",    "#2e8cff",     "#2e8cff",     "#2e8cff"   } },
        { "mapMissionTrajectory",   { "#2e8cff",    "#2e8cff",     "#2e8cff",     "#2e8cff"   } },
    };

    return colors;
}

} // namespace

CustomFlyViewOptions::CustomFlyViewOptions(CustomOptions *options, QObject *parent)
    : QGCFlyViewOptions(options, parent)
{
}

/*===========================================================================*/

CustomOptions::CustomOptions(QObject *parent)
    : QGCOptions(parent)
    , _flyViewOptions(new CustomFlyViewOptions(this, this))
{
}

/*===========================================================================*/

CustomPlugin::CustomPlugin(QObject *parent)
    : QGCCorePlugin(parent)
    , _options(new CustomOptions(this))
{
}

QGCCorePlugin *CustomPlugin::instance()
{
    return _customPluginInstance();
}

QQmlApplicationEngine *CustomPlugin::createQmlApplicationEngine(QObject *parent)
{
    _qmlEngine = QGCCorePlugin::createQmlApplicationEngine(parent);

    _urlInterceptor = new CustomOverrideInterceptor();
    _qmlEngine->addUrlInterceptor(_urlInterceptor);

    return _qmlEngine;
}

void CustomPlugin::destroyQmlApplicationEngine(QQmlApplicationEngine *qmlEngine)
{
    if (qmlEngine && (qmlEngine == _qmlEngine)) {
        qmlEngine->removeUrlInterceptor(_urlInterceptor);
        delete _urlInterceptor;
        _urlInterceptor = nullptr;
        _qmlEngine = nullptr;
    }

    QGCCorePlugin::destroyQmlApplicationEngine(qmlEngine);
}

void CustomPlugin::paletteOverride(const QString &colorName, QGCPalette::PaletteColorInfo_t &colorInfo)
{
    const auto it = skinColors().constFind(colorName);
    if (it == skinColors().constEnd()) {
        return;
    }

    colorInfo[QGCPalette::Dark][QGCPalette::ColorGroupEnabled]   = QColor(QLatin1String(it->darkEnabled));
    colorInfo[QGCPalette::Dark][QGCPalette::ColorGroupDisabled]  = QColor(QLatin1String(it->darkDisabled));
    colorInfo[QGCPalette::Light][QGCPalette::ColorGroupEnabled]  = QColor(QLatin1String(it->lightEnabled));
    colorInfo[QGCPalette::Light][QGCPalette::ColorGroupDisabled] = QColor(QLatin1String(it->lightDisabled));
}

/*===========================================================================*/

QUrl CustomOverrideInterceptor::intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type)
{
    switch (type) {
    case QQmlAbstractUrlInterceptor::QmlFile:
    case QQmlAbstractUrlInterceptor::UrlString:
        if (url.scheme() == QStringLiteral("qrc")) {
            const QString overrideRes = QStringLiteral(":/Custom%1").arg(url.path());
            if (QFile::exists(overrideRes)) {
                QUrl result;
                result.setScheme(QStringLiteral("qrc"));
                result.setPath(overrideRes.mid(1));
                return result;
            }
        }
        break;
    default:
        break;
    }

    return url;
}
