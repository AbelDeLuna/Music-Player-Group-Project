#include <QtWidgets>
#include <QtNetwork>
#include <QDebug>
#include <stdlib.h>
#include "server.h"

Server::Server(QWidget *parent)
    : QDialog(parent)
    , statusLabel(new QLabel)
    , tcpServer(Q_NULLPTR)
    , networkSession(0)
    , blockSize(0)
    , linkCombo(new QComboBox)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    statusLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Server::sessionOpened);

        statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    } else {
        sessionOpened();
    }
//----------------------------------------------------------------------------------------------------------------
    QLabel *youTubeLinksLabel = new QLabel(tr("youTubeLinksLabel:"));
    youTubeLinksLabel->setBuddy(linkCombo);

    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);
//----------------------------------------------------------------------------------------------------------------
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::storeLink);
//----------------------------------------------------------------------------------------------------------------

    QVBoxLayout *mainLayout = Q_NULLPTR;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        QVBoxLayout *outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        QHBoxLayout *outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        QGroupBox *groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QVBoxLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QVBoxLayout(this);
    }

    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(youTubeLinksLabel);
    mainLayout->addWidget(linkCombo);

    setWindowTitle(QGuiApplication::applicationDisplayName());
}

void Server::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    statusLabel->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                         .arg(ipAddress).arg(tcpServer->serverPort()));
}

void Server::storeLink()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QAbstractSocket::disconnected,
            tcpSocket, &QObject::deleteLater);
    connect(tcpSocket, &QIODevice::readyRead, this, &Server::readData);
}

void Server::readData()
{
    qDebug()<<"Started Reading Message";
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    if (tcpSocket->bytesAvailable() < blockSize)
        return;

    QString newLink;
    in >> newLink;

    emit newMessage(newLink);   // sends link out, will be captured by downloader linked w/in main

    linkCombo->addItem(newLink);
    tcpSocket->disconnectFromHost();
    blockSize = 0;
}


//#include "server.h"
//#include <QtWidgets>
//#include <QtNetwork>
//#include <stdlib.h>
//#include <QDebug>
//Server::Server(QString name,QObject *parent) : QObject(parent)
//{
//    totalRequests=0;
//    myServer = new QLocalServer(this);
//    if (!myServer->listen(name)) {
//        qDebug()<<"Can't Start Server";
//        return;
//    }
//    //connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
//    connect(myServer, SIGNAL(newConnection()), this, SLOT(receiveConnection()));
//    connect(this,SIGNAL(newMessage(QString)),this,SLOT(test(QString)));
//    qDebug()<<"Started Server "<<name;
//}
//Server::~Server()
//{
//    qDebug()<<"Deleted Server ";
//    delete myServer;
//}

//void Server::receiveConnection(){
//    qDebug()<<"Recieved Connection";
//    clientConnection = myServer->nextPendingConnection();

//    connect(clientConnection, SIGNAL(disconnected()),
//            clientConnection, SLOT(deleteLater()));


//    while (clientConnection->bytesAvailable() < (int)sizeof(quint16))
//        clientConnection->waitForReadyRead();

//    readMessage(clientConnection);
//    clientConnection->disconnectFromServer();
//}

//QString Server::readMessage(QLocalSocket* clientConnection)
//{
//    qDebug()<<"Started Reading Message";
//    QDataStream in(clientConnection);
//    in.setVersion(QDataStream::Qt_4_0);
//    quint16 bytes_to_read;
//    in >> bytes_to_read;
//    qDebug()<<"Finished Reading Message";

//    QString message;
//    in >> message;
//    message= message.remove(QChar('\"'));
//    qDebug()<<"Recieved Message is "<<message;
//    emit newMessage(message);
//    messageBuffer = message;
//    return message;

//}
