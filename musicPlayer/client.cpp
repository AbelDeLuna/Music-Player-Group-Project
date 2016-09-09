#include "client.h"
#include <QDebug>
#include <QtNetwork>
#include <QtWidgets>
Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QLocalSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
}
Client::~Client()
{
    delete socket;
}

void Client::getConnection(QString serverName)
{
    socket->abort();
    socket->connectToServer(serverName);

    qDebug()<<"Client Connected to Server:"<<serverName;
}
void Client::readMessage()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    if (blockSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    if (in.atEnd())
        return;

    QString message;
    in >> message;

    //ui->label->setText(message);
}

void Client::sendMessage(QString message)
{
    qDebug()<<"Client Sending Message to the Server";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
    socket->waitForBytesWritten();
    socket->flush();
    qDebug()<<"Client Finished Sending Message";

}
