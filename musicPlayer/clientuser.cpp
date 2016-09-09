#include "clientuser.h"

ClientUser::ClientUser(QString name): User(name){
    myClient = new Client();
}
