// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material


ColumnLayout {
  id: editor
  anchors.fill: parent
  spacing: 8

  ListModel {
    id: rows
  }

  function populateUi(data)
  {
    rows.clear()

    if (!data) {
      return
    }

    const keys = Object.keys(data)
    for (let i = 0; i < keys.length; ++i)
    {
      const key = String(keys[i])
      const value = String(data[keys[i]])
      rows.append({ key: key, value: value })
    }
  }

  function getFilters()
  {
    const data = {}

    for (let i = 0; i < rows.count; ++i)
    {
      const key = String(rows.get(i).key || "").trim()
      const value = String(rows.get(i).value || "")
      if (!key.length || !value.length) {
        continue
      }
      data[key] = value
    }

    return data
  }

  ListView {
    id: list
    Layout.fillWidth: true
    Layout.preferredHeight: Math.min(contentHeight, 360)
    clip: true
    spacing: 8
    model: rows

    delegate: RowLayout {
      width: list.width
      spacing: 8

      TextField {
        Layout.fillWidth: true
        text: model.key
        placeholderText: "Key"
        onTextEdited: rows.set(index, { key: text, value: model.value })
      }

      TextField {
        Layout.fillWidth: true
        text: model.value
        placeholderText: "Value"
        onTextEdited: rows.set(index, { key: model.key, value: text })
      }

      ToolButton {
        text: "✕"
        Accessible.name: "Remove"
        focusPolicy: Qt.ClickFocus
        onClicked: rows.remove(index)
      }
    }
  }

  RowLayout {
    Layout.fillWidth: true
    spacing: 8
    Button {
      text: "Add"
      focusPolicy: Qt.ClickFocus
      activeFocusOnTab: false
      onClicked: rows.append({ key: "", value: "" })
    }
    Item { Layout.fillWidth: true }
  }
}
