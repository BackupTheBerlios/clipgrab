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



#include "video.h"

video::video()
{
    this->_networkAccessManager = new QNetworkAccessManager(this);
    _treeItem = NULL;
}

bool video::supportsSearch()
{
    if (_supportsSearch == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

QString video::getName()
{
    return _name;
}

QIcon* video::getIcon()
{
    if (_icon)

    {
        return _icon;
    }
    else
    {
        return new QIcon(QPixmap(":/img/icon.png"));
    }
}

bool video::compatibleWithUrl(QString url)
{
    for (int i = 0; i < this->_urlRegExp.size(); ++i)
    {
        if (this->_urlRegExp.at(i).indexIn(url)!=-1)
        {
            return true;
        }
    }

    return false;

}

bool video::setUrl(QString url)
{
    if (_url.isEmpty())
    {
        this->_url = QUrl(url);
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

void video::analyse()
{
    if (this->_url.isValid())
    {
        _step = 1;
        _redirectLevel = 0;
        QNetworkRequest request;
        request.setUrl(QUrl(this->_url));
        request.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; Konqueror/4.2; Linux) KHTML/4.1.4 (like Gecko) SUSE");
        this->_networkAccessManager->get(request);
        this->_redirectLevel = 0;
        connect(this->_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkReply(QNetworkReply*)));
    }
    else
    {
        emit error("Url is not valid");
    }
}

void video::handleNetworkReply(QNetworkReply* networkReply)
{
    if (!networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().isEmpty())
    {
        if (this->_redirectLevel<16)
        {
            this->_redirectLevel++;
            QUrl _url = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            qDebug() << "redirect:" << _url;
            networkReply->close();

            QNetworkRequest request;
            request.setUrl(_url);
            request.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; Konqueror/4.2; Linux) KHTML/4.1.4 (like Gecko) SUSE");
            QNetworkReply* reply = this->_networkAccessManager->get(request);
            connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(changeProgress(qint64, qint64)));
        }
        else
        {
            emit error("Too many redirects");
        }
    }
    else if (networkReply->error())
    {
        qDebug() << "error" << networkReply->errorString();
        QMessageBox box;
        box.setText(networkReply->errorString());
        box.exec();
        emit error(networkReply->errorString(), this);
        networkReply->close();
        this->processNetworkReply(0);
    }
    else
    {
        this->processNetworkReply(networkReply->readAll());
        networkReply->close();
    }
    networkReply->deleteLater();

}

void video::processNetworkReply(QByteArray){}

void video::changeProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (this->_treeItem)
    {
        if (bytesTotal > 0)
        {
            if (bytesReceived > _downloadProgress)
            {
                _downloadProgress = bytesReceived;
            }
            this->_progressBar->setMaximum(bytesTotal);
            this->_progressBar->setValue(bytesReceived);
        }
    }
    else
    {
        qDebug() << bytesReceived << "of" << bytesTotal;
    }
    emit progressChanged(bytesReceived, bytesTotal);
}

void video::setQuality(int quality)
{
    this->_quality = quality;
}

QString video::quality()
{
    return this->_supportedQualities.at(_quality).quality;
}

video* video::createNewInstance()
{
    return new video();
}

QList<QString> video::getSupportedQualities()
{
    QStringList result;
    for (int i = 0; i < _supportedQualities.size(); ++i)
    {
        result << _supportedQualities.at(i).quality;
    }
    return result;
}

QString video::title()
{
    return _title;
}

void video::download()
{
    _step = 2;
    _redirectLevel = 0;
    QNetworkRequest request;
    request.setUrl(QUrl(this->_supportedQualities.at(_quality).videoUrl));
    request.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; Konqueror/4.2; Linux) KHTML/4.1.4 (like Gecko) SUSE");
    QNetworkReply* reply = this->_networkAccessManager->get(request);
    this->_redirectLevel = 0;
    _networkAccessManager->disconnect();
    connect(this->_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkReply(QNetworkReply*)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(changeProgress(qint64, qint64)));
}

void video::setTreeItem(QTreeWidgetItem* item)
{
    this->_treeItem = item;
}

void video::setFormat(int format)
{
    this->_format = format;
}

void video::startConvert()
{
    this->_progressBar->setMinimum(0);
    this->_progressBar->setMaximum(0);
    connect(this->_converter, SIGNAL(conversionFinished()), this, SLOT(conversionFinished()));
    this->_converter->startConversion(&this->_videoData, _targetPath, _converterMode);

}

void video::setConverter(converter* converter, int mode)
{
    _converter = converter;
    _converterMode = mode;
}

void video::setTargetPath(QString target)
{
    _targetPath = target;
}

QString video::getSaveTitle()
{
    QString title = this->_title;
    if (title.size() > 0)
    {
        QStringList charsToBeRemoved;
        charsToBeRemoved << "\\" << "/" << ":" << "*" << "?" << "<" << ">" << "|" << "'" << "\"" << "&amp;quot;";
        title = title.simplified();

        if (title[0] == QChar('.'))
        {
            title = title.remove(0, 1);
        }
        if (title.size() > 0)
        {
            for (int i = 0; i < charsToBeRemoved.size(); ++i)
            {
                title = title.replace(charsToBeRemoved.at(i), "");
            }
        }
        else
        {
            return "download-empty-title";
        }
    }
    else
    {
        return "download-empty-title";
    }
    return title;
}

void video::conversionFinished()
{
    this->_progressBar->setMaximum(1);
    this->_progressBar->setValue(1);
    this->_progressBar->setFormat(tr("Finished"));
}
