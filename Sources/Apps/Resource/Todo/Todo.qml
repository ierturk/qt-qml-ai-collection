import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Universal 2.15

import org.erturk.todo 1.0

ColumnLayout {
    property bool statusChanged: false

    Timer {
        id: timer
        function setTimeout(cb, delayTime) {
            timer.interval = delayTime;
            timer.repeat = false;
            timer.triggered.connect(cb);
            timer.triggered.connect(function release () {
                timer.triggered.disconnect(cb);
                timer.triggered.disconnect(release);
            });
            timer.start();
        }
    }

    // Frame comes from QtQuick.Controls
    Frame {
        Layout.fillWidth: true

        background: Rectangle {
            color: "transparent"
            border.color: "#AA00FF"
            radius: 2
        }

        ListView {
            /* Important features: Size, Model, Delegate */
            implicitWidth: 560
            implicitHeight: 380
            clip: true // Any list items outside the listview won't be visible
            anchors.fill: parent

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            // Example of model with 100 checkboxes
            //        model: 100

            // Example of model done here with 2 checkboxes
            //        model: ListModel {
            //            ListElement {
            //                done: true
            //                description: "Attend a meeting at 1:45pm"
            //            }
            //            ListElement {
            //                done: false
            //                description: "Fix the sink"
            //            }
            //        }

            // Example of model implemented in C++
            model: TodoModel {
                // After adding this make sure to run qmake from the Build menu
                // This is a tricky part, because here `todoList` is a constructor variable from main.cpp
                list: todoList
            }

            // RawLayout comes from QtQuick.Layouts
            delegate: RowLayout {
                width: parent ? parent.width : 0 // For some reason, without condition it returns notification of parent=null

                CheckBox {
                    checked: model.done
                    onClicked: {
                        model.done = checked;
                        statusChanged = true;
                        model.status = (model.status !== 2) ? 1 : model.status;
                        timer.setTimeout(function(){ console.log("triggered"); btnSave.clicked()}, 5000);
                    }
                }
                TextField {
                    id: txtId
                    text: model.id
                    width: 10
                    visible: false
                    readOnly: true

                }
                TextField {
                    id: txtDescription
                    text: model.description
                    readOnly: true
                    onEditingFinished: model.description = text
                    onTextEdited: {
                        statusChanged = true;
                        model.status = (model.status !== 2) ? 1 : model.status;
                        model.description = text;
                        timer.setTimeout(function(){
                            if (statusChanged) {
                                console.log("triggered");
                                btnSave.clicked();
                            }
                        }, 5000);
                    }
                    Layout.fillWidth: true
                }
                TextField {
                    id: txtBarcode
                    text: model.barcode
                    width: 30
                    visible: true
                    readOnly: true
                }
            }
        }
    }

    RowLayout {
        Button {
            text: qsTr("Add new item")
            onClicked: {
                statusChanged = true;
                todoList.appendItem()
            }
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Remove completed")
            onClicked: todoList.removeCompletedItems()
            Layout.fillWidth: true
        }

        Button {
            id: btnSave
            text: qsTr("Save")
            onClicked: {
                statusChanged = false;
                todoList.saveItems()
            }
            Layout.fillWidth: true
            enabled: statusChanged ? true : false;
        }
    }
}
