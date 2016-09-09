/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>
#include <QtGui>
#include <QtCore>

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QQueue>
#include <QList>

#include "httpwindow.h"
#include "ui_authenticationdialog.h"

#ifndef QT_NO_SSL
static const char defaultUrl[] = "";
#else
static const char defaultUrl[] = "";
#endif
static const char defaultFileName[] = "index0.mp3";


QMediaPlayer *player = new QMediaPlayer();//Our MP3 Player
QMediaPlaylist *playlist = new QMediaPlaylist();//Our Playlist
QList<QMediaContent> tempPlaylist;
QQueue<QUrl> songsThatNeedToBeDownloaded;//Used to allow multiple downloads to occur at the same time
QList<QString> songNameList;//List of all song names(given by user)
QMediaContent currentSong;//The current playing song (was going to use to hold player state)
QMediaContent newlyAddedSong;//Song that was just downloaded (was going to use to hold player state by adding to the songsThatNeedToBeAdded QQueue)
QMediaContent firstSongAdded;//The first song added (again, player state)
int songIndex = 0;//actual song index
int songNameIndex = 0;//song name index (to be used with the QList)
int totalNumberOfSongs = -1;//the total number of songs in the playlist(might be a way to do this via a QPlaylist method?)
bool songBeingDownloaded = false;

ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent)
  : QProgressDialog(parent)
{
    setWindowTitle(tr("Download Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}

HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
    , statusLabel(new QLabel(tr("Please enter the URL of a file you want to download.\n\n"), this))
    , songNameLabel(new QLabel(tr("Current Song Name Will Be Here")))
    , urlLineEdit(new QLineEdit(defaultUrl))
    , downloadButton(new QPushButton(tr("Download")))
    , previousButton(new QPushButton(tr("Previous")))
    , pauseButton(new QPushButton(tr("Pause")))
    , playButton(new QPushButton(tr("Play")))
    , nextButton(new QPushButton(tr("Next")))
    , launchCheckBox(new QCheckBox("Launch file"))
    , defaultFileLineEdit(new QLineEdit(defaultFileName))
    , downloadDirectoryLineEdit(new QLineEdit)
    , reply(Q_NULLPTR)
    , file(Q_NULLPTR)
    , httpRequestAborted(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("HTTP"));

    connect(&qnam, &QNetworkAccessManager::authenticationRequired,
            this, &HttpWindow::slotAuthenticationRequired);
#ifndef QT_NO_SSL
    connect(&qnam, &QNetworkAccessManager::sslErrors,
            this, &HttpWindow::sslErrors);
#endif

    QFormLayout *formLayout = new QFormLayout;
    urlLineEdit->setClearButtonEnabled(true);
    connect(urlLineEdit, &QLineEdit::textChanged,
            this, &HttpWindow::enableDownloadButton);
    formLayout->addRow(tr("&URL:"), urlLineEdit);
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (downloadDirectory.isEmpty() || !QFileInfo(downloadDirectory).isDir())
        downloadDirectory = QDir::currentPath();
    downloadDirectoryLineEdit->setText(QDir::toNativeSeparators(downloadDirectory));
    formLayout->addRow(tr("&Download directory:"), downloadDirectoryLineEdit);
    formLayout->addRow(tr("Default &file:"), defaultFileLineEdit);
    launchCheckBox->setChecked(true);
    formLayout->addRow(launchCheckBox);
    formLayout->addRow(songNameLabel);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);

    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);

    downloadButton->setDefault(true);
    connect(downloadButton, &QAbstractButton::clicked, this, &HttpWindow::downloadFile);

/**
    connect(downloadButton, &QAbstractButton::clicked, this, &HttpWindow::initializePlaylist);
*/


    pauseButton->setDefault(true);
    connect(pauseButton, &QAbstractButton::clicked, this, &HttpWindow::pausePlayer);
    playButton->setDefault(true);
    connect(playButton, &QAbstractButton::clicked, this, &HttpWindow::playPlayer);
    previousButton->setDefault(true);
    connect(previousButton, &QAbstractButton::clicked, this, &HttpWindow::previousPlayer);
    nextButton->setDefault(true);
    connect(nextButton, &QAbstractButton::clicked, this, &HttpWindow::nextPlayer);


    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(previousButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(playButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(pauseButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(nextButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    urlLineEdit->setFocus();
}

void HttpWindow::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;

    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &HttpWindow::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);

    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, &QProgressDialog::canceled, this, &HttpWindow::cancelDownload);
    connect(reply, &QNetworkReply::downloadProgress, progressDialog, &ProgressDialog::networkReplyProgress);
    connect(reply, &QNetworkReply::finished, progressDialog, &ProgressDialog::hide);
    progressDialog->show();

    statusLabel->setText(tr("Downloading %1...").arg(url.toString()));
    songBeingDownloaded = true;//
}

void HttpWindow::downloadFile()
{
    const QString urlSpec = "http://www.youtubeinmp3.com/fetch/?video=" + urlLineEdit->text().trimmed();
    if (urlSpec.isEmpty())
        return;

    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(urlSpec, newUrl.errorString()));
        return;
    }

    QString fileName = newUrl.fileName();
    if (fileName.isEmpty())
        fileName = defaultFileLineEdit->text().trimmed();
    if (fileName.isEmpty())
        fileName = defaultFileName;
    QString downloadDirectory = QDir::cleanPath(downloadDirectoryLineEdit->text().trimmed());
    if (!downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir())
        fileName.prepend(downloadDirectory + '/');
    if (QFile::exists(fileName)) {
        if (QMessageBox::question(this, tr("Overwrite Existing File"),
                                  tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(fileName),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
            == QMessageBox::No)
            return;
        QFile::remove(fileName);
    }

    file = openFileForWrite(fileName);
    if (!file)
        return;

    downloadButton->setEnabled(false);

    // schedule the request
    //Download QQueue
    if (songBeingDownloaded == true)//song is being downloaded currently
        songsThatNeedToBeDownloaded.enqueue(newUrl);
    else
        startRequest(newUrl);
    /**
    else if (!songsThatNeedToBeDownloaded.isEmpty())
    {
        songsThatNeedToBeDownloaded.enqueue(newUrl);
        while (!songsThatNeedToBeDownloaded.isEmpty() && songBeingDownloaded == false)
        {
        QUrl tempURL = songsThatNeedToBeDownloaded.dequeue();
        startRequest(tempURL);
        }
    }
    else
        startRequest(newUrl);
        */
}

