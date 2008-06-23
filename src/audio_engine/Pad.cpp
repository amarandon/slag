/*
 *   $Id: Pad.cpp,v 1.19 2007/01/28 08:13:58 rhizome Exp $
 *
 *      Copyright (C) 2004-2007 Alex Marandon
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

#include <qdom.h>

#include "Pad.h"
#include "../common.h"
#include "../audio_IO/AudioFile.h"

////////////////////////////////////////////////////////////////////////

AudibleElem::~AudibleElem () {
#ifdef DEBUG
    qDebug("AudibleElem destructor");
#endif
}

////////////////////////////////////////////////////////////////////////

Pad::Pad() {
    setActive(0);
}

Pad::Pad(Pad & orig_pad) : AudibleElem() {
    setActive(orig_pad.isActive());
    setVolume(orig_pad.volumePercent());
}

Pad::Pad(const QDomElement & dom_elem) {
    if( dom_elem.attribute("active") == "1" ) 
        setActive(1);
    else
        setActive(0);

    if ( ! dom_elem.attribute("volume").isNull()) {
        setVolume(dom_elem.attribute("volume").toInt());
    }

    if ( ! dom_elem.attribute("balance").isNull()) {
        setBalance(dom_elem.attribute("balance").toInt());
    }
}

Pad::~Pad() {
    //Logger::get_logger()->debug("Destruction Pad");
}

QDomElement Pad::getDom(QDomDocument& doc) {
    QDomElement dom_elem = doc.createElement("pad");
    dom_elem.setAttribute("active", isActive());
    dom_elem.setAttribute("volume", volumePercent());
    dom_elem.setAttribute("balance", balance());
    return dom_elem;
}

////////////////////////////////////////////////////////////////////////

PadReader::PadReader(Pad* pad, AudioData* new_audiodata) 
    : _pad(pad), _empty(0)
{
    setAudioData(new_audiodata);
}

PadReader::~PadReader() {
    //Logger::get_logger()->debug("Destruction PadReader");
    //delete audiodata;
}

void PadReader::update() {
    if ( bufptr == endptr && _pad->isActive() ) {
        bufptr = audiodata->buffer;  // pointeur de parcours revient au debut
        empty(false);
    }
}

void PadReader::setAudioData(AudioData* new_audiodata) {
    Q_ASSERT(new_audiodata != NULL);
    audiodata = new_audiodata;
    // pointeur sur la fin de liste
    endptr = audiodata->buffer + (audiodata->nb_frames * 2);	
    // pointeur de parcours initialise en fin de liste
    bufptr = endptr;	
}

//EOF
