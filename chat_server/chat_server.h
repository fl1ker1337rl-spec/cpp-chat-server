#pragma once
#include <QTcpServer>
#include <QObject>
#include <QMap>

struct ClientInfo {
    QString name;
    QTcpSocket *socket;
};

class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = nullptr);
    virtual ~ChatServer();
    void broadcastUserList();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    ChatServer(const ChatServer&) = delete;
    ChatServer& operator=(const ChatServer&) = delete;
    ChatServer(ChatServer&&) = delete;
    ChatServer& operator=(ChatServer&&) = delete;

    QMap<QString, ClientInfo> clients_;

};

