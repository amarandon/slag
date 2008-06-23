/*
 *
 *   $Id: SongReader.h,v 1.22 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef SONG_READER_H
#define SONG_READER_H

#include <vector>

#include <qmutex.h>
#include <q3ptrlist.h>
#include <qstringlist.h>
#include <qdom.h>

#include "../gui/StringListEdit.h"
#include "../config/Config.h"
#include "../common.h"
#include "AudibleElem.h"
#include "SafeData.h"
#include "ChannelPtrList.h"

class Pattern;
class Part;
class PartReader;
class Channel;
class Song;

typedef QMap<QString, Channel*> NameToChanMap;
typedef QMap<QString, Pattern*> NameToPatternMap;

typedef std::vector<PartReader*> PartReaderList;
typedef std::vector<PartReader*>::iterator PartReaderIterator;

typedef std::vector<Pattern*> PatternList;
typedef std::vector<Pattern*>::iterator PatternIterator;

class SongReader : public QObject {
    Q_OBJECT
private:
    Song* _song;
    uint current_pad_position;
    PatternIterator patternIterator;
    PartReaderList current_part_readers;
    sample_count_t buf_size;
    bool pattern_mode;
    Pattern* current_pattern;
    Pattern* next_wanted_pattern;
    QMutex pattern_mutex;
    bool is_playing;
    sample_count_t current_sample_number;

    void deletePartReaders();
    void handlePatternEnd();
    void cleanCurrentPartReaders();
    void processPartReaders(sample_t*, const sample_count_t, 
                            const sample_count_t, Channel*);
public:
    SongReader(Song* song, int b_size);
    ~SongReader();

    Song* song();
    void update(Channel*);
    void read(sample_t * client_buffer, sample_count_t buffer_n_samples, 
              Channel* channel = NULL);
    void updatePatternList(const StringItemList &);
    void deleteChannel(Channel*);

    inline void setPlaying(bool v) { is_playing = v; }
    inline bool isPlaying() { return is_playing; }
    inline void setLoopMode(bool v) { Config::instance()->loop(v); }
    inline void setBufferSize(int size) { buf_size = size; }

public slots:
    void init();
    void setPatternMode();
    inline bool patternMode() { return pattern_mode; }
    inline void setSongMode() { pattern_mode = false; }
    void setNextWantedPattern(const QString &);

signals:
    void patternChanged(const QString &);
    void channelAdded(Channel*);
    void patternAdded(Pattern*);
    void patternRemoved(Pattern*);
    void updated();
    void stop();
};

#endif // SONG_H

//EOF
