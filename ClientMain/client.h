#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <qlocalsocket.h>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QLocalSocket;
QT_END_NAMESPACE



namespace Ui {
class client;
}

class client : public QDialog
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0);
    ~client();
private slots:
    void getConnection();
    void readMessage();
    void sendMessage(QString message);
    void enableGetFortuneButton();

private:
    QLocalSocket *socket;
    QString currentFortune;
    quint16 blockSize;
    Ui::client *ui;
};

#endif // CLIENT_H
