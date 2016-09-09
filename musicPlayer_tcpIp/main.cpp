#include <QApplication>
#include <QtCore>
#include <QObject>
#include <QString>
#include <stdlib.h>

#include <clientuser.h>
#include <hostuser.h>
#include <server.h>
#include <user.h>
#include <player.h>
#include <download.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//    Player musicPlayer;
    Download downloader("/Users/conoroshaughnessy/");
    HostUser host("host user");
    //ClientUser client("client user");

    QObject::connect(host.myServer,SIGNAL(newMessage(QString)),&downloader,SLOT(downloadFile(QString)));
    //QObject::connect(downloader,SIGNAL(finishedDownloading(QString)),&musicPlayer,SLOT(addSong(QString)));

    /*
    QString filePath = "C:/Users/jappa_000/Downloads/ff7.mp3";

    musicPlayer.addSong(filePath);

    filePath = "C:/Users/jappa_000/Downloads/soundsOfdarkness.mp3";
    musicPlayer.addSong(filePath);
    musicPlayer.startPlaylist();
    musicPlayer.play();
    QString downloadLink = "https://www.youtube.com/watch?v=v-RE7RUzjf8";
    */

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return app.exec();
}
