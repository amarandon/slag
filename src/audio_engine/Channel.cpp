/*
 *   $Id: Channel.cpp,v 1.31 2007/01/28 08:13:58 rhizome Exp $
 *
 *      Copyright (C) 2004-2008 Alex Marandon
 *
 *  This file is part of slag, a pattern-based audio sequencer.
 *
 *  slag is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  slag is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with slag; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <qregexp.h>
#include <qdir.h>

#include "../audio_IO/WavFile.h"
#include "Channel.h"

Channel::Channel(const QDomElement & dom_elem) 
    : AudibleElem(), SafelyDeletable(),
      current_sample_number(0),
      _name(dom_elem.attribute("id")),
      _audioFile(NULL),
      _audioData(NULL),
      _detailsVisible(false)
{
    if ( ! dom_elem.attribute("file").isNull() ) {
        setFile(new WavFile(dom_elem.attribute("file")));
        Q_ASSERT(_audioData != NULL);
    }


    if ( ! dom_elem.attribute("volume").isNull() ) 
        setVolume(dom_elem.attribute("volume").toInt());

    if ( ! dom_elem.attribute("balance").isNull() ) 
        setBalance(dom_elem.attribute("balance").toInt());

    if ( ! dom_elem.attribute("row_index").isNull() ) 
        setRowIndex(dom_elem.attribute("row_index").toInt());

    if ( dom_elem.attribute("active") == "1" ) 
        unmute();
    else
        mute();

    if ( dom_elem.attribute("details_visible") == "1" ) {
        showDetails(true);
    } else {
        showDetails(false);
    }

    _changing.setValue(false);
}

Channel::Channel() 
    : AudibleElem(), SafelyDeletable(),
      current_sample_number(0),
      _name("new channel"),
      _audioFile(NULL),
      _audioData(NULL),
      _detailsVisible(false)
{
    setVolume(80);
    mute();
    _changing.setValue(false);
}

Channel::~Channel() {
    qDebug("Destruction channel " + _name);
    delete _audioData;
    delete _audioFile;
}

void Channel::setFile(AudioFile* file) {
    _audioFile = file;
    if (_audioData != NULL) {
        AudioData* audiodata_old = _audioData;
        delete audiodata_old;
    }

    _audioData = new AudioData;

    _audioFile->get_data(_audioData);
    emit modified();
}

void Channel::setFile(const QString& filename) {
    QMutexLocker mutex(&_mutex);
    setChanging(true);
    AudioFile* old_file = _audioFile;
    setFile(new WavFile(filename));
    delete old_file;
    setName(removeWavSuffix(_audioFile->name()));
    emit fileChanged();
    emit modified();
}

QDomElement Channel::getDom(QDomDocument& doc) {
    QDomElement dom_elem = doc.createElement("channel");
    dom_elem.setAttribute("id", _name);
    if (_audioFile != NULL) {
        dom_elem.setAttribute("file", 
                QDir::cleanDirPath(QDir().absFilePath(_audioFile->name())));
    }
    dom_elem.setAttribute("active", isActive());
    dom_elem.setAttribute("volume", volumePercent());
    dom_elem.setAttribute("balance", balance());
    dom_elem.setAttribute("row_index", _rowIndex);
    if (_detailsVisible)
        dom_elem.setAttribute("details_visible", 1);
    else
        dom_elem.setAttribute("details_visible", 0);
    return dom_elem;
}

void Channel::setChanging(bool b) {
    _changing.setValue(b); 
}

QString removeWavSuffix(QString filename) {
    filename.replace(QRegExp( "(.*/|.[wW][aA][vV])" ), "");
    return filename;
}

//EOF
