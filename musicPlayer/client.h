#ifndef CLIENT_H
#define CLIENT_H
#include <qlocalsocket.h>
QT_BEGIN_NAMESPACE
class QLocalSocket;
QT_END_NAMESPACE


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    ~Client();
public slots:
    void getConnection(QString serverName);
    void readMessage();
    void sendMessage(QString message);

private:
    QLocalSocket *socket;
    QString currentFortune;
    quint16 blockSize;
};

#endif // CLIENT_H
