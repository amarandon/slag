/*
 *   $Id: Pad.h,v 1.15 2007/01/28 08:13:58 rhizome Exp $
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

#ifndef PAD_H
#define PAD_H

#include "AudibleElem.h"

class QDomElement;
class QDomDocument;
class AudioData;

/// A Pad is a kind of note, represented by a button in the GUI
class Pad : public AudibleElem {
public:
    Pad();
    Pad(Pad&);
    Pad(const QDomElement &);
    ~Pad();
    QDomElement getDom(QDomDocument&);
    void printXml();
};

class PadReader {
    Pad * _pad;
    SafeData<bool> _empty;
public:
    PadReader(Pad * pad, AudioData* new_audiodata);
    ~PadReader();
    void setAudioData(AudioData* new_audiodata);
    sample_t* endptr;
    sample_t* bufptr;
    inline bool empty() { return _empty.getValue(); }; 
    inline void empty(bool val) { _empty.setValue(val); }; 
    inline sample_t volumeLeft() { return _pad->volumeLeft(); }
    inline sample_t volumeRight() { return _pad->volumeRight(); }
    void update();
    AudioData * audiodata;
};

#endif // PAD_H

//EOF
