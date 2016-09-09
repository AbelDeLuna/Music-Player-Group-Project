#include "clientuser.h"
#include <QApplication>

ClientUser::ClientUser(QString name): User(name){
    QGuiApplication::setApplicationDisplayName(Client::tr("Fortune Client"));
    myClient = new Client();
    myClient->show();
}
