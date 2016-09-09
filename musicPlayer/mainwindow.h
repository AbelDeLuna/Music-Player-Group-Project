#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <client.h>
#include <player.h>
#include <QMainWindow>
#include <QDialog>
#include <QMediaPlayer>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setPlayer(Player*);

private slots:
    void on_PauseButton_clicked();

    void on_PlayButton_clicked();

    void on_NextButton_clicked();

    void on_PreviousButton_clicked();

    void on_VolumeSlider_sliderMoved(int position);

    void on_ProgressSlider_sliderMoved(int position);

    void on_durationChanged(qint64 position);

    void on_positionChanged(qint64 position);

    void updateText();
private:
    Ui::MainWindow *ui;
    Player* _player;

};

#endif // MAINWINDOW_H
