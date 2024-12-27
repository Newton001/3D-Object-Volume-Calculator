import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtCharts 2.15

ApplicationWindow {
    visible: true
    width: 1280
    height: 960
    title: "3D Object Volume Calculator"
    color: "#f5f5f5" // Light background for better contrast

    // Title Bar
    Rectangle {
        width: parent.width
        height: 70
        color: "#34495e"
        radius: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2c3e50" }
            GradientStop { position: 1.0; color: "#34495e" }
        }
        Text {
            text: "3D Object Volume Calculator"
            font.pixelSize: 30
            font.bold: true
            font.family: "Roboto"
            color: "#ecf0f1"
            anchors.centerIn: parent
        }
    }

    // Main Layout
    SplitView {
        id: mainView
        width: parent.width
        height: parent.height - 130
        anchors.topMargin: 70
        orientation: Qt.Horizontal

        // Left Panel (Controls)
        Rectangle {
            width: 400
            color: "#ffffff"
            border.color: "#bdc3c7"
            border.width: 2
            radius: 10
            anchors.margins: 20
            ScrollView {
                width: parent.width
                height: parent.height
                clip: true

                Column {
                    spacing: 20
                    padding: 20

                    Button {
                        text: "Load Volume File"
                        width: parent.width - 40
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 18
                        background: Rectangle {
                            color: "#3498db"
                            radius: 10
                        }
                        onClicked: fileDialog.open()
                    }

                    Button {
                        text: "Open 2D Slice Viewer"
                        width: parent.width - 40
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 18
                        background: Rectangle {
                            color: "#3498db"
                            radius: 10
                        }
                        onClicked: {
                            fileLoader.visualizeSlicesWithSlider(); // Call the C++ function
                        }
                    }

                    GroupBox {
                        title: "Processing Parameters"
                        font.pixelSize: 16
                        contentItem: Column {
                            spacing: 15
                            padding: 15

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
                        width: parent.width - 40
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 18
                        background: Rectangle {
                            color: "#2ecc71"
                            radius: 10
                        }
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

        // Right Panel (3D Viewer, Status, and Plot)
        Rectangle {
            width: parent.width
            height: parent.height
            color: "#ffffff"
            border.color: "#bdc3c7"
            border.width: 2
            radius: 10
            anchors.margins: 20

            Column {
                spacing: 20
                anchors.fill: parent
                anchors.margins: 20

                Text {
                    text: "3D Model Properties"
                    font.pixelSize: 22
                    font.bold: true
                    color: "#34495e"
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

                Text {
                    text: "Signal Plot"
                    font.pixelSize: 22
                    font.bold: true
                    color: "#34495e"
                }

                ChartView {
                    id: intensityChart
                    width: parent.width
                    height: 400
                    legend.visible: false
                    antialiasing: true

                    ValueAxis {
                        id: xAxis
                        titleText: "Slice Index"
                        min: 0
                        max: 100 // Dynamic update based on voxel data
                    }

                    ValueAxis {
                        id: yAxis
                        titleText: "Voxel Count"
                        min: 0
                        max: 10000 // Adjust this based on expected voxel range
                    }

                    LineSeries {
                        id: lineSeries
                        axisX: xAxis
                        axisY: yAxis
                        name: "Voxel Count Data"
                        color: "blue"
                        onClicked: (point) => {
                                       console.log(`Slice Index: ${point.x}, Voxel Count: ${point.y}`);
                                       selectedPointLabel.text = `Slice Index: ${point.x}, Voxel Count: ${point.y}`;
                                   }
                    }

                    // Enable zooming and panning
                    MouseArea {
                        anchors.fill: parent

                        onWheel: (event) => {
                                     const zoomFactor = event.angleDelta.y > 0 ? 0.9 : 1.1;
                                     const xCenter = (xAxis.max + xAxis.min) / 2;
                                     const yCenter = (yAxis.max + yAxis.min) / 2;

                                     xAxis.min = xCenter - (xCenter - xAxis.min) * zoomFactor;
                                     xAxis.max = xCenter + (xAxis.max - xCenter) * zoomFactor;

                                     yAxis.min = yCenter - (yCenter - yAxis.min) * zoomFactor;
                                     yAxis.max = yCenter + (yAxis.max - yCenter) * zoomFactor;
                                 }
                    }

                    // Label to display selected point details
                    Rectangle {
                        id: selectedPointLabelContainer
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 10
                        width: 300
                        height: 40
                        color: "#ffffff"
                        border.color: "#bdc3c7"
                        border.width: 1
                        radius: 5

                        Text {
                            id: selectedPointLabel
                            anchors.centerIn: parent
                            text: "Select a point for details"
                            font.pixelSize: 16
                            color: "#34495e"
                        }
                    }
                }

                Button {
                    text: "Update Plot"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 18
                    background: Rectangle {
                        color: "#e74c3c"
                        radius: 10
                    }
                    onClicked: {
                        lineSeries.clear();
                        for (let i = 0; i <= 100; i += 10) {
                            lineSeries.append(i, Math.random() * 100);
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
            if (fileDialog.fileUrl && fileDialog.fileUrl !== "") {
                statusText.text = "Loading file: " + fileDialog.fileUrl;
                vtkWindow.loadFile(fileDialog.fileUrl.toString().substring(8));
            } else {
                statusText.text = "Invalid file selected.";
            }
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

    function refreshModel() {
        console.log("Refreshing model...");
        console.log("Width:", vtkWindow.width);
        console.log("Height:", vtkWindow.height);
        console.log("Depth:", vtkWindow.depth);

        listModel.clear();
        listModel.append({ parameter: "Width", value: vtkWindow.width });
        listModel.append({ parameter: "Height", value: vtkWindow.height });
        listModel.append({ parameter: "Depth", value: vtkWindow.depth });
        listModel.append({ parameter: "Volume", value: vtkWindow.volume });
        listModel.append({ parameter: "Surface Area", value: vtkWindow.surfaceArea });
        listModel.append({ parameter: "File Name", value: "Patient T03" });
        listModel.append({ parameter: "Zoom Level", value: "N/A" });
        listModel.append({ parameter: "Model Loaded", value: vtkWindow.modelLoaded ? "Yes" : "No" });
    }

    Connections {
        target: vtkWindow

        function onDimensionsChanged() {
            refreshModel();
        }

        function onVolumeChanged() {
            refreshModel();
        }

        function onModelLoadedChanged() {
            refreshModel();
        }

        function onIntensityDataChanged(data) {
            console.log("Received intensity data:", data.length, "points.");
            if (data.length > 0) {
                console.log("First data point:", data[0].first, ",", data[0].second);
            }
        }

        function onVoxelDataUpdated(sliceIndices, voxelCounts) {
            console.log("Voxel data updated signal received");
            lineSeries.clear();
            xAxis.max = Math.max(...sliceIndices);
            yAxis.max = Math.max(...voxelCounts);
            for (let i = 0; i < sliceIndices.length; i++) {
                lineSeries.append(sliceIndices[i], voxelCounts[i]);
            }
        }
    }

}
