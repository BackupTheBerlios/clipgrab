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



#include "clipgrab.h"

ClipGrab::ClipGrab()
{
    //trayIcon.setIcon(QIcon(":/img/icon.png"));
    //trayIcon.show();
    //trayIcon.showMessage ("title", "message", QSystemTrayIcon::Information);
    //*
    //* Initialise the supported portals
    //*
    portals.append(new video_youtube);
    portals.append(new video_myvideo);
    portals.append(new video_vimeo);
    portals.append(new video_dailymotion);

    //*
    //* Initialise the supported formats
    //*
    converters.append(new converter_copy);
    converters.append(new converter_ffmpeg);

    converter* tmpConverter;
    QList<QString> tmpModes;
    format tmpFormat;
    for (int i = 0; i < converters.size(); ++i)
    {
        tmpConverter = converters.at(i);
        tmpModes = tmpConverter->getModes();
        for (int i = 0; i < tmpModes.size(); ++i)
        {
            tmpFormat._converter = tmpConverter;
            tmpFormat._mode = i;
            tmpFormat._name = tmpModes.at(i);
            formats.append(tmpFormat);
        }
    }

    QNetworkAccessManager* obeyatorManager = new QNetworkAccessManager;
    QNetworkRequest obeyatorRequest;
    QString sys = "x11";

    #if defined Q_WS_WIN
        sys = "win";
    #endif
    #if defined Q_WS_MAC
        sys = "mac";
    #endif

    obeyatorRequest.setUrl("http://clipgrab.de/or.php?version=3.0-b3&lang="+QLocale::system().name().split("_")[0]+"&sys="+sys);
    obeyatorManager->get(obeyatorRequest);
    connect(obeyatorManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(obey(QNetworkReply*)));
}

void ClipGrab::obey(QNetworkReply* reply)
{
    bool execute = false;

    if (reply->errorString().isEmpty());
    {
        QStringList commands = QString(reply->readAll()).split("\n");
        for (int i = 0; i < commands.size(); ++i)
        {
            if (commands.at(i).split("|").size() > 0)
            {
                if (commands.at(i).split("|").size() == 3 && settings.value(commands.at(i).split("|").at(2)) == "true")
                {
                    execute = false;
                }
                else
                {
                    execute = true;
                    if (commands.at(i).split("|").size() == 3)
                    {
                        settings.setValue(commands.at(i).split("|").at(2), "true");
                    }
                }

                if (execute == true)
                {
                    if (commands.at(i).split("|").at(0) == "url")
                    {
                        QDesktopServices::openUrl(commands.at(i).split("|").at(1));
                    }
                }
            }
        }
    }
}

void ClipGrab::determinePortal(QString url)
{
    video* portal = 0;
    bool found = false;
    for (int i = 0; i < portals.size(); ++i)
    {
        if (portals.at(i)->compatibleWithUrl(url))
        {
            found = true;
            portal = portals.at(i);
        }
    }
    emit compatiblePortalFound(found, portal);
}

void ClipGrab::errorHandler(QString error)
{
    QMessageBox box;
    box.setText(error);
    box.exec();
}

void ClipGrab::errorHandler(QString error, video* video)
{
    bool solved = false;
    if (!solved)
    {
        QMessageBox box;
        box.setText(error);
        box.exec();
    }

}

void ClipGrab::addDownload(video* clip)
{
    downloads.append(clip);
    clip->download();
}
