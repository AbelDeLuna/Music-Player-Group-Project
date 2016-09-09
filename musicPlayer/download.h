#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>
#include <QtNetwork>

class Download : public QObject
{
    Q_OBJECT
public:
    explicit Download(QObject *parent = 0);
    void startRequest(const QUrl &requestedUrl);
signals:
    void finishedDownloading(QString filePath);
public slots:
    void downloadFile(QString link);
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void downloadYoutube(QString);

private:
    QFile *openFileForWrite(const QString &fileName);
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    QString downloadDirectory;
    bool httpRequestAborted;
    QString fileName;
};

#endif // DOWNLOAD_H