QFile *HttpWindow::openFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
        return Q_NULLPTR;
    }
    return file.take();
}

void HttpWindow::cancelDownload()
{
    statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    downloadButton->setEnabled(true);
}

void HttpWindow::httpFinished()
{
    songBeingDownloaded = false;
    if (!songsThatNeedToBeDownloaded.isEmpty())
    {
    QUrl tempURL = songsThatNeedToBeDownloaded.dequeue();
    startRequest(tempURL);
    }
    QFileInfo fi;

    if (file) {
        fi.setFile(file->fileName());
        file->close();
        delete file;
        file = Q_NULLPTR;
    }

    if (httpRequestAborted) {
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        statusLabel->setText(tr("Download failed:\n%1.").arg(reply->errorString()));
        downloadButton->setEnabled(true);
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = Q_NULLPTR;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) {
            downloadButton->setEnabled(true);
            return;
        }
        startRequest(redirectedUrl);
        return;
    }

    if (fi.size() < 21000){
        statusLabel->setText("Error downloading song, try again or try a new link.");
}
    else{
    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
}
    songNameList.append(fi.fileName());

    if (launchCheckBox->isChecked()){
    //Just added these below so you aren't forced to have the launchBox checked
     //playlist->addMedia(QUrl::fromLocalFile(fi.absoluteFilePath()));//**********
     //player->setVolume(50);
     //player->setPlaylist(playlist);

     //All these if/elses were some of my attempts to hold the player state when songs are being downloaded
     /**
     if(songIndex == 0)
     {
        firstSongAdded = QUrl::fromLocalFile(fi.absoluteFilePath());
     }

     if (QMediaPlayer::NoMedia == 1)//playing (0 = stopped, 1 = playing, 2 = paused)
     {
         if(!songsThatNeedToBeAdded.isEmpty()){
             while(!songsThatNeedToBeAdded.isEmpty())
             {
                 playlist->addMedia((songsThatNeedToBeAdded.dequeue()));
             }
            player->setPlaylist(playlist);
         }
         else{
             player->setPlaylist(playlist);
         }
     }
     else
     {
         songsThatNeedToBeAdded.enqueue(QUrl::fromLocalFile(fi.absoluteFilePath()));
     }
     */
}
    //newlyAddedSong = QUrl::fromLocalFile(fi.absoluteFilePath());
    //player->setMedia(currentSong);
    tempPlaylist.append(QUrl::fromLocalFile(fi.absoluteFilePath()));
    playlist->addMedia(QUrl::fromLocalFile(fi.absoluteFilePath()));
    player->setVolume(50);
    player->setPlaylist(playlist);
    downloadButton->setEnabled(true);
    //songIndex++;
    totalNumberOfSongs++;
}

void HttpWindow::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

void HttpWindow::slotAuthenticationRequired(QNetworkReply*,QAuthenticator *authenticator)
{
    QDialog authenticationDialog;
    Ui::Dialog ui;
    ui.setupUi(&authenticationDialog);
    authenticationDialog.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), url.host()));

    // Did the URL have information? Fill the UI
    // This is only relevant if the URL-supplied credentials were wrong
    ui.userEdit->setText(url.userName());
    ui.passwordEdit->setText(url.password());

    if (authenticationDialog.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
}

#ifndef QT_NO_SSL
void HttpWindow::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("One or more SSL errors has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
#endif




void HttpWindow::pausePlayer()
{
    player->pause();
}
void HttpWindow::playPlayer()
{   
    //Again, these if/elses were attempts to get the player to hold it's state
    /**
    if(QMediaPlayer::NoMedia == 1)
    {
        player->setMedia(firstSongAdded);
        songNameLabel->setText(songNameList[songNameIndex]);
        player->play();
    }
    else{
    currentSong = player->currentMedia();
    player->setMedia(currentSong);
    //currentSong = playlist->currentMedia();
    //player->setMedia(currentSong);
    //currentSong = (QMediaContent)songPlaylist->dequeue();
    songNameLabel->setText(songNameList[songNameIndex]);
    player->play();
    }
    */
    songNameLabel->setText(songNameList[songNameIndex]);
    //player->setMedia(tempPlaylist[songIndex]);
    player->play();

}
void HttpWindow::previousPlayer()
{
    playlist->previous();
    //player->setPlaylist(playlist);*
    //currentSong = playlist->currentMedia();
    if (songNameIndex > 0){
    songNameIndex--;
    songIndex--;
    songNameLabel->setText(songNameList[songNameIndex]);
    player->play();
    }
    //player->play();
}
void HttpWindow::nextPlayer()
{
    playlist->next();
    //player->setPlaylist(playlist);*
    //currentSong = playlist->currentMedia();
    if (songNameIndex < totalNumberOfSongs){
    songNameIndex++;
    songIndex++;
    songNameLabel->setText(songNameList[songNameIndex]);
    player->play();
    }
    //player->play();
}
