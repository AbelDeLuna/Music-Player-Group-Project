#include "download.h"
#include <QDebug>

Download::Download(QString pathName, QObject *parent) : QObject(parent)
{
    downloadDirectory = (QDir::toNativeSeparators(pathName));
}

void Download::downloadFile(QString link)
{
    const QString urlSpec = "http://www.youtubeinmp3.com/fetch/?video=" + link;
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
        fileName = "default.mp3";

    if (QFile::exists(fileName)) {
        qDebug()<<"File Already Exists";
    }
    file = openFileForWrite(fileName);
    if (!file)
        return;
    qDebug()<<"Starting Download Request";
    startRequest(newUrl);
}

void Download::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;
    //delete reply; // was crashing program
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &Download::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &Download::httpReadyRead);
    qDebug()<<"Starting Download";
}

QFile* Download::openFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        qDebug()<<"Unable to write to File";
        return Q_NULLPTR;
    }
    return file.take();
}

void Download::cancelDownload()
{

}

void Download::httpFinished()
{
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
            return;
        }
        qDebug()<<"Redirected to newURL";
        startRequest(redirectedUrl);
        return;
    }
    else{
    qDebug()<<"Saved File";
    emit finishedDownloading(filePath);
    }
}

void Download::httpReadyRead()
{
    if (file)
        file->write(reply->readAll());
}
