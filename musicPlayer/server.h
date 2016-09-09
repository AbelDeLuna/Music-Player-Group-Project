#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDialog>
#include <QString>
#include <qlocalserver.h>
#include <qlocalsocket.h>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QLocalServer;
QT_END_NAMESPACE


class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QString name,QObject *parent = 0);
    QString getLinkBuffer();


    ~Server();
public slots:
    QString readMessage(QLocalSocket* clientConnection);
    void receiveConnection();
    void test(QString);

private:
    QString messageBuffer;
    quint16 blockSize;
    QString serverName;
    int totalRequests;
    QLocalServer *myServer;
    QLocalSocket *clientConnection;
signals:
    void newMessage(QString message);

};

#endif // SERVER_H
