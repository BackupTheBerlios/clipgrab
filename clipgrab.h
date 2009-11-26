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



#ifndef CLIPGRAB_H
#define CLIPGRAB_H

#include <QtGui>
#include <QtNetwork>

#include "video.h"
#include "video_youtube.h"
#include "video_myvideo.h"
#include "video_vimeo.h"
#include "video_dailymotion.h"


#include "converter.h"
#include "converter_copy.h"
#include "converter_ffmpeg.h"


struct format
{
    converter* _converter;
    QString _name;
    int _mode;
};


class ClipGrab : public QObject
{
    Q_OBJECT

    public:
    ClipGrab();

    QList<video*> portals;
    QList<format> formats;
    QList<video*> downloads;
    QList<converter*> converters;
    QSettings settings;
    QSystemTrayIcon trayIcon;

    public slots:
        void determinePortal(QString url);
        void errorHandler(QString);
        void errorHandler(QString, video*);
        void addDownload(video* clip);
        void obey(QNetworkReply* reply);

    signals:
        void compatiblePortalFound(bool, video* portal);

};

#endif // CLIPGRAB_H
