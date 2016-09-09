#include "server.h"
#include <QtWidgets>
#include <QtNetwork>
#include <stdlib.h>
#include <QDebug>
Server::Server(QString name,QObject *parent) : QObject(parent)
{
    totalRequests=0;
    myServer = new QLocalServer(this);
    if (!myServer->listen(name)) {
        qDebug()<<"Can't Start Server";
        return;
    }
    //connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(myServer, SIGNAL(newConnection()), this, SLOT(receiveConnection()));
    connect(this,SIGNAL(newMessage(QString)),this,SLOT(test(QString)));
    qDebug()<<"Started Server "<<name;
}
Server::~Server()
{
    qDebug()<<"Deleted Server ";
    delete myServer;
}

void Server::receiveConnection(){
    qDebug()<<"Recieved Connection";
    clientConnection = myServer->nextPendingConnection();

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));


    while (clientConnection->bytesAvailable() < (int)sizeof(quint16))
        clientConnection->waitForReadyRead();

    readMessage(clientConnection);
    clientConnection->disconnectFromServer();
}

QString Server::readMessage(QLocalSocket* clientConnection)
{
    qDebug()<<"Started Reading Message";
    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    quint16 bytes_to_read;
    in >> bytes_to_read;
    qDebug()<<"Finished Reading Message";

    QString message;
    in >> message;
    message= message.remove(QChar('\"'));
    qDebug()<<"Recieved Message is "<<message;
    emit newMessage(message);
    messageBuffer = message;
    return message;

}

void Server::test(QString message){
    //qDebug()<<"Recieved Emit Message "<<message;
}
/*
QString Server::getLinkBuffer(){
    return messageBuffer;
}
*/
