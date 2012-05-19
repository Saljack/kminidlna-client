/*
     KMiniDLNA Android client
    
     Copyright (C) 2012  Tomáš Poledný saljacky(at)gmail(dot)com
 
     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.
 
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkclient.h"
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QDir>
#include <QByteArray>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslSocket>
#include <QtXml/QDomDocument>
#include <QtXml/QDomText>
#include <QtXml/QDomNode>
#include <QTimer>
#include <QSettings>
#include <QFile>


const QString NetworkClient::GET_STATE ="/minidlna-state.xml";
const QString NetworkClient::PUT_STATE ="/minidlna-set-state.xml";
const QString NetworkClient::VERSION ="/version.xml";
const QString NetworkClient::MEDIA_FOLDERS ="/mediafolders.xml";
const QByteArray NetworkClient::DEFAULT_HOST = "https://192.168.1.1:8080";

NetworkClient::NetworkClient(QObject *parent) :
    QObject(parent), m_host(DEFAULT_HOST)
{
    run = false;
    m_mng = new QNetworkAccessManager(this);

    connect(m_mng, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    connect(m_mng, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), SLOT(onNetworkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
    m_sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    loadSettings();
}

NetworkClient::~NetworkClient(){
    saveSettings();
}

void NetworkClient::onNetworkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility access){
    if(access < 1){
        emit noConnection(-1);
    }
}

bool NetworkClient::isNetwork(){
    return m_mng->networkAccessible();
}

void NetworkClient::start(){
    QByteArray str;
    if(run){
        str = "stop";
    }else{
        str = "start";
    }
    QNetworkRequest* req = createNetworkRequest(QUrl(m_host+PUT_STATE));
    req->setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    QNetworkReply *reply = 0;
//    QNetworkRequest copiedreq = *req;

    reply = m_mng->put(*req, QByteArray("<?xml version=\"1.0\" encoding=\"UTF-8\" ?><minidlna><run>"+str+"</run></minidlna>"));
    delete req;
}

void NetworkClient::loadState(){
    QNetworkRequest* req = createNetworkRequest(QUrl(m_host+GET_STATE));
    QNetworkReply *reply = 0;
    reply = m_mng->get(*req);
    delete req;
}

void NetworkClient::reloadAll(){
    loadState();
    loadVersion();
}

void NetworkClient::loadVersion(){
    QNetworkRequest* req = createNetworkRequest(QUrl(m_host+VERSION));
    QNetworkReply *reply = 0;
    reply = m_mng->get(*req);
    delete req;
}

void NetworkClient::realoadMediaFolders(){
    QNetworkRequest* req = createNetworkRequest(QUrl(m_host+MEDIA_FOLDERS));
    QNetworkReply *reply = 0;
    reply = m_mng->get(*req);
    delete req;
}

QString NetworkClient::parseVersion(const QByteArray& xml){
    QDomDocument doc;
    doc.setContent(xml);
    QDomText state = doc.documentElement().firstChildElement("version").firstChild().toText();
    return state.data();
}

QString NetworkClient::parseState(const QByteArray& xml){
    QDomDocument doc;
    doc.setContent(xml);
    QDomText state = doc.documentElement().firstChildElement("state").firstChild().toText();
    return state.data();
}

/**
  * Must delete returned pointer!
  */
QNetworkRequest* NetworkClient::createNetworkRequest(const QUrl& url){
    QNetworkRequest* req = new QNetworkRequest(url);
    req->setRawHeader("Authorization", "Basic " +
                      (m_login.toLatin1()+":"+QByteArray::fromBase64(m_password)).toBase64());
    req->setSslConfiguration(m_sslConf);
    return req;
}


void NetworkClient::finished(QNetworkReply* reply){
    QString str = reply->url().path();
    qDebug() << "ERROR: " << reply->error() << reply->errorString();
    switch(reply->error()){
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::ConnectionRefusedError:
        emit noConnection(QNetworkReply::ConnectionRefusedError);
        break;
    case QNetworkReply::ContentAccessDenied:
    case QNetworkReply::AuthenticationRequiredError:
        emit noConnection(QNetworkReply::ContentAccessDenied);
        break;
    case QNetworkReply::UnknownNetworkError:
         emit noConnection(QNetworkReply::UnknownNetworkError);
         break;
    case QNetworkReply::NoError:
        if(str == VERSION){
            QString v = parseVersion(reply->readAll());
            if(!v.isEmpty()){
                emit versionRecieved(v);
            }
        }else if(str == GET_STATE && reply->operation() == QNetworkAccessManager::GetOperation){

            QString state = parseState(reply->readAll());
            if(!state.isEmpty()){
                if(state == "running"){
                    run = true;
                }else{
                    run = false;
                }
                emit stateRecieved(state);
            }
        }else if(str == MEDIA_FOLDERS && reply->operation() == QNetworkAccessManager::GetOperation){
            QString xmlDoc(reply->readAll());
            emit mediaFoldersLoaded(xmlDoc);
        }else{
            QTimer::singleShot(500, this, SLOT(reloadAll()));
        }
        break;
    default:
        qDebug() << "Other reply: " << reply->errorString() << reply->error();
    }
    reply->deleteLater();
}

void NetworkClient::loadSettings(){
    QSettings settings("Saljack", "KMiniDLNA");
    m_host = settings.value("host", DEFAULT_HOST).toByteArray();
    m_login = settings.value("login", "username").toString();
    m_password = settings.value("password", QByteArray("password").toBase64()).toByteArray();
    m_rememberPassword = settings.value("rememberPassword", false).toBool();
    emit settingsLoaded();
}

void NetworkClient::saveSettings(){
    QSettings settings("Saljack", "KMiniDLNA");
    settings.setValue("host", m_host);
    settings.setValue("login", m_login);
    settings.setValue("rememberPassword", m_rememberPassword);
    if(rememberPassword()){
        settings.setValue("password", m_password);
    }else{
        settings.remove("password");
    }

    settings.sync();
}

QString NetworkClient::host(){
    return m_host;
}

QString NetworkClient::login(){
    return m_login;
}

QString NetworkClient::password(){
    return QByteArray::fromBase64(m_password);
}

void NetworkClient::setHost(QString host){
    m_host = host.toAscii();
}

void NetworkClient::setLogin(QString login){
    m_login = login;
}

void NetworkClient::setPassword(QString password){
    m_password = password.toAscii().toBase64();
}


bool NetworkClient::rememberPassword(){
    return m_rememberPassword;
}

void NetworkClient::setRememberPassword(bool remember){
    m_rememberPassword = remember;
}

