#include <QCoreApplication>
#include "chat_server/chat_server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ChatServer server;

    return QCoreApplication::exec();
}
