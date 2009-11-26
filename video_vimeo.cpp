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



#include "video_vimeo.h"

video_vimeo::video_vimeo()
{
    this->_name = "Vimeo";
    this->_supportsTitle = true;
    this->_supportsDescription = true;
    this->_supportsThumbnail = true;
    this->_supportsSearch = true;
    this->_icon = 0;
    this->_urlRegExp << QRegExp("http://(www\\.)*vimeo\\.com/[0-9]+", Qt::CaseInsensitive);
    _treeItem = NULL;
}

video* video_vimeo::createNewInstance()
{
    return new video_vimeo();
}


bool video_vimeo::setUrl(QString url)
{
    if (_url.isEmpty())
    {
        this->_url = QUrl(url.replace("vimeo.com/", "vimeo.com/moogaloop/load/clip:"));
        if (_url.isValid())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


void video_vimeo::processNetworkReply(QByteArray data)
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
                QString xml = QString::fromLocal8Bit(data, data.size());
                QRegExp expression;
                expression = QRegExp("<caption>(.*)</caption>");
                expression.setMinimal(true);
                if (expression.indexIn(xml) !=-1)
                {
                    _title = QString(expression.cap(1));
                    qDebug() << _title;
                    QRegExp expression2;
                    QRegExp expression3;

                    expression = QRegExp("<nodeId>(.*)</nodeId>");
                    expression.setMinimal(true);
                    expression2 = QRegExp("<request_signature>(.*)</request_signature>");
                    expression2.setMinimal(true);
                    expression3 = QRegExp("<request_signature_expires>(.*)</request_signature_expires>");
                    expression3.setMinimal(true);
                    if (expression.indexIn(xml)!=-1 && expression.cap(1) != "" && expression2.indexIn(xml)!=-1 && expression2.cap(1) != ""&& expression3.indexIn(xml)!=-1 && expression3.cap(1) != "")
                    {
                        videoQuality newQuality;
                        newQuality.quality = tr("high");
                        newQuality.videoUrl = "http://vimeo.com/moogaloop/play/clip:" + expression.cap(1) + "/" + expression2.cap(1) + "/" + expression3.cap(1);
                        qDebug() << newQuality.videoUrl;
                        _supportedQualities.append(newQuality);

                        QRegExp expression4;
                        expression4 = QRegExp("<isHD>(.*)</isHD>");
                        qDebug() << expression4.indexIn(xml)<< expression4.cap(1);
                        if (expression4.indexIn(xml)!=-1 && expression4.cap(1).trimmed() == "1" )
                        {
                            videoQuality newQuality;
                            newQuality.quality = tr("HD");
                            newQuality.videoUrl = "http://vimeo.com/moogaloop/play/clip:" + expression.cap(1) + "/" + expression2.cap(1) + "/" + expression3.cap(1)+"?q=hd";
                            qDebug() << newQuality.videoUrl;
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
