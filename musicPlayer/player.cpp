#include "player.h"
#include "download.h"
#include <QDebug>
#include <QMediaMetaData>
Player::Player(QObject *parent) : QObject(parent)
{
    stopBool = false;
    currentIndex=-1;
    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setVolume(50);
    connect(mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(playNextSong()));

}
Player::~Player(){
    delete mediaPlayer;
}

void Player::addSong(QString filePath){
    qDebug()<<"Added Song"<<filePath;
    musicList.append(filePath);
    //Intial procedure to set start playing songs
    if (musicList.size() == 1){
        startPlaylist();
        play();
    }
    //Automatically play songs when no more songs in the playlist
    else if (musicList.size()-2 == currentIndex && mediaPlayer->state() == QMediaPlayer::StoppedState){
        mediaPlayer->setMedia(QUrl::fromLocalFile(musicList[++currentIndex]));
        play();
    }

}

void Player::play(){
    qDebug()<<"Playing Song";
    mediaPlayer->play();
    emit playingNewSong();
}

void Player::pause(){
    qDebug()<<"Paused Song";
    mediaPlayer->pause();
}

void Player::previous(){
    if (currentIndex >= 1){
        stopBool = true;
        mediaPlayer->stop();
        mediaPlayer->setMedia(QUrl::fromLocalFile(musicList[--currentIndex]));
        play();
    }
    else
        //qDebug()<<"Cant go to previous song";
    stopBool = false;
}

void Player::next(){
    if (currentIndex < musicList.size()-1) {
        stopBool = true;
        mediaPlayer->stop();
        mediaPlayer->setMedia(QUrl::fromLocalFile(musicList[++currentIndex]));
        play();
    }
        //qDebug()<<"Cant go to next song";
    stopBool = false;
}

void Player::startPlaylist(){
    if (musicList.empty()){
      //  qDebug()<<"Music List is Empty";
        return;
    }
    currentIndex=0;
    mediaPlayer->setMedia(QUrl::fromLocalFile(musicList[currentIndex]));
}


void Player::playNextSong(){
    if (mediaPlayer->state() == QMediaPlayer::StoppedState && !stopBool){
        qDebug()<<"Stopped State";
        if (musicList.size()-1>currentIndex) { //Fails when going to be out of index
            next();
        }
    }
}

QString Player::getSongName(){
       QString title = mediaPlayer->metaData(QMediaMetaData::Title).toString();
       return title;
}

qint64 Player::getDuration(){
    qint64 duration =  mediaPlayer->metaData(QMediaMetaData::Duration).toLongLong();
    return duration;
}

void Player::setVolume(int newVolume){
    mediaPlayer->setVolume(newVolume);
}
