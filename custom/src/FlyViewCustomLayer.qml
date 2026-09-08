import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlightMap

/// Pilot-2-style flight overlay: a compact telemetry strip centered on the bottom edge and an
/// attitude/compass cluster on the right. This replaces QGC's instrument panel, which
/// CustomFlyViewOptions turns off — without that the two would occupy the same corner.
Item {
    id: _root

    // Screen real estate already claimed by the layers below this one.
    property var parentToolInsets
    property var totalToolInsets:   _toolInsets
    property var mapControl

    readonly property var  _activeVehicle:  QGroundControl.multiVehicleManager.activeVehicle
    readonly property var  _qgcPal:         QGroundControl.globalPalette
    readonly property real _margin:         ScreenTools.defaultFontPixelWidth * 1.5

    /// A single labelled readout: abbreviation above, current value below.
    component TelemetryValue: ColumnLayout {
        id: cell

        property string label
        property var    fact

        spacing: 0

        QGCLabel {
            Layout.alignment:   Qt.AlignHCenter
            text:               cell.label
            font.pointSize:     ScreenTools.smallFontPointSize
            opacity:            0.65
        }

        QGCLabel {
            Layout.alignment:   Qt.AlignHCenter
            text:               cell.fact ? (cell.fact.valueString + " " + cell.fact.units) : qsTr("--")
            font.bold:          true
        }
    }

    QGCToolInsets {
        id: _toolInsets

        leftEdgeTopInset:       parentToolInsets.leftEdgeTopInset
        leftEdgeCenterInset:    parentToolInsets.leftEdgeCenterInset
        leftEdgeBottomInset:    parentToolInsets.leftEdgeBottomInset
        rightEdgeTopInset:      parentToolInsets.rightEdgeTopInset
        rightEdgeCenterInset:   _root.width - attitudeCluster.x + attitudeCluster.extraInset + _margin
        rightEdgeBottomInset:   parentToolInsets.rightEdgeBottomInset
        topEdgeLeftInset:       parentToolInsets.topEdgeLeftInset
        topEdgeCenterInset:     parentToolInsets.topEdgeCenterInset
        topEdgeRightInset:      parentToolInsets.topEdgeRightInset
        bottomEdgeLeftInset:    parentToolInsets.bottomEdgeLeftInset
        bottomEdgeCenterInset:  _root.height - telemetryStrip.y + _margin
        bottomEdgeRightInset:   parentToolInsets.bottomEdgeRightInset
    }

    Rectangle {
        id: telemetryStrip

        anchors.horizontalCenter:   parent.horizontalCenter
        anchors.bottom:             parent.bottom
        anchors.bottomMargin:       _margin + parentToolInsets.bottomEdgeCenterInset
        width:                      telemetryRow.width + (_margin * 3)
        height:                     telemetryRow.height + _margin
        radius:                     height / 3
        color:                      _qgcPal.windowTransparent
        border.color:               _qgcPal.groupBorder
        border.width:               1

        RowLayout {
            id:                 telemetryRow
            anchors.centerIn:   parent
            spacing:            _margin * 1.5

            TelemetryValue {
                label:  qsTr("H")
                fact:   _activeVehicle ? _activeVehicle.altitudeRelative : null
            }

            Rectangle {
                Layout.fillHeight:  true
                Layout.margins:     _margin / 3
                width:              1
                color:              _qgcPal.text
                opacity:            0.2
            }

            TelemetryValue {
                label:  qsTr("D")
                fact:   _activeVehicle ? _activeVehicle.distanceToHome : null
            }

            Rectangle {
                Layout.fillHeight:  true
                Layout.margins:     _margin / 3
                width:              1
                color:              _qgcPal.text
                opacity:            0.2
            }

            TelemetryValue {
                label:  qsTr("HS")
                fact:   _activeVehicle ? _activeVehicle.groundSpeed : null
            }

            Rectangle {
                Layout.fillHeight:  true
                Layout.margins:     _margin / 3
                width:              1
                color:              _qgcPal.text
                opacity:            0.2
            }

            TelemetryValue {
                label:  qsTr("VS")
                fact:   _activeVehicle ? _activeVehicle.climbRate : null
            }
        }
    }

    IntegratedCompassAttitude {
        id: attitudeCluster

        anchors.right:          parent.right
        anchors.rightMargin:    _margin + parentToolInsets.rightEdgeCenterInset
        anchors.verticalCenter: parent.verticalCenter
    }
}
