#include "chat_server.h"
#include <QDebug>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent){
    qint16 port = 12345;
    if(listen(QHostAddress::Any, port)){
        qDebug() << "Сервер принимает подключения";
    } else {
        qDebug() << "Ошибка сервера!";
    }
}

ChatServer::~ChatServer(){
    QTcpServer::close();
}

void ChatServer::incomingConnection(qintptr socketDescriptor){
    QTcpSocket *clientsocket = new QTcpSocket(this);

    if (clientsocket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Новое TCP-соединение установлено!";

        // Чтение входящих данных от клиента
        connect(clientsocket, &QTcpSocket::readyRead, this, [this, clientsocket](){
            QByteArray data = clientsocket->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);

            if (!doc.isNull() && doc.isObject()) {
                QJsonObject jsonObj = doc.object();
                QString type = jsonObj["type"].toString();

                if (type == "auth") {
                    QString username = jsonObj["username"].toString();
                    QString clientId = jsonObj["senderID"].toString();

                    // Ключом делаем ID, а в структуре сохраняем имя и сокет
                    clients_[clientId] = ClientInfo{ username, clientsocket };

                    qDebug() << "Пользователь вошел в чат! Личный ID:" << clientId << "| Имя:" << username;
                    broadcastUserList();
                }
                // Обработка обычного сообщения (тип "message")
                else if (type == "private_message") {
                    QString senderId = jsonObj["senderID"].toString();
                    QString recipientId = jsonObj["recipientID"].toString();
                    QString content = jsonObj["content"].toString();

                    // Достаем имя отправителя из мапы
                    QString senderName = clients_.contains(senderId) ? clients_[senderId].name : "Неизвестный";

                    // Внедряем имя в JSON, чтобы получатель знал кто написал
                    jsonObj["senderName"] = senderName;
                    jsonObj["senderID"] = senderId;
                    QJsonDocument modifiedDoc(jsonObj);
                    QByteArray modifiedData = modifiedDoc.toJson(QJsonDocument::Compact);

                    // Пересылаем пакет получателю
                    if (clients_.contains(recipientId)) {
                        clients_[recipientId].socket->write(modifiedData);
                        qDebug() << "Переслано от" << senderName << "для ID:" << recipientId;
                    } else {
                        qDebug() << "Ошибка: Получатель не найден!";
                    }
                }
            }
        });

        // Обработка отключения клиента
        connect(clientsocket, &QTcpSocket::disconnected, this, [this, clientsocket](){
            // Ищем ключ (ID/имя) в QMap по значению сокета в структуре ClientInfo
            QString disconnectedId;
            for (auto it = clients_.begin(); it != clients_.end(); ++it) {
                if (it.value().socket == clientsocket) {
                    disconnectedId = it.key();
                    break;
                }
            }

            if (!disconnectedId.isEmpty()) {
                clients_.remove(disconnectedId);
                qDebug() << "Клиент с ID" << disconnectedId << "отключился. Осталось клиентов:" << clients_.size();
            }
            clientsocket->deleteLater();
        });

    } else {
        delete clientsocket;
    }
}
// Вывод всех пользователей
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

    qDebug() << "Все клиенты: " << UsersArray;

    for(auto it = clients_.constBegin(); it != clients_.constEnd(); ++it){
        it.value().socket->write(data);
    }
}
















