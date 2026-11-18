#include "chat_server.h"
#include <QDebug>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent){
    qint16 port = 12345;
    if(listen(QHostAddress::Any, port)){
        qDebug() << "Server is listening for connections";
    } else {
        qDebug() << "Server error!";
    }
}

ChatServer::~ChatServer(){
    QTcpServer::close();
}

void ChatServer::incomingConnection(qintptr socketDescriptor){
    QTcpSocket *clientsocket = new QTcpSocket(this);

    if (clientsocket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "New TCP connection established!";

        // Reading incoming data from the client
        connect(clientsocket, &QTcpSocket::readyRead, this, [this, clientsocket](){
            QByteArray data = clientsocket->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);

            if (!doc.isNull() && doc.isObject()) {
                QJsonObject jsonObj = doc.object();
                QString type = jsonObj["type"].toString();

                if (type == "auth") {
                    QString username = jsonObj["username"].toString();
                    QString clientId = jsonObj["senderID"].toString();

                    // Using ID as key, and storing name and socket in the structure
                    clients_[clientId] = ClientInfo{ username, clientsocket };

                    qDebug() << "User logged into chat! Personal ID:" << clientId << "| Name:" << username;
                    broadcastUserList();
                }
                // Processing regular message (type "message")
                else if (type == "private_message") {
                    QString senderId = jsonObj["senderID"].toString();
                    QString recipientId = jsonObj["recipientID"].toString();
                    QString content = jsonObj["content"].toString();

                    // Retrieving sender name from the map
                    QString senderName = clients_.contains(senderId) ? clients_[senderId].name : "Unknown";

                    // Injecting name into JSON so the recipient knows who wrote it
                    jsonObj["senderName"] = senderName;
                    jsonObj["senderID"] = senderId;
                    QJsonDocument modifiedDoc(jsonObj);
                    QByteArray modifiedData = modifiedDoc.toJson(QJsonDocument::Compact);

                    // Forwarding the packet to the recipient
                    if (clients_.contains(recipientId)) {
                        clients_[recipientId].socket->write(modifiedData);
                        qDebug() << "Forwarded from" << senderName << "to ID:" << recipientId;
                    } else {
                        qDebug() << "Error: Recipient not found!";
                    }
                }
            }
        });

        // Handling client disconnection
        connect(clientsocket, &QTcpSocket::disconnected, this, [this, clientsocket](){
            // Searching for key (ID/name) in QMap by socket value in ClientInfo structure
            QString disconnectedId;
            for (auto it = clients_.begin(); it != clients_.end(); ++it) {
                if (it.value().socket == clientsocket) {
                    disconnectedId = it.key();
                    break;
                }
            }

            if (!disconnectedId.isEmpty()) {
                clients_.remove(disconnectedId);
                qDebug() << "Client with ID" << disconnectedId << "disconnected. Clients remaining:" << clients_.size();
            }
            clientsocket->deleteLater();
        });

    } else {
        delete clientsocket;
    }
}
// Broadcasting user list to all
void ChatServer::broadcastUserList(){
    QJsonArray UsersArray;

    for(auto it = clients_.constBegin(); it != clients_.constEnd(); ++it){
        QJsonObject userObj;
        userObj["id"] = it.key();
        userObj["name"] = it.value().name;
        UsersArray.append(userObj);
    }

    QJsonObject rootObj;
    rootObj["type"] = "user_list";
    rootObj["users"] = UsersArray;

    QJsonDocument doc(rootObj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    qDebug() << "All clients: " << UsersArray;

    for(auto it = clients_.constBegin(); it != clients_.constEnd(); ++it){
        it.value().socket->write(data);
    }
}