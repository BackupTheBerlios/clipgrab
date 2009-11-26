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



#include "video_dailymotion.h"

video_dailymotion::video_dailymotion()
{
    this->_name = "Dailymotion";
    this->_supportsTitle = true;
    this->_supportsDescription = true;
    this->_supportsThumbnail = true;
    this->_supportsSearch = true;
    this->_icon = 0;
    this->_urlRegExp << QRegExp("http://\\w*\\.dailymotion\\.com/.*video/.*", Qt::CaseInsensitive);
}

video* video_dailymotion::createNewInstance()
{
    return new video_dailymotion();
}

void video_dailymotion::processNetworkReply(QByteArray data)
{
    switch (this->_step)
    {
        case 1:
        //
        // Analysing the video
        //
        {
            QString newUrl = "http://www.youtube.com/get_video";
            QString html = QString::fromLocal8Bit(data, data.size());
            QRegExp expression;
            expression = QRegExp("<h1 class=\"dmco_title\">(.*)</h1>");
            expression.setMinimal(true);
            if (expression.indexIn(html) !=-1)
            {
                _title = QString(expression.cap(1)).replace("Dailymotion - ", "");

                expression = QRegExp("addVariable\\(\"video\", \"(.*)\"\\)");
                expression.setMinimal(true);
                if (expression.indexIn(html)!=-1)
                {
                    QStringList qualityLinks = QUrl::fromEncoded(expression.cap(1).toAscii()).toString(QUrl::None).split("|");
                    if (!qualityLinks.filter("@@spark").isEmpty())
                    {
                        videoQuality newQuality;
                        newQuality.quality = tr("normal");
                        newQuality.videoUrl = qualityLinks.filter("@@spark").first().split("|").last();
                        _supportedQualities.append(newQuality);
                     }
                    if (!qualityLinks.filter("@@vp6-hd").isEmpty())
                    {
                        videoQuality newQuality;
                        newQuality.quality = tr("HD");
                        newQuality.videoUrl = qualityLinks.filter("@@vp6-hd").first().split("|").last();
                        _supportedQualities.append(newQuality);
                     }
                    if (!qualityLinks.filter("@@vp6-hq").isEmpty())
                    {
                        videoQuality newQuality;
                        newQuality.quality = tr("high");
                        newQuality.videoUrl = qualityLinks.filter("@@vp6-hq").first().split("|").last();
                        _supportedQualities.append(newQuality);
                     }
                    else if (!qualityLinks.filter("@@h264-hq").isEmpty())
                    {
                        videoQuality newQuality;
                        newQuality.quality = tr("high");
                        newQuality.videoUrl = qualityLinks.filter("@@h264-hq").first().split("|").last();
                        _supportedQualities.append(newQuality);
                     }
                    else if (!qualityLinks.filter("@@h264").isEmpty())
                    {
                        videoQuality newQuality;
                        newQuality.quality = tr("high");
                        newQuality.videoUrl = qualityLinks.filter("@@h264").first().split("|").last();
                        _supportedQualities.append(newQuality);
                     }
               }
                else
                {
                    emit error("Could not retrieve video link.", this);
                }
            }
            else
            {
                emit error("Could not retrieve video title.", this);

            }
            emit analysingFinished();
            break;
        }

        case 2:
        {
            this->_videoData = data;
            emit downloadFinished();
        }

    }
}
