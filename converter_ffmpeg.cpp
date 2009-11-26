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



#include "converter_ffmpeg.h"

converter_ffmpeg::converter_ffmpeg()
{
        this->_modes.append(tr("MPEG4"));
        this->_modes.append(tr("WMV (Windows)"));
        this->_modes.append(tr("OGG Theora"));
        this->_modes.append(tr("MP3 (audio only)"));
        this->_modes.append(tr("OGG Vorbis (audio only)"));

}




QString converter_ffmpeg::getExtensionForMode(int mode)
{
    switch (mode)
    {
        case 0:
            return "mp4";
            break;
        case 1:
            return "wmv";
            break;
        case 2:
            return "ogg";
            break;
        case 3:
            return "mp3";
            break;
        case 4:
            return "ogg";
            break;
    }
    return "";
}



void converter_ffmpeg::startConversion(QByteArray* data, QString target, int mode)
{
    QString ffmpegCall;
    switch (mode)
    {
    case 0:
        ffmpegCall = "ffmpeg -i - -y -qscale 0.1 -vcodec mpeg4 \""+target+".mp4\"";
        break;
    case 1:
        ffmpegCall = "ffmpeg -i - -y -qscale 0.1 -acodec wmav2 -ab 300k -vcodec wmv2 \""+target+".wmv\"";
        break;
    case 2:
        ffmpegCall = "ffmpeg -i - -y -qscale 0.1 -vcodec libtheora -acodec libvorbis -ac 2  \""+target+".ogg\"";
        break;
    case 3:
        ffmpegCall = "ffmpeg -i - -y -acodec libmp3lame -ab 300k \""+target+".mp3\"";
        break;
    case 4:
        ffmpegCall = "ffmpeg -i - -y -vn -acodec libvorbis -ac 2 \""+target+".ogg\"";
        break;
    }
    #ifdef Q_WS_MAC
        ffmpegCall.prepend(QApplication::applicationDirPath()+"/");
    #endif
    ffmpeg.data = *data;
    ffmpeg.ffmpegCall = ffmpegCall;
    connect(&ffmpeg, SIGNAL(finished()), this, SLOT(emitFinished()));
    ffmpeg.start();

}


converter* converter_ffmpeg::createNewInstance()
{
    return new converter_ffmpeg();
}

void converter_ffmpeg::emitFinished()
{
    emit conversionFinished();
}
