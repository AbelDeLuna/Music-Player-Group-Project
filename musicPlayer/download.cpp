#include "download.h"
#include <QDebug>

#include <QGlobal.h>
#include <QTime>

Download::Download(QObject *parent) : QObject(parent)
{
    downloadDirectory = (QDir::toNativeSeparators(downloadDirectory));
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}


void Download::startRequest(const QUrl &requestedUrl){
    url = requestedUrl;
    httpRequestAborted = false;
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &Download::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &Download::httpReadyRead);
    qDebug()<<"Starting Download";
}

QFile* Download::openFileForWrite(const QString &fileName){
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        qDebug()<<"Unable to write to File";
        return Q_NULLPTR;
    }
    return file.take();
}

void Download::downloadYoutube(QString link){
    downloadFile("http://www.youtubeinmp3.com/fetch/?video=" + link);
}


void Download::downloadFile(QString link){
    const QString urlSpec = link;
    qDebug()<<urlSpec;
    if (link.isEmpty()){
        qDebug()<<"Parameter Empty";
        return;
    }
    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid()) {
        qDebug()<<"URL is invalid";
        return;
    }

    fileName = newUrl.fileName();
    qDebug()<<"File Name is"<<fileName;
    if (fileName.isEmpty())
        fileName = "default" + QString::number((qrand() % 1000000 +1)) + ".mp3";
    qDebug()<<"New File Name is"<<fileName;
    if (QFile::exists(fileName)) {
        qDebug()<<"File Already Exists";
    }

    file = openFileForWrite(fileName);
    if (!file)
        return;
    qDebug()<<"Starting Download Request";
    startRequest(newUrl);
}

void Download::cancelDownload(){

}

void Download::httpFinished(){
    qDebug()<<"Finished Downloading";
    QString filePath;
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        filePath = fi.absoluteFilePath();
        qDebug()<<filePath;
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
        qDebug()<<(reply->errorString());
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
            qDebug()<<"This got called";
            return;
        }
        qDebug()<<"Redirected to newURL";
        delete file;
        //startRequest(redirectedUrl);
        downloadFile(redirectedUrl.toEncoded());
        return;
        qDebug()<<"I SHOULD NEVER BE CALLED";
    }
    else{
    qDebug()<<"Saved File";
    emit finishedDownloading(filePath);
    }

}

void Download::httpReadyRead(){
    if (file)
        file->write(reply->readAll());
}
