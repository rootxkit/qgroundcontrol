# ============================================================================
# QGroundControl DJI-Style Skin
# Visual-only custom build overlay: palette override, no behavioral changes.
# ============================================================================

message(STATUS "QGC: Adding DJI-style UI skin")

# ----------------------------------------------------------------------------
# QML Overrides
# ----------------------------------------------------------------------------
# Each file is aliased to the core resource path it replaces. CustomOverrideInterceptor
# rewrites qrc:/qml/<path> to qrc:/Custom/qml/<path> at load time, so core QML is
# substituted rather than edited.
set(CUSTOM_QML_OVERRIDES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FlyViewCustomLayer.qml"
)
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/FlyViewCustomLayer.qml"
    PROPERTIES QT_RESOURCE_ALIAS "QGroundControl/FlyView/FlyViewCustomLayer.qml"
)
qt_add_resources(${CMAKE_PROJECT_NAME} custom_qml_overrides
    PREFIX "/Custom/qml"
    FILES ${CUSTOM_QML_OVERRIDES}
)

set(CUSTOM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CustomPlugin.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CustomPlugin.h
    PARENT_SCOPE
)

set(CUSTOM_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PARENT_SCOPE
)

set(CUSTOM_DEFINITIONS
    QGC_CUSTOM_BUILD
    CUSTOMHEADER="CustomPlugin.h"
    CUSTOMCLASS=CustomPlugin
    PARENT_SCOPE
)

set(CUSTOM_QT_COMPONENTS
    Core
    Qml
    PARENT_SCOPE
)
