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



#ifndef video_YOUTUBE_H
#define video_YOUTUBE_H

#include "video.h"

class video_youtube : public video
{
    Q_OBJECT

    private:
        QString _maxfmt;

    public:
        video_youtube();
        video* createNewInstance();

    public slots:
        virtual void processNetworkReply(QByteArray);
};

#endif // video_YOUTUBE_H
