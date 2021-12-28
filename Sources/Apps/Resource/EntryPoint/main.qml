import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15
import QtMultimedia 5.15
import ZXing 1.0
import org.erturk.face 1.0
import org.erturk.sgdet 1.0
import org.erturk.nucleo 1.0
import org.erturk.todolist 1.0

ApplicationWindow {
    visible: true
    width: 1280
    height: 800
    title: Qt.application.name
    // flags: Qt.FramelessWindowHint


    property var nullPoints: [Qt.point(0,0), Qt.point(0,0), Qt.point(0,0), Qt.point(0,0)]
    property var points: nullPoints

    Nucleo {
        id: threadNucleo
    }

    Timer {
        id: resetInfo
        interval: 2000
    }

    FaceDetectFilter {
        id: faceDetectFilter
        onObjectDetected:
        {
            if((w == 0) || (h == 0))
            {
                smile.visible = false;
            }
            else
            {
                var r = videoOutput.mapNormalizedRectToItem(Qt.rect(x, y, w, h));
                smile.x = r.x;
                smile.y = r.y;
                smile.width = r.width;
                smile.height = r.height;
                smile.visible = true;
            }
        }
    }

    SGDetFilter {
        id: sgDetFilter
        onObjectDetected: {
        }
    }

    VideoFilter {
        id: zxingFilter

        formats: (oneDSwitch.checked ? (ZXing.OneDCodes) : ZXing.None) | (twoDSwitch.checked ? (ZXing.TwoDCodes) : ZXing.None)
        tryRotate: tryRotateSwitch.checked
        tryHarder: tryHarderSwitch.checked

        // callback with parameter 'result', called for every sucessfully processed frame
        // onFoundBarcode: {}

        // callback with parameter 'result', called for every processed frame
        onNewResult: {
            points = result.isValid
                    ? [result.position.topLeft, result.position.topRight, result.position.bottomRight, result.position.bottomLeft]
                    : nullPoints

            if (result.isValid)
                resetInfo.restart()

            if (result.isValid || !resetInfo.running)
                info.text = qsTr("Format: \t %1 \nText: \t %2 \nError: \t %3 \nTime: \t %4 ms").arg(result.formatName).arg(result.text).arg(result.status).arg(result.runTime)

//            console.log(result)
        }
    }

/*
    Camera {
        id: camera

        captureMode: Camera.CaptureViewfinder
        deviceId: QtMultimedia.availableCameras[camerasComboBox.currentIndex] ? QtMultimedia.availableCameras[camerasComboBox.currentIndex].deviceId : ""

        onDeviceIdChanged: {
            focus.focusMode = CameraFocus.FocusContinuous
            focus.focusPointMode = CameraFocus.FocusPointAuto
        }

        onError: console.log("camera error:" + errorString)
    }
*/

   MediaPlayer {
        id: video_cam00
        objectName: "video_cam00"
        autoPlay: true
        source: "gst-pipeline: v4l2src device=/dev/video0 ! image/jpeg, width=1280, height=720, framerate=25/1, format=MJPG ! jpegdec ! qtvideosink"
    }

    ShaderEffect {
        id: videoShader
        property variant src: videoOutput
        property variant source: videoOutput
    }

    /*
    header: TabBar {
        id: tabBarHeader
        currentIndex: mainSwipe.currentIndex
        TabButton {
            text: qsTr("Home")
        }
        TabButton {
            text: qsTr("Help")
        }
    }
    */

    SwipeView {
        id: mainSwipe
        anchors.fill: parent
        // currentIndex: tabBarHeader.currentIndex
        currentIndex: tabBarFooter.currentIndex
        interactive: false

        Page
        {
            padding: 5
            GroupBox
            {
                anchors.fill: parent
                padding: 5
                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        visible: true   // QtMultimedia.availableCameras.length > 1
                        Label {
                            text: qsTr("Camera: ")
                            Layout.fillWidth: false
                        }
                        ComboBox {
                            id: camerasComboBox
                            Layout.fillWidth: true
                            model: QtMultimedia.availableCameras
                            textRole: "displayName"
                            currentIndex: 0
                        }
                    }

                    VideoOutput {
                        id: videoOutput
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        // filters: [zxingFilter, faceDetectFilter]
                        // filters: [zxingFilter, faceDetectFilter, sgDetFilter]
                        // filters: [zxingFilter, sgDetFilter]
                        filters: [sgDetFilter]
                        // filters: [faceDetectFilter]
                        source: video_cam00
                        autoOrientation: true

                        Image {
                            id: smile
                            source: "qrc:/smile.png"
                            visible: false
                        }

                        Shape {
                            id: polygon
                            anchors.fill: parent
                            visible: points.length == 4
                            ShapePath {
                                strokeWidth: 3
                                strokeColor: "red"
                                strokeStyle: ShapePath.SolidLine
                                fillColor: "transparent"
                                //TODO: really? I don't know qml...
                                startX: videoOutput.mapPointToItem(points[3]).x
                                startY: videoOutput.mapPointToItem(points[3]).y
                                PathLine {
                                    x: videoOutput.mapPointToItem(points[0]).x
                                    y: videoOutput.mapPointToItem(points[0]).y
                                }
                                PathLine {
                                    x: videoOutput.mapPointToItem(points[1]).x
                                    y: videoOutput.mapPointToItem(points[1]).y
                                }
                                PathLine {
                                    x: videoOutput.mapPointToItem(points[2]).x
                                    y: videoOutput.mapPointToItem(points[2]).y
                                }
                                PathLine {
                                    x: videoOutput.mapPointToItem(points[3]).x
                                    y: videoOutput.mapPointToItem(points[3]).y
                                }
                            }
                        }

                        Label {
                            id: info
                            color: "white"
                            padding: 10
                            background: Rectangle { color: "#80808080" }
                            // width: 1280
                            // height: 200
                            // anchors.fill: width

                        }

                        ColumnLayout {
                            anchors.right: parent.right

                            Switch {
                                id: tryRotateSwitch
                                background: Rectangle { color: "#80808080" }
                                text: qsTr("Try Rotate")
                                checked: true 
                            }
                            
                            Switch {
                                id: tryHarderSwitch
                                background: Rectangle { color: "#80808080" }
                                text: qsTr("Try Harder")
                                checked: true 
                            }
                            
                            Switch {
                                id: oneDSwitch
                                background: Rectangle { color: "#80808080" }
                                text: qsTr("1D Codes")
                                checked: true
                            }
                            
                            Switch {
                                id: twoDSwitch
                                background: Rectangle { color: "#80808080" }
                                text: qsTr("2D Codes")
                                checked: true 
                            }

                            Button {
                                property bool test : true
                                property var threadNucleoVal: 0
                                text: "Nucleo"
                            
                                onClicked: {
                                    if(test) {
                                        threadNucleoVal=threadNucleo.start()
                                    } else {
                                        threadNucleoVal=threadNucleo.quit()
                                    }
                                    console.log(threadNucleoVal)
                                    test=!test
                                }
                            }
                        }
                    }
                }
            }
        }

        Page {
            padding: 5

            GroupBox {
                anchors.fill: parent
                padding: 5
                
                Todo{
                }
            }

            /*
            GroupBox
            {
                anchors.fill: parent
                padding: 5

                Label
                {
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    wrapMode: Label.Wrap

                    text:   "Qt Qml AI  Demo Project" +
                            "For more information please refer to <a href=\"https://www.erturk.me\">StarGate, Inc.</a>"

                    // onLinkActivated: {
                    //     Qt.openUrlExternally(link)
                    // }
                }
            }
            */
        }
    }
    
    footer: TabBar {
        id: tabBarFooter
        currentIndex: mainSwipe.currentIndex
        TabButton {
            text: qsTr("Debug")
        }
        TabButton {
            text: qsTr("Todo")
        }
    }
    
}