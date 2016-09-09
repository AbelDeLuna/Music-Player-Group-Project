#include "hostuser.h"
#include <QApplication>

HostUser::HostUser() :User()
{
    myServer=new Server();
    QGuiApplication::setApplicationDisplayName(Server::tr("default"));
    myServer->show();
}

HostUser::HostUser(QString name) :User(name)
{
    myServer = new Server();
    QGuiApplication::setApplicationDisplayName(Server::tr("name"));
    myServer->show();
}

void HostUser::storeMessage(QString message){
    recievedMessages.append(message);

}
