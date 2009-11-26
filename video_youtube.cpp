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



#include "video_youtube.h"

video_youtube::video_youtube()
{
    this->_name = "Youtube";
    this->_supportsTitle = true;
    this->_supportsDescription = true;
    this->_supportsThumbnail = true;
    this->_supportsSearch = true;
    this->_icon = 0;
    this->_urlRegExp << QRegExp("http://\\w*\\.youtube\\.com/watch\\?v\\=.*", Qt::CaseInsensitive);
    this->_urlRegExp << QRegExp("http://\\w*\\.youtube\\.com/view_play_list\\?p\\=.*&v\\=.*", Qt::CaseInsensitive);
}

video* video_youtube::createNewInstance()
{
    return new video_youtube();
}

void video_youtube::processNetworkReply(QByteArray data)
{
    if (!data.isEmpty())
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

                expression = QRegExp("<meta name=\"title\" content=\"(.*)\"");
                expression.setMinimal(true);
                if (expression.indexIn(html) !=-1)
                {
                    _title = QString(expression.cap(1)).replace("&amp;quot;", "\"").replace("&amp;amp;", "&");

                    expression = QRegExp("\"fmt_url_map\": \"(.*)\"");
                    expression.setMinimal(true);
                    if (expression.indexIn(html)!=-1 && expression.cap(1) != "")
                    {
                        QStringList qualityLinks = QUrl::fromEncoded(expression.cap(1).toAscii()).toString(QUrl::None).split(",");
                        if (!qualityLinks.filter("22|").isEmpty())
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("HD");
                            newQuality.videoUrl = QUrl::fromEncoded(qualityLinks.filter("22|").first().split("|").last().toAscii()).toString(QUrl::None);
                            _supportedQualities.append(newQuality);
                         }
                        if (!qualityLinks.filter("18|").isEmpty())
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("high");
                            newQuality.videoUrl = QUrl::fromEncoded(qualityLinks.filter("18|").first().split("|").last().toAscii()).toString(QUrl::None);
                            _supportedQualities.append(newQuality);
                        }
                        else if (!qualityLinks.filter("35|").isEmpty())
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("high");
                            newQuality.videoUrl = QUrl::fromEncoded(qualityLinks.filter("35|").first().split("|").last().toAscii()).toString(QUrl::None);
                            _supportedQualities.append(newQuality);
                        }
                        if (!qualityLinks.filter("34|").isEmpty())
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("normal");
                            newQuality.videoUrl = QUrl::fromEncoded(qualityLinks.filter("34|").first().split("|").last().toAscii()).toString(QUrl::None);
                            _supportedQualities.append(newQuality);
                         }
                        if (_supportedQualities.isEmpty())
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("normal");
                            newQuality.videoUrl = QUrl::fromEncoded(qualityLinks.first().split("|").last().toAscii()).toString(QUrl::None);
                            _supportedQualities.append(newQuality);
                        }
                    }
                    else if (expression.indexIn(html)!=-1 && expression.cap(1) == "")
                    {
                        expression = QRegExp("\"t\": \"(.*)\"");
                        expression.setMinimal(true);
                        QRegExp expression2 = QRegExp("\"video_id\": \"(.*)\"");
                        expression2.setMinimal(true);
                        if (expression.indexIn(html) !=-1 && expression2.indexIn(html) !=-1)
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("normal");
                            newQuality.videoUrl = QUrl::fromEncoded(QString("http://www.youtube.com/get_video?video_id=" + expression2.cap(1) + "&t=" + expression.cap(1)).toAscii()).toString(QUrl::None);
                            _supportedQualities.append(newQuality);
                        }
                        else
                        {
                            emit error("Could not retrieve video link.", this);

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
    else
    {
        switch (this->_step)
        {
        case 1:
            emit analysingFinished();
        case 2:
            emit downloadFinished();
        }
    }
}
