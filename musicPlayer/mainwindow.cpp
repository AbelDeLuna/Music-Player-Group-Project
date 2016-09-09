#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _player = NULL;


}

MainWindow::~MainWindow()
{
    delete ui;
    _player = NULL;
}


void MainWindow::setPlayer(Player* player){
    this->_player = player;
    connect (_player,SIGNAL(playingNewSong()),this,SLOT(updateText()));
    //connect(_player, &Player::positionChanged, this, &MainWindow::on_positionChanged);
    //connect(_player, &Player::durationChanged, this, &MainWindow::on_durationChanged);

}

void MainWindow::on_PauseButton_clicked()
{
    _player->pause();
}

void MainWindow::on_PlayButton_clicked()
{
    _player->play();
}

void MainWindow::on_NextButton_clicked()
{
    _player->next();
}

void MainWindow::on_PreviousButton_clicked()
{
    _player->previous();
}

void MainWindow::on_VolumeSlider_sliderMoved(int position)
{
    _player->setVolume(position);
    ui->Volume->setNum(position);
}

void MainWindow::on_ProgressSlider_sliderMoved(int position)
{
    //_player->setPosition(position);
}


void MainWindow::on_durationChanged(qint64 position)
{
    //ui->Volume->setMaximum(position);
}

void MainWindow::on_positionChanged(qint64 position)
{
    //ui->Duration->setValue(position);
}

void MainWindow::updateText(){
    ui->SongName->setText(_player->getSongName());
    int numberOfSeconds;
    int numberOfMinutes = 0;
    numberOfSeconds = _player->getDuration();
    numberOfSeconds = numberOfSeconds / 1000;
        while (numberOfSeconds > 60)
        {
            numberOfSeconds = numberOfSeconds - 60;
            numberOfMinutes++;
        }
    ui->MinutesLabel->setNum(numberOfMinutes);
    ui->ColonLabel->setText(":");
    ui->SecondsLabel->setNum(numberOfSeconds);
}
