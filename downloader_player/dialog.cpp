#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);

    connect(player, &QMediaPlayer::positionChanged, this, &Dialog::on_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &Dialog::on_durationChanged);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_Duration_sliderMoved(int position)
{
    player->setPosition(position);
}

void Dialog::on_Volume_sliderMoved(int position)
{
    player->setVolume(position);
}

void Dialog::on_Play_clicked()
{
        //load file and play it
    player->setMedia(QUrl::fromLocalFile("C:/Users/User/Downloads/roar.mp3"));
    player->play();
    qDebug() << player->errorString();
}

void Dialog::on_Stop_clicked()
{
    player->stop();
}

void Dialog::on_positionChanged(qint64 position)
{
    ui->Duration->setValue(position);
}

void Dialog::on_durationChanged(qint64 position)
{
    ui->Volume->setMaximum(position);
}
