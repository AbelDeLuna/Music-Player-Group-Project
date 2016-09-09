#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMediaPlayer>
#include <QDebug>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_Duration_sliderMoved(int position);

    void on_Volume_sliderMoved(int position);

    void on_Play_clicked();

    void on_Stop_clicked();

    void on_positionChanged(qint64 position);

    void on_durationChanged(qint64 position);

private:
    Ui::Dialog *ui;
    QMediaPlayer* player;
};

#endif // DIALOG_H
