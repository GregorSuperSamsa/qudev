import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts


ApplicationWindow
{
  // Material theme
  readonly property color backgroundColor: "#121212"
  readonly property color accentColor: "#FF5722"
  readonly property color accentColor2: "#9E9E9E"
  Material.theme: Material.Dark
  Material.accent: accentColor
  Material.primary: backgroundColor
  Material.background: backgroundColor

  visible: true
  width: 1100; height: 700
  title: "Qudev Viewer"

  header: ToolBar {
    RowLayout {
      anchors.fill: parent
      spacing: 6

      ToolButton {
        id: scanButton
        enabled: !service.scanning
        onClicked: service.scan()

        contentItem: RowLayout {
          anchors.fill: parent
          anchors.margins: 6
          spacing: 6

          Label {
            text: service.scanning ? qsTr("Scanning devices…") : qsTr("Scan devices")
            Layout.alignment: Qt.AlignVCenter
          }
          BusyIndicator {
            running: service.scanning
            visible: running
            Layout.preferredWidth:  32
            Layout.preferredHeight: 32
            Layout.alignment: Qt.AlignVCenter
          }
        }
      }

      ToolButton {
        id: monitorButton
        onClicked: {
          if (service.monitoring)
            service.stopMonitoring()
          else
            service.startMonitoring()
        }

        contentItem: RowLayout {
          anchors.fill: parent
          anchors.margins: 6
          spacing: 6

          Label {
            text: service.monitoring ? qsTr("Stop monitoring") : qsTr("Start monitoring")
            Layout.alignment: Qt.AlignVCenter
          }

          BusyIndicator {
            running: service.monitoring
            visible: running
            Layout.preferredWidth:  32
            Layout.preferredHeight: 32
            Layout.alignment: Qt.AlignVCenter
          }
        }
      }

      ToolButton {
        contentItem: RowLayout {
          anchors.fill: parent
          anchors.margins: 6
          spacing: 6

          Label {
            text: qsTr("Clear")
            Layout.alignment: Qt.AlignVCenter
          }
        }
        enabled: !service.scanning
        onClicked: deviceModel.clear()
      }

      Item { Layout.fillWidth: true }

      ToolButton {
        contentItem: RowLayout {
          anchors.fill: parent
          anchors.margins: 6
          spacing: 6

          Label {
            text: qsTr("Filters")
            Layout.alignment: Qt.AlignVCenter
          }
        }
        onClicked: filtersDrawer.open()
      }
    }
  }

  TreeView {
    id: treeView
    anchors.fill: parent
    anchors.topMargin: 10
    anchors.bottomMargin: 10
    clip: true

    columnWidthProvider: function(col) {
      return treeView.width - treeView.leftMargin + treeView.rightMargin;
    }

    rowHeightProvider: function(row) {
      return 36;
    }

    selectionModel: ItemSelectionModel {}

    model: deviceModel

    delegate: Item {
      readonly property real itemPadding: 15
      readonly property real itemIndentation: 15

      // Assigned to by TreeView:
      required property TreeView treeView
      required property bool isTreeNode
      required property bool expanded
      required property bool hasChildren
      required property int depth
      required property int row
      required property int column
      required property bool current

      property Animation indicatorAnimation: NumberAnimation {
        target: indicator
        property: "rotation"
        from: expanded ? 0 : 90
        to: expanded ? 90 : 0
        duration: 100
        easing.type: Easing.OutQuart
      }
      TableView.onPooled: indicatorAnimation.complete()
      TableView.onReused: if (current) indicatorAnimation.start()
      onExpandedChanged: indicator.rotation = expanded ? 90 : 0

      function getRowOpacity(row)
      {
        if (row === treeView.currentRow)
          return 0.8;
        else if (row % 2 !== 0)
          return 0.1;

        return 0.3;
      }

      function getRowColor(row)
      {
        if (row === treeView.currentRow)
          return accentColor;
        else if (row % 2 !== 0)
          return accentColor2;

        return backgroundColor;
      }

      Rectangle {
        id: background
        anchors.fill: parent
        color: getRowColor(row)
        opacity: getRowOpacity(row)
      }

      Label {
        id: indicator
        x: itemPadding + (depth * itemIndentation)
        anchors.verticalCenter: parent.verticalCenter
        visible: isTreeNode && hasChildren
        text: "▶"

        TapHandler {
          onSingleTapped: {
            let index = treeView.index(row, column)
            treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
            treeView.toggleExpanded(row)
          }
        }
      }

      Label {
        id: label
        x: itemPadding + (isTreeNode ? (depth + 1) * itemIndentation : 0)
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - itemPadding - x
        clip: true
        text: (model.display ?? "").replace(/\n/g, "; ")
      }
    }
  }

  FiltersDrawer {
    id: filtersDrawer
  }
}
