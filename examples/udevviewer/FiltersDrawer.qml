import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Drawer {
  id: filtersDrawer
  edge: Qt.RightEdge
  width: 480
  height: parent.height
  padding: 0

  background: Rectangle {
    color: Material.backgroundColor
    radius: 0
  }
  Material.elevation: 4

  property var filtersData

  function normalize(obj)
  {
    // Normalize scalars
    obj.subsystem     = obj.subsystem     || "";
    obj.devtype       = obj.devtype       || "";
    obj.sysname       = obj.sysname       || "";
    obj.devnode       = obj.devnode       || "";
    obj.syspathPrefix = obj.syspathPrefix || "";

    // Normalize lists
    obj.actions = Array.isArray(obj.actions) ? obj.actions.slice() : [];
    obj.tags    = Array.isArray(obj.tags)    ? obj.tags.slice()    : [];

    // Normalize maps
    obj.properties      = (obj.properties      && typeof obj.properties      === "object") ? obj.properties      : {};
    obj.sysattrs        = (obj.sysattrs        && typeof obj.sysattrs        === "object") ? obj.sysattrs        : {};
    obj.nomatchSysattrs = (obj.nomatchSysattrs && typeof obj.nomatchSysattrs === "object") ? obj.nomatchSysattrs : {};
  }

  function populateUI(obj)
  {
    normalize(obj);

    // Populate UI
    subsystem.text     = obj.subsystem;
    devtype.text       = obj.devtype;
    sysname.text       = obj.sysname;
    devnode.text       = obj.devnode;
    syspathPrefix.text = obj.syspathPrefix;
    tags.text          = obj.tags.join(", ");

    propertiesEditor.populateUi(obj.properties);
    sysattributesEditor.populateUi(obj.sysattrs);
    nomatchSysattributesEditor.populateUi(obj.nomatchSysattrs);

    Qt.callLater(function() {
      for (let i = 0; i < actionsRepeater.count; ++i) {
        const item = actionsRepeater.itemAt(i)
        if (!item) {
          continue
        }
        const name = actionsRepeater.model[i]
        item.checked = obj.actions.indexOf(name) >= 0
      }
    })
  }

  function clearUI()
  {
    populateUI(({}));
  }

  function getFilters(obj)
  {
    normalize(obj)

    obj.subsystem     = subsystem.text;
    obj.devtype       = devtype.text;
    obj.sysname       = sysname.text;
    obj.devnode       = devnode.text;
    obj.syspathPrefix = syspathPrefix.text;

    obj.tags = (tags.text || "")
    .split(",")
    .map(s => s.trim())
    .filter(Boolean);

    const actions = [];
    for (let i = 0; i < actionsRepeater.count; ++i)
    {
      const item = actionsRepeater.itemAt(i);
      if (item && item.checked) {
        actions.push(item.text);
      }
    }
    obj.actions = actions;

    obj.properties      = propertiesEditor.getFilters();
    obj.sysattrs        = sysattributesEditor.getFilters();
    obj.nomatchSysattrs = nomatchSysattributesEditor.getFilters();
  }

  onOpened: {
    filtersData = JSON.parse(JSON.stringify(filtersModel.value || {}));
    populateUI(filtersData)
  }

  contentItem: ColumnLayout {
    anchors.fill: parent

    ScrollView {
      id: scrollView
      Layout.fillWidth: true
      Layout.fillHeight: true
      contentWidth: availableWidth
      clip: true

      Pane {
        id: pad
        width: scrollView.availableWidth > 0 ? scrollView.availableWidth : scrollView.width
        implicitWidth: width
        padding: 16

        contentItem: ColumnLayout {
          spacing: 12
          Label {
            text: "Filters";
            font.pixelSize: 18;
            font.bold: true
          }

          TextField {
            id: subsystem
            Layout.fillWidth: true
            placeholderText: "subsystem (e.g. usb, block, net)"
          }

          TextField {
            id: devtype
            Layout.fillWidth: true
            placeholderText: "devtype (DEVTYPE)"
          }

          TextField {
            id: sysname
            Layout.fillWidth: true
            placeholderText: "sysname (e.g. 1-2, sda, eth0)"
          }

          TextField {
            id: devnode
            Layout.fillWidth: true
            placeholderText: "devnode (e.g. /dev/sda, /dev/bus/usb/001/002)"
          }

          TextField {
            id: syspathPrefix
            Layout.fillWidth: true
            placeholderText: "syspath prefix (startsWith)"
          }

          GroupBox {
            title: "Actions"
            Layout.fillWidth: true
            RowLayout {
              spacing: 8
              Repeater {
                id: actionsRepeater
                model: ["add","remove","change","move","bind"]
                delegate: CheckBox {
                  text: modelData
                }
              }
            }
          }

          TextField {
            id: tags
            Layout.fillWidth: true
            placeholderText: "tags (comma-separated)"
          }

          GroupBox {
            title: "Properties (libudev NAME=VALUE)"
            Layout.fillWidth: true
            KeyValueEditor {
              id: propertiesEditor
              Layout.fillWidth: true
            }
          }

          GroupBox {
            title: "Sysattrs (sysfs attributes)"
            Layout.fillWidth: true
            KeyValueEditor {
              id: sysattributesEditor
              Layout.fillWidth: true
            }
          }

          GroupBox {
            title: "Negative Sysattrs (key ≠ value)"
            Layout.fillWidth: true
            KeyValueEditor {
              id: nomatchSysattributesEditor
              Layout.fillWidth: true
            }
          }
        }
      }
    }

    // Footer
    Pane {
      Layout.fillWidth: true
      padding: 12

      RowLayout {
        anchors.fill: parent
        spacing: 8

        Button {
          text: "Cancel";
          onClicked: filtersDrawer.close()
        }

        // spacer
        Item {
          Layout.fillWidth: true
        }

        Button {
          text: "Clear"
          onClicked: {
            clearUI();
          }
        }

        Button {
          text: "Apply"
          highlighted: true
          onClicked: {
            getFilters(filtersData);
            filtersModel.setValue(filtersData);
            filtersDrawer.close();
          }
        }
      }
    }
  }
}
