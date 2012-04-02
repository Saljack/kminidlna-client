import QtQuick 1.1
import com.nokia.android 1.1

Window{
    id: window


    Rectangle{
        id:recTitle
        anchors.top: window.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#C0C0C0"
        Image{
            id: imgIcon
            width: 32
            height: 32
            source: "kminidlna.png"
        }
        Text{
            id: txtTitle
            text: "KMiniDLNA"
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
            anchors.left: parent.left
            anchors.right: parent.right
        }




    }

    PageStack {
        id: pageStack
        anchors {
            left: parent.left; right: parent.right;
            top: recTitle.bottom; bottom: toolBar.top
        }
    }

    ToolBar {
        id: toolBar
        anchors.bottom: window.bottom
        tools: ToolBarLayout {
            id: toolBarLayout
            ToolButton {
                id: tbtnBack
                flat: true
                iconSource: "toolbar-back"
                onClicked:
                    pageStack.depth <= 1 ? Qt.quit() : pageStack.pop();
            }

            ToolButton {
                id: tbtnReload
                flat: true
                iconSource: "toolbar-refresh"
                onClicked: {
                    client.reloadAll();
                    client.realoadMediaFolders();
                }
            }
        }
    }

    Component.onCompleted: {
        pageStack.push(Qt.resolvedUrl("mainpage.qml"));
    }

}



