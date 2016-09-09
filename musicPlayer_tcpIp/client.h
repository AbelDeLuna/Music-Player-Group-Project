#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;
QT_END_NAMESPACE

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = Q_NULLPTR);

private slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void sessionOpened();

    void enableSubmitLinkButton();
    void submitLink();

private:
    QComboBox *hostCombo;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;

    QTcpSocket *tcpSocket;
    quint16 blockSize;

    QNetworkSession *networkSession;

    QLineEdit *linkEdit;
    QPushButton *submitLinkButton;
};

#endif
