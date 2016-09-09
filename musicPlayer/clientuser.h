#ifndef CLIENTUSER_H
#define CLIENTUSER_H
#include <user.h>
#include <client.h>

class ClientUser : public User
{
public:
    ClientUser(QString);
//private:
    Client *myClient;

};

#endif // CLIENTUSER_H
