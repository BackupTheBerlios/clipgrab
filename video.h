/*
    ClipGrab³
    Copyright (C) Philipp Schmieder
    http://clipgrab.de
    feedback [at] clipgrab [dot] de

    This file is part of ClipGrab.
    ClipGrab is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    ClipGrab is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ClipGrab.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef VIDEO_H
#define VIDEO_H

#include <QtGui>
#include <QtNetwork>
#include "converter.h"

struct videoQuality
{
    QString quality;
    QString audioCodec;
    QString videoCodec;
    QString videoUrl;

    videoQuality()
    {
        quality = "standard";
    }

    videoQuality(QString Quality, QString VideoUrl)
    {
        quality = Quality;
        videoUrl = VideoUrl;
    }
};

class video : public QObject
{
    Q_OBJECT
public:
    video();

    virtual video* createNewInstance();

    //*
    //*Portal Information
    //*
    QIcon* getIcon();
    QString getName();
    bool supportsSearch();
    bool compatibleWithUrl(QString);

    //*
    //*Portal Access
    //*
    QString getSearch(QString);

    //*
    //*Video Access
    //*
    virtual bool setUrl(QString);
    virtual void analyse();
    void download();
    void setQuality(int);
    QString quality();
    QString title();
    void setTreeItem(QTreeWidgetItem* item);
    virtual QList<QString> getSupportedQualities();
    void setFormat(int format);
    QProgressBar* _progressBar; //fixme!
    void setTargetPath(QString target);
    QString getSaveTitle();

    void setConverter(converter* converter, int mode);


protected:
    //*
    //*Portal Information
    //*
    QString _name;
    QList<QRegExp> _urlRegExp;
    QIcon* _icon;
    bool _supportsTitle;
    bool _supportsDescription;
    bool _supportsThumbnail;
    bool _supportsSearch;
    QList<videoQuality> _supportedQualities;
    converter* _converter;
    int _converterMode;

    //*
    //*Video Informatoin
    //*
    QUrl _url;
    QUrl _urlThumbnail;
    float _downloadProgress;
    QString _targetPath;

    QString _title;

    int _format;
    int _quality;
    QByteArray _videoData;

    //*
    //*Processing
    //*
    QNetworkAccessManager* _networkAccessManager;
    int _step;
    int _redirectLevel;
    virtual void processNetworkReply(QByteArray);

    QTreeWidgetItem* _treeItem;

    protected slots:
        virtual void handleNetworkReply(QNetworkReply*);
        void changeProgress(qint64, qint64);
        virtual void startConvert();
        void conversionFinished();

    signals:
        void error(QString);
        void error(QString, video*n);
        void progressChanged(int, int);
        void stateChanged(QString);
        void downloadFinished();
        void analysingFinished();


};

#endif // ABSTRACT_video_H
