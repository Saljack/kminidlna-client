#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QSslConfiguration>
#include <QString>
#include <QByteArray>
#include <QtNetwork/QNetworkReply>

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = 0);
    ~NetworkClient();
    bool isNetwork();
    Q_INVOKABLE void loadSettings();
    Q_INVOKABLE void saveSettings();

    //Static path
    static const QString VERSION;
    static const QString GET_STATE;
    static const QString PUT_STATE;
    static const QString MEDIA_FOLDERS;
    static const QByteArray DEFAULT_HOST;
    Q_INVOKABLE QString login();
    Q_INVOKABLE QString password();
    Q_INVOKABLE QString host();
    Q_INVOKABLE bool rememberPassword();
    Q_INVOKABLE void setHost(QString host);
    Q_INVOKABLE void setLogin(QString login);
    Q_INVOKABLE void setPassword(QString password);
    Q_INVOKABLE void setRememberPassword(bool remember);


private:
    QNetworkAccessManager* m_mng;
    QSslConfiguration m_sslConf;
    QNetworkRequest* createNetworkRequest(const QUrl& url);
    QByteArray m_host;
    QString parseVersion(const QByteArray& xml);
    QString parseState(const QByteArray& xml);
    QObject* m_txtLogin;
    QString m_login;
    QByteArray m_password;
    bool run;
    bool m_rememberPassword;

signals:
    void versionRecieved(QString version);
    void stateRecieved(QString state);
    void settingsLoaded();
    void noConnection(int networkError);
    void mediaFoldersLoaded(QString xmlDoc);

public slots:
    Q_INVOKABLE void start();
    Q_INVOKABLE void loadVersion();
    Q_INVOKABLE void loadState();
    Q_INVOKABLE void reloadAll();
    Q_INVOKABLE void realoadMediaFolders();
    void finished(QNetworkReply* reply);
    void onNetworkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility access);
};

#endif // NETWORKCLIENT_H
