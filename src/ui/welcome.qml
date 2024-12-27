import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 1280
    height: 960
    title: "Welcome to 3D Medical Volume Calculator"

    // Welcome Screen
    Rectangle {
        id: welcomeScreen
        width: parent.width
        height: parent.height
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1e3d59" }
            GradientStop { position: 1.0; color: "#3b6978" }
        }

        Column {
            spacing: 20
            anchors.centerIn: parent

            // Main Title
            Text {
                text: "Welcome to Your 3D Medical Image Volume Calculator"
                font.pixelSize: 38
                font.bold: true
                color: "#ffffff"
                horizontalAlignment: Text.AlignHCenter
            }

            // Subtitle
            Text {
                text: "Click anywhere to continue..."
                font.pixelSize: 20
                color: "#d3e0ea"
                horizontalAlignment: Text.AlignHCenter
            }

            // Image Row
            Row {
                spacing: 30
                anchors.horizontalCenter: parent.horizontalCenter

                // Image 1
                Rectangle {
                    width: 220
                    height: 200
                    radius: 15
                    color: "#ffffff"
                    border.color: "#c0c7d6"
                    border.width: 2
                    Image {
                        source: "images/scan1.jpg"
                        anchors.centerIn: parent
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        fillMode: Image.PreserveAspectFit
                    }
                }

                // Image 2
                Rectangle {
                    width: 220
                    height: 200
                    radius: 15
                    color: "#ffffff"
                    border.color: "#c0c7d6"
                    border.width: 2
                    Image {
                        source: "images/scan2.jpg"
                        anchors.centerIn: parent
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        fillMode: Image.PreserveAspectFit
                    }
                }

                // Image 3
                Rectangle {
                    width: 220
                    height: 200
                    radius: 15
                    color: "#ffffff"
                    border.color: "#c0c7d6"
                    border.width: 2
                    Image {
                        source: "images/image3.jpg"
                        anchors.centerIn: parent
                        width: parent.width * 0.8
                        height: parent.height * 0.8
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }

            // Decorative Line
            Rectangle {
                width: 500
                height: 2
                color: "#d3e0ea"
                radius: 1
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
            }

            // Medical Tagline
            Text {
                text: "Empowering Precision and Insights for Medical Imaging"
                font.pixelSize: 24
                font.italic: true
                color: "#d3e0ea"
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // Transition to Main Screen
        MouseArea {
            anchors.fill: parent
            onClicked: {
                welcomeScreen.visible = false;
                mainLoader.source = "main.qml";
            }
        }
    }

    // Loader for Main Screen
    Loader {
        id: mainLoader
        anchors.fill: parent
    }
}
