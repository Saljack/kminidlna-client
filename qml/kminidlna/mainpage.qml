import QtQuick 1.0
import com.nokia.android 1.1


Page {
    id: mainPage
    anchors.fill: parent
//    Flickable {
//        id: flick
//         anchors.fill: parent
//         contentWidth: column1.width; contentHeight: column1.height;
        Column {
            id: column1
            anchors{
                left: parent.left; right: parent.right;
                top: parent.top; bottom: parent.bottom
            }

            spacing: 8

            Item{
                id: itmState
                anchors.left: parent.left
                anchors.right: parent.right
                height: 40

                Rectangle {
                    id: led
                    anchors.top: parent.top
                    width: 32
                    height: 32
                    color: "#8d8d8d"
                    radius: 45
                    border.color: "black"
                }
                Text{
                    id: txtState
                    text: "Not loaded"
                    color: "White";
                    font.pixelSize: 24;
                    anchors.left: led.right
                    anchors.right: parent.right
                }

            }


            Text{
                anchors.left: parent.left
                anchors.right: parent.right
                color: "#ffffff"
                font.pixelSize:24
                text: "Host"
            }

            TextField{
                id: txtHost
                inputMethodHints: Qt.ImhNoPredictiveText;
                anchors.left: parent.left
                anchors.right: parent.right
                text: client.host();
                onTextChanged: {
                    client.setHost(txtHost.text);
                }
            }
            Text{
                color: "#ffffff"
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 24

                text: "Login"
            }

            TextField{
                id: txtLogin
                anchors.left: parent.left
                anchors.right: parent.right

                text: client.login();
                onTextChanged: {
                    client.setLogin(txtLogin.text);
                }
            }
            Text{
                color: "#ffffff"
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 24
                text: "Password"
            }
            TextField{
                id: txtPassword
                anchors.left: parent.left
                anchors.right: parent.right

                text:  client.password();
                echoMode: TextInput.Password
                onTextChanged: {
                    client.setPassword(txtPassword.text);
                }
            }


            CheckBox {
                id: chbxRememberPassword
                checked: client.rememberPassword();
                text: "Remember password"
                onClicked:{
                    client.setRememberPassword(chbxRememberPassword.checked);
                }
            }
            Text {
                id: txtLastUpdate
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Last update: Not know"
                color: "white";
                font.pixelSize: 24
            }

            Text {
                id: txtVersion
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Not loaded"
                color: "white";
                font.pixelSize: 24
                Connections {
                    target: client
                    onVersionRecieved:{
                        updateLastUpdate();
                        txtVersion.text = "Interface version: "+version;
                    }
                }
            }
            Button {
                id: btnVersion
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Reload state"
                onClicked: {
                    client.reloadAll()
                }
            }
            Button {
                id: btnShowFolders
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Show folders"
                onClicked: {

                    pageStack.push(Qt.resolvedUrl("mediafolder.qml"));
                    client.realoadMediaFolders();
                }
            }

            Button {
                id: btnStart
                anchors.left: parent.left
                anchors.right: parent.right
                text: "Start"
                onClicked: {
                    client.start()
                }
            }
//        }
    }

    QueryDialog{
        id: tDialog
        acceptButtonText: "Ok"
        titleText: "Connection problem"
        //        title: [Text{
        //                text:"Connection was not found"
        //                color: "white";
        //            }
        //        ]
        //        buttons: [
        //            Button{
        //                id: btnOK
        //                anchors.left: parent.left
        //                anchors.right: parent.right
        //                text: "Ok"
        //                onClicked: {
        //                   tDialog.accept ();
        //                }
        //            }
        //        ]
        //             onAccepted: callbackFunction()
    }

    Connections {
        target: client
        onStateRecieved:{
            updateLastUpdate();
            if(state == "running"){
                led.color = "#00B000";
                txtState.text = " running";
                btnStart.text = "Stop"
            }else{
                led.color = "#8d8d8d";
                txtState.text = " not running";
                btnStart.text = "Start"
            }
        }
    }

    Connections{
        target: client
        onSettingsLoaded:{
            txtHost.text = client.host();
            txtLogin.text = client.login();
            txtPassword.text = client.password();
            chbxRememberPassword.checked = client.rememberPassword();
        }
    }

    Connections{
        target: client
        onNoConnection:{
            updateLastUpdate();
            txtState.text = " No connection";
            txtVersion.text = "Interface version: No connection";
            var txtErr = getTextError(networkError);
            tDialog.titleText = txtErr;
            tDialog.open();

        }
    }

    Component.onCompleted: {
        client.loadSettings();
        tDialog.close();
        client.reloadAll();
    }

    function getTextError(err){
        if(err == -1){
            return "Network is not accessible";
        }else if(err == 1){
            return "Connection refused.";
        }else if(err == 201){
            return "Wrong login or password";
        }
    }

    function updateLastUpdate(){
        var time = new Date();
        txtLastUpdate.text = "Last update: " + Qt.formatDateTime(time,  "hh:mm:ss d. M. yyyy ");
    }


}


