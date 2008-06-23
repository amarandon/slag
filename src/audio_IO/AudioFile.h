/*
 *   $Id: AudioFile.h,v 1.12 2007/01/26 07:39:55 rhizome Exp $
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
 */

/// Audio files abstract interface

#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <qstring.h>
#include "../common.h"
 
struct AudioData {
    sample_t* buffer;
    int nb_frames;
    ~AudioData() {
	qDebug("AudioData destruction.");
    };
};
  
class AudioFile 
{
    QString _name;
public:
    const QString& name() { return _name; }
    AudioFile(const char* filename) : _name(filename) {}
    virtual void get_data(AudioData*) = 0;
    inline virtual ~AudioFile() {
        qDebug(QString("AudioFile %1 destruction.").arg(_name));
    }
};
  
#endif

//EOF
