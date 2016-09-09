#include "client.h"
#include "ui_client.h"
#include <QtNetwork>
#include <QtWidgets>

client::client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::client)
{
    ui->setupUi(this);
    socket = new QLocalSocket(this);
    connect(ui->submitPushButton, SIGNAL(clicked()),
            this, SLOT(getConnection()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
}

client::~client()
{
    delete socket;
    delete ui;
}


void client::getConnection()
{
    /*
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << QString::number(1337);
    out.device()->seek(0);
    */

    //blockSize = 0;
    QString hello= "Hello";
    socket->abort();
    socket->connectToServer("fortune");
    sendMessage(ui->lineEdit->displayText());
}

void client::readMessage()
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

    ui->label->setText(message);
}

void client::sendMessage(QString message)
{
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
}

/*void server::sendMessage()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << QString::number(totalRequests);
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    totalRequests++;
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    clientConnection->write(block);
    clientConnection->flush();
    clientConnection->disconnectFromServer();
}
*/

void client::enableGetFortuneButton()
{

}
