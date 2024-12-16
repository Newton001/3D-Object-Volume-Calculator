import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import VTK 1.0

ApplicationWindow {
    visible: true
    width: 1280
    height: 960
    title: "3D Object Volume Calculator"

    VTKWindow {
        id: vtkWindow
    }

    // Title Bar
    Rectangle {
        width: parent.width
        height: 60
        color: "#34495e"
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2c3e50" }
            GradientStop { position: 1.0; color: "#34495e" }
        }
        radius: 5

        Text {
            text: "3D Object Volume Calculator"
            font.pixelSize: 26
            font.bold: true
            color: "#ecf0f1"
            anchors.centerIn: parent
        }
    }

    // Main Layout
    SplitView {
        id: mainView
        width: parent.width
        height: parent.height - 100
        anchors.topMargin: 60
        orientation: Qt.Horizontal

        // Left Panel (Controls)
        Rectangle {
            width: 380
            color: "#ecf0f1"
            border.color: "#bdc3c7"
            border.width: 2
            radius: 10
            anchors.margins: 10

            ScrollView {
                width: parent.width
                height: parent.height

                Column {
                    spacing: 20
                    padding: 20

                    Button {
                        text: "Load Volume File"
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: fileDialog.open()
                    }

                    GroupBox {
                        title: "Processing Parameters"
                        contentItem: Column {
                            spacing: 10
                            padding: 10

                            Label { text: "Lower Threshold" }
                            Slider {
                                id: lowerThresholdSlider
                                from: 0
                                to: 1000
                                value: 50
                            }
                            Label { text: lowerThresholdSlider.value.toFixed(1) }

                            Label { text: "Upper Threshold" }
                            Slider {
                                id: upperThresholdSlider
                                from: 0
                                to: 1000
                                value: 800
                            }
                            Label { text: upperThresholdSlider.value.toFixed(1) }

                            Label { text: "Kernel Size" }
                            Slider {
                                id: kernelSizeSlider
                                from: 1
                                to: 10
                                value: 5
                            }
                            Label { text: kernelSizeSlider.value.toFixed(0) }

                            Label { text: "Smoothing StdDev" }
                            Slider {
                                id: smoothingStdDevSlider
                                from: 0.5
                                to: 5.0
                                value: 1.0
                                stepSize: 0.1
                            }
                            Label { text: smoothingStdDevSlider.value.toFixed(1) }
                        }
                    }

                    Button {
                        text: "Apply Settings"
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: {
                            vtkWindow.updateSettings(
                                        lowerThresholdSlider.value,
                                        upperThresholdSlider.value,
                                        kernelSizeSlider.value,
                                        smoothingStdDevSlider.value
                                        )
                        }
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
            anchors.margins: 10

            Column {
                spacing: 20
                anchors.centerIn: parent

                Text {
                    text: "3D Model Properties"
                    font.pixelSize: 22
                    color: "#2c3e50"
                }

                ListView {
                    id: parameterList
                    width: 600
                    height: 400
                    spacing: 2
                    model: ListModel { id: listModel }
                    delegate: Row {
                        Rectangle {
                            width: parameterList.width / 2
                            height: 40
                            border.color: "#bdc3c7"
                            color: index % 2 === 0 ? "#f8f9fa" : "#ecf0f1"
                            Text {
                                text: model.parameter
                                anchors.centerIn: parent
                                font.pixelSize: 16
                                color: "#2c3e50"
                            }
                        }
                        Rectangle {
                            width: parameterList.width / 2
                            height: 40
                            border.color: "#bdc3c7"
                            color: index % 2 === 0 ? "#f8f9fa" : "#ecf0f1"
                            Text {
                                text: model.value
                                anchors.centerIn: parent
                                font.pixelSize: 16
                                color: "#2c3e50"
                            }
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select Volume File"
        folder: shortcuts.home
        nameFilters: ["NIfTI Files (*.nii *.nii.gz)", "VTK Files (*.vtk)", "All Files (*)"]
        onAccepted: {
            statusText.text = "Loading file: " + fileDialog.fileUrl;
            vtkWindow.loadFile(fileDialog.fileUrl.toString().substring(8));
        }
    }

    // Status Bar
    Rectangle {
        width: parent.width
        height: 40
        color: "#34495e"
        border.color: "#bdc3c7"
        border.width: 1
        anchors.bottom: parent.bottom

        Text {
            id: statusText
            text: "Ready"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 16
            color: "#ecf0f1"
        }
    }

    Connections {
        target: vtkWindow

        onDimensionsChanged: refreshModel()
        onVolumeChanged: refreshModel()
        onModelLoadedChanged: refreshModel()
    }

    function refreshModel() {
        listModel.clear();
        listModel.append({ parameter: "Width", value: vtkWindow.width });
        listModel.append({ parameter: "Height", value: vtkWindow.height });
        listModel.append({ parameter: "Depth", value: vtkWindow.depth });
        listModel.append({ parameter: "Volume", value: vtkWindow.volumer });
        listModel.append({ parameter: "Surface Area", value: vtkWindow.surfaceArea || "Patient T03" });
        listModel.append({ parameter: "File Name", value: vtkWindow.fileName });
        listModel.append({ parameter: "Zoom Level", value: "N/A" });
        listModel.append({ parameter: "Model Loaded", value: vtkWindow.modelLoaded ? "Yes" : "No" });
    }
}

