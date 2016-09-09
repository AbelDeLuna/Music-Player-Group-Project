#include "mainwindow.h"
#include <QApplication>
#include <clientuser.h>
#include <hostuser.h>
#include <server.h>
#include <user.h>
#include <player.h>
#include <QString>
#include "download.h"
#include <QObject>
#include "mediator.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
/*
    HostUser Hello("fortune");
    ClientUser client("fortune");
    client.myClient->getConnection("fortune");
    client.myClient->sendMessage((QString)"Hello World");
*/
    MainWindow window;
    Player* musicPlayer = new Player;
    Download* downloader = new Download;
    HostUser* user = new HostUser("fortune");
    Mediator* mediator = new Mediator(musicPlayer, downloader, user);

    window.setPlayer(musicPlayer);
    window.show();

   // QObject::connect(user->myServer,SIGNAL(newMessage(QString)),downloader,SLOT(downloadYoutube(QString)));
    //QObject::connect(&downloader,SIGNAL(finishedDownloading(QString)),&musicPlayer,SLOT(addSong(QString)));
    //QString filePath = "C:/Users/jappa_000/Downloads/ff7.mp3";

    //musicPlayer->addSong(filePath);

    //filePath = "C:/Users/jappa_000/Downloads/soundsOfdarkness.mp3";
    //musicPlayer.addSong(filePath);
   // musicPlayer->startPlaylist();
   // musicPlayer->play();
    //QString downloadLink = "https://www.youtube.com/watch?v=v-RE7RUzjf8";

   // downloader->downloadYoutube(downloadLink);

    return a.exec();
}
