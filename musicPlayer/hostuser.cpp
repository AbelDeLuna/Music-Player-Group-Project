#include "hostuser.h"

HostUser::HostUser() :User()
{
    QString name = "default";
    myServer=new Server(name);
}

HostUser::HostUser(QString name) :User(name){
    myServer=new Server(name);
}

void HostUser::storeMessage(QString message){
    recievedMessages.append(message);

}
