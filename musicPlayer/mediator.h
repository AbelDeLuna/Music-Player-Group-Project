#ifndef MEDIATOR_H
#define MEDIATOR_H

#include "server.h"
#include "download.h"
#include "player.h"
#include "hostuser.h"
#include <QObject>
#include <QQueue>
class Mediator : public QObject
{
    Q_OBJECT
public:
    explicit Mediator(Player*,Download*,HostUser*,QObject *parent =0);
    ~Mediator();

public slots:
    void download();
    void downloadToPlayer(QString);
    void addQueue(QString);
    void finishedDownload(QString);
private:
    Player* _player;
    Download* _download;
    HostUser* _hostUser;
    QQueue<QString> downloadQueue;
    bool currentlyDownloading;

signals:
    void queueContainsItems();



};

#endif // MEDIATOR_H
