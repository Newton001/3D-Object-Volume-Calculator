import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3


ApplicationWindow {
    visible: true
    width: 1280
    height: 960
    title: "3D Object Volume Calculator"

    // Title Bar
    Rectangle {
        width: parent.width
        height: 50
        color: "#2c3e50"
        Text {
            text: "3D Object Volume Calculator"
            font.pixelSize: 24
            font.bold: true
            color: "#ecf0f1"
            anchors.centerIn: parent
        }
    }

    // Main Layout
    SplitView {
        id: mainView
        width: parent.width
        height: parent.height - 90
        anchors.topMargin: 50
        orientation: Qt.Horizontal

        // Left Panel (Controls)
        Rectangle {
            width: 350
            color: "#ecf0f1"
            border.color: "#bdc3c7"
            border.width: 2
            radius: 10

            Column {
                spacing: 20
                padding: 20
                anchors.fill: parent

                // Load Files Button
                Button {
                    text: "Load Files"
                    font.pixelSize: 16
                    background: Rectangle {
                        color: "#3498db"
                        radius: 5
                    }
                    contentItem: Text {
                        text: "Load Files"
                        font.pixelSize: 16
                        color: "white"
                        anchors.centerIn: parent
                    }
                    onClicked: fileDialog.open()
                }

                // Smoothing Checkbox
                CheckBox {
                    id: smoothingCheck
                    text: "Enable Smoothing"
                    font.pixelSize: 16
                }

                // Threshold Slider
                Column {
                    spacing: 10
                    Text {
                        text: "Threshold: " + thresholdSlider.value
                        font.pixelSize: 16
                        color: "#2c3e50"
                    }
                    Slider {
                        id: thresholdSlider
                        from: 0
                        to: 100
                        value: 50
                        width: 300
                        onValueChanged: console.log("Slider Value:", value)
                    }
                }

                // Start Processing Button
                Button {
                    text: "Start Processing"
                    font.pixelSize: 16
                    background: Rectangle {
                        color: "#2ecc71"
                        radius: 5
                    }
                    contentItem: Text {
                        text: "Start Processing"
                        font.pixelSize: 16
                        color: "white"
                        anchors.centerIn: parent
                    }
                    onClicked: console.log("Processing started")
                }
            }
        }

        // Right Panel (Visualization)
        Rectangle {
            color: "white"
            border.color: "#bdc3c7"
            border.width: 2
            radius: 10
            Text {
                text: "3D Model Viewer (Placeholder)"
                anchors.centerIn: parent
                font.pixelSize: 20
                color: "#7f8c8d"
            }
        }
    }

    // File Dialog for File Loading
    FileDialog {
        id: fileDialog
        title: "Select Files or Folder"
        folder: shortcuts.home
        nameFilters: ["Images (*.png *.jpg *.jpeg *.dcm)", "All Files (*)"]
        onAccepted: {
            console.log("Selected file(s):", fileDialog.fileUrls)
            mainWindow.loadFiles(fileDialog.fileUrls) // Send fileUrls directly to C++
        }
    }


    // Status Bar
    Rectangle {
        width: parent.width
        height: 40
        color: "#ecf0f1"
        border.color: "#bdc3c7"
        border.width: 1
        anchors.bottom: parent.bottom
        z: 2

        Text {
            id: statusText
            text: "Ready"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 16
            color: "#2c3e50"
        }
    }
}
