/*
 *   $Id: Channel.h,v 1.16 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef CHANNEL_H
#define CHANNEL_H

#include <qdom.h>
#include <qstring.h>
#include <qmutex.h>

#include "AudibleElem.h"
#include "SafelyDeletable.h"
#include "SafeData.h"

typedef unsigned int sample_count_t;

class AudioData;
class AudioFile;

class Channel : public AudibleElem, public SafelyDeletable {
    Q_OBJECT
public:
    Channel(const QDomElement &);
    Channel();
    ~Channel();
    void setFile(AudioFile*);
    void setFile(const QString&);
    QDomElement getDom(QDomDocument&);
    inline AudioData* audioData() { return _audioData; }
    inline AudioFile* audioFile() { return _audioFile; }
    inline QMutex& mutex() { return _mutex; }

    inline bool changing() { return _changing.getValue(); }
    void setChanging(bool);

    inline uint rowIndex() const { return _rowIndex; }
    inline void setRowIndex(uint i) { _rowIndex = i; emit indexChanged(); }
    inline void increaseRowIndex() { ++_rowIndex; emit indexChanged(); }
    inline void decreaseRowIndex() { --_rowIndex; emit indexChanged(); }

    inline const QString & name() const { return _name; }
    inline void setName(const QString & n) { _name = n; emit renamed(); }
    inline void showDetails(bool v) { _detailsVisible = v; }
    inline bool detailsShown() { return _detailsVisible; }

    sample_count_t current_sample_number;
signals:
    void fileChanged();
    void renamed();
    void indexChanged();
private:
    /// if the audio data assigned to this channel is changing
    SafeData<bool> _changing;  
    uint _rowIndex;
    QMutex _mutex;
    QString _name;
    AudioFile* _audioFile;
    AudioData* _audioData;
    bool _detailsVisible;
};

QString removeWavSuffix(QString);

#endif

//EOF 
