#include "user.h"

// used as base class to wrap host or client as users
User::User()
{
    name = "default";
}

User::User(QString name){
    this->name = name;
}
