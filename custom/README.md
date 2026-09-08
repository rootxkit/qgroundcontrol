# DJI-Style Skin

A QGroundControl custom build overlay that restyles the app in the style of DJI
Pilot 2: near-black neutral chrome, a single blue accent (`#2e8cff`), status
colors tuned to stay legible over video, and a compact Fly view overlay.

Vehicle, firmware, mission and link behavior are untouched. The one functional
change is that QGC's instrument panel is hidden — the telemetry strip takes its
place. Revert by returning `true` from
`CustomFlyViewOptions::showInstrumentPanel()`.

## Building

`QGC_CUSTOM_DIR` defaults to `custom`, so the presence of this directory is
enough — no extra flags:

```bash
just configure
just build
```

CMake confirms the overlay was picked up:

```
QGC: Custom build directory detected: custom
QGC: Adding DJI-style UI skin
```

To build stock QGC instead, point the overlay elsewhere:

```bash
cmake -B build -DQGC_CUSTOM_DIR=no-such-dir
```

## Layout

| Path | Purpose |
| --- | --- |
| `cmake/CustomOverrides.cmake` | Branding and feature toggles, read before `project()` |
| `CMakeLists.txt` | Exports `CUSTOM_SOURCES` / `CUSTOM_DEFINITIONS` to `src/CMakeLists.txt` |
| `src/CustomPlugin.{h,cc}` | Core plugin; the palette table lives here |
| `src/FlyViewCustomLayer.qml` | Fly view overlay: telemetry strip + attitude cluster |

`CUSTOMHEADER` / `CUSTOMCLASS` in `CMakeLists.txt` are how `src/API/QGCCorePlugin.cc`
resolves `QGCCorePlugin::instance()` to `CustomPlugin`.

## Changing colors

Edit the `skinColors()` table in `src/CustomPlugin.cc`. Keys must match the
`DECLARE_QGC_*_COLOR` names in `QGCPalette::_buildMap()`
(`src/QmlControls/QGCPalette.cc`) — an unrecognized key is silently ignored, so
verify against that list when adding entries.

Five roles deliberately keep their stock values: `brandingPurple`,
`mapWidgetBorderLight`, `mapWidgetBorderDark`, `surveyPolygonInterior`,
`surveyPolygonTerrainCollision`.

## Overriding core QML

`src/FlyViewCustomLayer.qml` is aliased in `CMakeLists.txt` onto the core
resource path it replaces. At load time `CustomOverrideInterceptor` rewrites
`qrc:/qml/<path>` to `qrc:/Custom/qml/<path>` whenever this overlay ships a
file at that path, so core QML is substituted rather than edited. Adding another
override means one entry in `CUSTOM_QML_OVERRIDES` plus its `QT_RESOURCE_ALIAS`.

An override must keep the property contract the core file has with its parent.
`FlyViewCustomLayer` takes `parentToolInsets` and `mapControl` and must publish
`totalToolInsets`; those insets are what stop QGC's own chrome from drawing
underneath the overlay.

## Not yet done

The right-edge control rail from the Pilot 2 layout is not implemented. QGC
already has a vertical icon rail — `FlyViewToolStrip` — but it is anchored top
left by `src/FlyView/FlyViewWidgetLayer.qml`. Moving it means overriding that
file, which is large and changes often upstream, so it is a deliberate
maintenance decision rather than an oversight.
