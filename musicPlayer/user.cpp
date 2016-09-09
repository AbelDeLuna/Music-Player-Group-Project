#include "user.h"

User::User()
{
    name = "default";
}

User::User(QString name){
    this->name = name;
}
