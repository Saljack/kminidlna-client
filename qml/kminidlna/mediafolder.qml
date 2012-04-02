import QtQuick 1.0
import com.nokia.android 1.1


Page {
    id: mediaFoldersPage
    anchors.fill: parent

    XmlListModel {
        id: xmlModel
        xml: ""
        query: "/mediafolders/mediafolder"
        XmlRole { name: "path"; query: "path/string()" }
        XmlRole { name: "mediaType"; query: "mediatype/string()" }

    }
    Text {
        id: txtMediaTitle
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: "white";

        text: "Media Folders";
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 25
    }
    ListView {
            anchors.top: txtMediaTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: mediaFoldersPage.height
            model: xmlModel
            delegate: Text {color: "white";  text: path+ "; "+mediaType; font.pixelSize: 22}
        }

    Connections{
        target: client
        onMediaFoldersLoaded:{
            xmlModel.xml = xmlDoc;
        }
    }

    function getXMLError(){
        return xmlModel.status;
    }

}
