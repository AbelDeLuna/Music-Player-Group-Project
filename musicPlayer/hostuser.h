#ifndef HOSTUSER_H
#define HOSTUSER_H
#include "user.h"
#include "server.h"
#include <QList>
class HostUser : public User
{
public:
    HostUser();
    HostUser(QString);
    Server *myServer;
private:
    QList<QString> recievedMessages;
public slots:
    void storeMessage(QString);
};

#endif // HOSTUSER_H
