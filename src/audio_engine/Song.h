/*
 *
 *   $Id: Song.h,v 1.45 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef SONG_H
#define SONG_H

#include <vector>
#include <QStringList>
#include <QVector>
#include <QMap>

#include "../gui/StringListEdit.h"
#include "AudibleElem.h"
#include "SafeData.h"
#include "ChannelPtrList.h"

class QDomElement;
class Q3ProgressDialog;
class Pattern;
class Channel;
class Part;
class UntitledList;
class PartReader;

typedef QMap<QString, Channel*> NameToChanMap;
typedef QMap<QString, Pattern*> NameToPatternMap;

typedef std::vector<Pattern*> PatternList;
typedef std::vector<Pattern*>::iterator PatternIterator;

typedef QList<Part*> PartList;
typedef QList<Part*>::iterator PartIterator;

typedef std::vector<PartReader*> PartReaderList;
typedef std::vector<PartReader*>::iterator PartReaderIterator;

typedef QList<Channel*> ChannelList;
typedef QList<Channel*>::iterator ChannelIterator;

class Song : public AudibleElem {
    Q_OBJECT
public:
    Song(const QDomElement &, Q3ProgressDialog* = NULL);
    ~Song();

    QDomDocument getDom();

    void update();

    void addPattern(Pattern* new_pattern);
    QStringList patternNames();
    void updatePatternList(const StringItemList &);
    void addChannel();
    void deleteChannel(Channel*, PartReaderList&);

    inline uint tempo() { return _tempo.getValue(); };
    inline void setTempo(uint nb) { _tempo.setValue(nb); emit modified(); };

    inline uint nbPads() { return _nbPads; }
    inline void setNbPads(uint nb) { _nbPads = nb; emit modified(); }
    
    inline int  padsPerStep() { return _padsPerStep; }
    inline void setPadsPerStep(int n) { _padsPerStep = n; emit modified(); }

    inline PatternList& patterns() { return _patterns; }
    inline PatternList& oldPatterns() { return _old_patterns; }
    inline const NameToPatternMap& nameToPatternMap() { return _nameToPatternMap; }

    inline ChannelList& channels() { return _channels; }
    bool hasChannel(const QString&);
    inline const NameToChanMap& nameToChanMap() { return  _nameToChanMap; }
    inline uint channelsCount() { return _channels.count(); }

    Channel* channelByIndex(uint index);
    inline PartList& parts() { return _parts; }

    inline Pattern* getPatternByName(const QString& name) {
        return nameToPatternMap()[name];
    }

    void removePattern(Pattern*);
    void removeOldPatterns();
    void freeChannelName(const QString);
    const QString getNewPartName();
    void freePartName(const QString);
    UntitledList* unamedPatternList();
    void freePatternName(const QString);
    
signals:
    void channelAdded(Channel*);
    void channelRemoved(Channel*);
    void patternAdded(Pattern*);
    void patternRemoved(Pattern*);
    void partAdded(Part*);
    void channelMoved(Channel*);

private:
    uint _nbPads;
    SafeData<uint> _tempo;
    UntitledList* _unamedChanList;
    UntitledList* _unamedPartList;
    UntitledList* _unamedPatternList;

    int _padsPerStep;

    PatternList _patterns;
    PatternList _old_patterns;
    NameToPatternMap _nameToPatternMap;

    ChannelList _channels;
    NameToChanMap _nameToChanMap;

    PartList _parts;
};

#endif

//EOF
