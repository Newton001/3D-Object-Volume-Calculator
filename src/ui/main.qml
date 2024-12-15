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

            ScrollView {
                width: parent.width
                height: parent.height
                clip: true
                ScrollBar.vertical: ScrollBar {
                    width: 16
                    policy: ScrollBar.AlwaysOn
                    contentItem: Rectangle {
                        width: 16
                        radius: 8
                        color: "#3498db"
                    }
                    background: Rectangle {
                        width: 16
                        color: "#bdc3c7"
                    }
                    hoverEnabled: true
                    active: true
                }

                Column {
                    spacing: 20
                    padding: 20
                    anchors.fill: parent

                    // Load Volume File Button
                    Button {
                        text: "Load Volume File"
                        font.pixelSize: 16
                        background: Rectangle {
                            color: "#3498db"
                            radius: 5
                        }
                        contentItem: Text {
                            text: "Load Volume File"
                            font.pixelSize: 16
                            color: "white"
                            anchors.centerIn: parent
                        }
                        onClicked: fileDialog.open()
                    }

                    // Pre-processing Options
                    GroupBox {
                        title: "Pre-processing"
                        font.pixelSize: 16
                        contentItem: Column {
                            spacing: 10
                            padding: 10

                            CheckBox {
                                text: "Apply Thresholding"
                                onCheckedChanged: thresholdingEnabled = checked
                            }

                            CheckBox {
                                text: "Apply Smoothing"
                                onCheckedChanged: smoothingEnabled = checked
                            }
                        }
                    }

                    // Volume Calculation Button
                    Button {
                        text: "Calculate Volume"
                        font.pixelSize: 16
                        background: Rectangle {
                            color: "#e74c3c"
                            radius: 5
                        }
                        contentItem: Text {
                            text: "Calculate Volume"
                            font.pixelSize: 16
                            color: "white"
                            anchors.centerIn: parent
                        }
                        onClicked: calculateVolume()
                    }

                    // Export Options
                    GroupBox {
                        title: "Export"
                        font.pixelSize: 16
                        contentItem: Column {
                            spacing: 10
                            padding: 10

                            Button {
                                text: "Export Model"
                                font.pixelSize: 16
                                onClicked: exportModel()
                            }

                            Button {
                                text: "Export Volume Data"
                                font.pixelSize: 16
                                onClicked: exportVolumeData()
                            }
                        }
                    }

                    // Show VTK Window Button
                    Button {
                        text: "Show VTK Window"
                        font.pixelSize: 16
                        background: Rectangle {
                            color: "#2ecc71"
                            radius: 5
                        }
                        contentItem: Text {
                            text: "Show VTK Window"
                            font.pixelSize: 16
                            color: "white"
                            anchors.centerIn: parent
                        }
                        onClicked: vtkWindow.showWindow()
                    }
                }
            }
        }

        // Right Panel (3D Viewer and Status)
        Rectangle {
            color: "white"
            border.color: "#bdc3c7"
            border.width: 2
            radius: 10

            Column {
                spacing: 20
                anchors.centerIn: parent

                Text {
                    text: "3D Model Viewer"
                    font.pixelSize: 20
                    color: "#7f8c8d"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                // 3D Model Visualization Placeholder
                Rectangle {
                    width: 600
                    height: 400
                    color: "#ecf0f1"
                    border.color: "#bdc3c7"
                    border.width: 1
                    radius: 10
                    Text {
                        text: "3D Model Visualization Area"
                        anchors.centerIn: parent
                        font.pixelSize: 18
                        color: "#7f8c8d"
                    }
                }

                // Volume Result Display
                Text {
                    id: volumeText
                    text: "Volume: N/A"
                    font.pixelSize: 18
                    color: "#2c3e50"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    // File Dialog for Selecting a Volume File
    FileDialog {
        id: fileDialog
        title: "Select Volume File"
        folder: shortcuts.home
        nameFilters: ["NIfTI Files (*.nii *.nii.gz)", "VTK Files (*.vtk)", "All Files (*)"]
        onAccepted: {
            console.log("Selected file:", fileDialog.fileUrl);
            statusText.text = "Loading file: " + fileDialog.fileUrl;
            vtkWindow.loadFile(fileDialog.fileUrl.toString().substring(8)); // Remove "file://"
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

    // Helper Variables for Pre-processing Options
    property bool thresholdingEnabled: false
    property bool smoothingEnabled: false

    // Functions for Volume Calculation, Export, and Error Handling
    function calculateVolume() {
        if (!vtkWindow.isFileLoaded()) {
            statusText.text = "Error: No file loaded!";
            return;
        }
        statusText.text = "Calculating Volume...";
        volumeText.text = "Volume: Calculating...";
        // Trigger the volume calculation (this would depend on how your volume is calculated in the VTK application)
    }

    function exportModel() {
        if (!vtkWindow.isFileLoaded()) {
            statusText.text = "Error: No file loaded!";
            return;
        }
        // Add export logic here (e.g., export 3D model to file)
        statusText.text = "Exporting Model...";
    }

    function exportVolumeData() {
        if (!vtkWindow.isFileLoaded()) {
            statusText.text = "Error: No file loaded!";
            return;
        }
        // Add export logic for volume data here (e.g., export volume as a .txt or .csv file)
        statusText.text = "Exporting Volume Data...";
    }
}
