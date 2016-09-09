#include "player.h"
#include "download.h"
#include <QDebug>
Player::Player(QObject *parent) : QObject(parent)
{
    currentIndex=-1;
    //playlist = new QMediaPlaylist();
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

}

void Player::play(){
    qDebug()<<"Playing Song";
    mediaPlayer->play();
}

void Player::pause(){
    qDebug()<<"Paused Song";
    mediaPlayer->pause();
}

void Player::previous(){
    //mediaPlayer->
}

void Player::next(){
    //mediaPlayer->
}

void Player::startPlaylist(){
    if (musicList.empty()){
        qDebug()<<"Music List is Empty";
        return;
    }
    currentIndex=0;
    mediaPlayer->setMedia(QUrl::fromLocalFile(musicList[currentIndex]));
}


void Player::playNextSong(){
    if (mediaPlayer->state() == QMediaPlayer::StoppedState){
        qDebug()<<"Stopped State";
        if (musicList.size()>currentIndex+1) { //Fails when going to be out of index
            mediaPlayer->setMedia(QUrl::fromLocalFile(musicList[++currentIndex]));
            play();
        }
    }
    else
        qDebug()<<"Not Stopped State";
}
