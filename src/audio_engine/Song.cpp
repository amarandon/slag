/*
 *
 *   $Id: Song.cpp,v 1.64 2007/01/28 08:13:58 rhizome Exp $
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

#include <qregexp.h>
#include <qmessagebox.h>
#include <q3progressdialog.h>

#include "../constants.h"
#include "../config/Config.h"
#include "../audio_IO/AudioFileExceptions.h"
#include "Part.h"
#include "Pad.h"
#include "Channel.h"
#include "Pattern.h"
#include "UntitledList.h"
#include "Song.h"

bool channelPtrRowIndexSmaller(const Channel* channel1,
        const Channel* channel2) {
    return channel1->rowIndex() < channel2->rowIndex(); 
}

Song::Song(const QDomElement & dom_elem, Q3ProgressDialog* progress) : 
    _nbPads(0),
    _unamedChanList(new UntitledList(tr("Untitled "))),
    _unamedPartList(new UntitledList("")),
    _unamedPatternList(new UntitledList(tr("pattern ")))
{
    NamePartMap part_map;

    QDomNodeList first_level_nodes = dom_elem.childNodes();
    uint first_level_len = first_level_nodes.length();

    if (progress){
        uint channels_nb = 0;
        uint patterns_nb = 0;
        for (uint i = 0; i < first_level_len; ++i) {
            QString node_name = first_level_nodes.item(i).toElement().nodeName();
            if (node_name == "channel") {
                ++channels_nb;
            } else if (node_name == "pattern") {
                ++patterns_nb;
            }
            progress->setTotalSteps(channels_nb + patterns_nb);
        }
    }

    uint progress_step = 0;
    for (uint i = 0; i < first_level_len; ++i) {
        QDomElement first_level_elem = first_level_nodes.item(i).toElement();

        QString node_name = first_level_elem.nodeName();

        // Création des channels que l'on range dans un dictionnaire
        if (node_name == "channel") {
            QDomElement chan_decl_elem = first_level_elem;
            Channel* channel = NULL;
            try {
                channel = new Channel(chan_decl_elem);
                if (progress) {
                    progress->setProgress(++progress_step);
                }
                _unamedChanList->append(channel->name());
                connect(channel, SIGNAL(modified()), this, SIGNAL(modified()));
            } catch (audio_IO::AudioFileException & e) {
                QString errmsg = tr("Unable to open file %1.").arg(e.filename);
                qWarning(errmsg);
                QMessageBox::critical(NULL, tr("Slag error"), errmsg);
                delete channel;
                continue;
            }
            if (channel != NULL) { // XXX c'est zarb par ici
                _nameToChanMap[chan_decl_elem.attribute("id")] = channel;
            }
            emit channelAdded(channel);
            //// l'attribut id sert de clé au dico de channels
            _channels.append(channel);


        } else if (node_name == "part") {
            // creation des parts
            QDomElement part_elem = first_level_elem;
            Part * part = new Part(part_elem);
            _unamedPartList->append(part->name());
            connect(part, SIGNAL(modified()), this, SIGNAL(modified()));
            part_map[part_elem.attribute("id")] = part;
            QDomNodeList pad_nodes = part_elem.childNodes();


            setNbPads(pad_nodes.length());

            _parts.append(part);

        } else if (node_name == "pattern") {
            QDomElement pattern_elem = first_level_elem;
            Pattern * pattern = new Pattern(pattern_elem, _nameToChanMap, part_map);
            _unamedPatternList->append(pattern->name());
            connect(pattern, SIGNAL(modified()), this, SIGNAL(modified()));
            qDebug(tr("Creating pattern %1.").arg(pattern_elem.attribute("id")));

            _nameToPatternMap[pattern_elem.attribute("id")] = pattern;

        } else if (node_name == "song") {
            QDomElement song_elem = first_level_elem;

            // affectation du tempo de la song
            setTempo(song_elem.attribute("tempo").toInt());
            setVolume(song_elem.attribute("volume").toInt());
            QDomNodeList pattern_nodes = song_elem.childNodes();
            for (unsigned int i = 0; i < pattern_nodes.length(); ++i) {
                QDomElement pattern_elem = pattern_nodes.item(i).toElement();
                if ( _nameToPatternMap[pattern_elem.attribute("ref")] == NULL )
                    continue; 
                else
                    addPattern(_nameToPatternMap[pattern_elem.attribute("ref")]);
            }
        }
    }
    qDebug("Sorting channels");
    qSort(_channels.begin(), _channels.end(), channelPtrRowIndexSmaller);
}

Song::~Song() {
    qDebug("Destruction Song");
    NameToPatternMap::iterator end = _nameToPatternMap.end();
    for (NameToPatternMap::iterator it = _nameToPatternMap.begin(); it != end; ++it)
    {
        delete it.data();
    }
    delete _unamedChanList;
    delete _unamedPartList;
    delete _unamedPatternList;
    while (!_channels.isEmpty())
        delete _channels.takeFirst();
}

QDomDocument Song::getDom() {
    QDomDocument doc;
    QDomElement slag_elem = doc.createElement("slag");
    doc.appendChild(slag_elem);

    ChannelIterator channels_end = _channels.end();
    for(ChannelIterator it(_channels.begin()); 
            it != channels_end; ++it) {
        if ( not (*it)->toDelete() ) {
            slag_elem.appendChild((*it)->getDom(doc));
        }
    }

    foreach(Part* part, _parts) {
        if ( not part->toDelete() ) {
            slag_elem.appendChild(part->getDom(doc));
        }
    }

    NameToPatternMap::Iterator end = _nameToPatternMap.end();
    for(NameToPatternMap::Iterator it = _nameToPatternMap.begin(); 
            it != end; ++it) {
        Pattern* pattern = *it;
        if (not pattern->toDelete()) {
            slag_elem.appendChild(pattern->getDom(doc));
        }
    }

    QDomElement song_elem = doc.createElement("song");
    song_elem.setAttribute("tempo", tempo());
    song_elem.setAttribute("volume", volumePercent());
    slag_elem.appendChild(song_elem);

    for (PatternIterator it(_patterns.begin()); it != _patterns.end(); ++it)
    {
        QDomElement patt_elem = doc.createElement("pattern");
        patt_elem.setAttribute("ref", (*it)->name());
        song_elem.appendChild(patt_elem);
    }

    return doc;
}


void Song::update() { /// Run in audio processing thread

    QMutableListIterator<Channel*> channel_iter(_channels);
    while (channel_iter.hasNext()) {
        Channel* channel = channel_iter.next();
        if (channel->toDelete() and (not channel->used())) {
            channel_iter.remove();
            delete channel;
        }
    }

    QMutableListIterator<Part*> part_iter(parts());
    while (part_iter.hasNext()) {
        Part* part = part_iter.next();
        if (part->toDelete() && ! part->used()) {
            for (PatternIterator pattern_it(patterns().begin()); 
                    pattern_it != patterns().end(); ++pattern_it) {
                for(ChannelIterator chan_it(channels().begin()); 
                        chan_it != channels().end(); ++chan_it) {
                    if((*pattern_it)->part(*chan_it) == part) {
                        (*pattern_it)->setPart(*chan_it, NULL);
                    }
                }
            }
            part_iter.remove();
            delete part;
        }
    }
}

void Song::addPattern(Pattern* new_pattern) {
    _patterns.push_back(new_pattern);
    emit modified();
}

QStringList Song::patternNames() {
    QStringList names;
    for (PatternIterator it = _patterns.begin(); it != _patterns.end(); it++) {
        names.append((*it)->name());
    }
    return names;
}

void Song::updatePatternList(const StringItemList & items) {
    // Prise en compte des patterns renommés
    for (StringItemList::const_iterator it = items.begin(); 
            it != items.end(); ++it) {
        const StringItem* patternItem = &(*it);
        if (patternItem->isRenamed && ! patternItem->isNew) {
            _nameToPatternMap[patternItem->text()] = 
                _nameToPatternMap[patternItem->originalText];
            _nameToPatternMap.remove(patternItem->originalText);
            _nameToPatternMap[patternItem->text()]->setName(patternItem->text());
            Q_ASSERT(_nameToPatternMap[patternItem->text()] != NULL);
        }
    }

    // Ajout des nouveaux patterns
    for (StringItemList::const_iterator it = items.begin(); 
            it != items.end(); ++it) {
        QString newName = (*it).text();
        if(_nameToPatternMap[newName] == NULL) {
            _nameToPatternMap[newName] = new Pattern(newName);
            emit patternAdded(_nameToPatternMap[newName]);
        }
    }

    // Suppression des patterns supprimés
    NameToPatternMap::iterator end = _nameToPatternMap.end();
    for (NameToPatternMap::iterator it = _nameToPatternMap.begin(); 
            it != end; ++it) {

        bool found = false;

        Pattern* pattern = *it;

        for (StringItemList::const_iterator it2 = items.begin(); 
                it2 != items.end(); ++it2) {
            if ((*it2).text() == pattern->name()) {
                found = true;
                break;
            }
        }

        if (! found)  {
            pattern->scheduleDelete();
            _old_patterns.push_back(pattern);
        }
    }

    // Reconstitution de la liste de patterns
    _patterns.clear();
    for (StringItemList::const_iterator it = items.begin(); 
            it != items.end(); ++it) {
        QString name = (*it).text();
        Q_ASSERT(_nameToPatternMap[name] != NULL);
        _patterns.push_back(_nameToPatternMap[name]);
    }
}

void Song::removePattern(Pattern* pattern) {
    _nameToPatternMap.remove(pattern->name());
    emit patternRemoved(pattern);
    delete pattern;
    emit modified();
}

void Song::removeOldPatterns() {
    for (PatternIterator it(_old_patterns.begin()); 
            it != _old_patterns.end(); ++it) {
        removePattern(*it);
    }
    _old_patterns.clear();
}

void Song::addChannel() {
    Channel* channel = new Channel();
    if (channel != NULL) {

        // We give it a name
        channel->setName(_unamedChanList->getNewName());

        // And a row index
        channel->setRowIndex(_channels.count()); 
        
        _nameToChanMap[channel->name()] = channel;
    }
    _channels.append(channel); 
    emit channelAdded(channel);
    emit modified();;
}

bool Song::hasChannel(const QString& name) {
    ChannelIterator channels_end = _channels.end();
    for(ChannelIterator it(_channels.begin()); 
            it != channels_end; ++it) {
        if (name == (*it)->name()) {
            return true;
        }
    }
    return false;
}

/// Run in GUI thread
void Song::deleteChannel(Channel* channel, 
                         PartReaderList& current_part_readers) {
    uint removed_channel_index = channel->rowIndex();
    _unamedChanList->remove(channel->name());

    foreach (Channel* channel, _channels) { 
        if (channel->rowIndex() > removed_channel_index) {
            channel->setRowIndex(channel->rowIndex() - 1);
        }
    }

    foreach (Pattern* pattern, _patterns) {
        pattern->removeChannel(channel);
    }
    
    Config* cfg = Config::instance();
    if(cfg->audioDriverName() == JACK_LABEL && cfg->jackTrackOutput()) {
        /// connected to ChannelJackDriver::deleteChannel(Channel*)
        emit channelRemoved(channel); 
        for(PartReaderIterator it = current_part_readers.begin(); 
                it != current_part_readers.end();) {
            PartReader* part_reader = *it;
            if (part_reader->channel() == channel) {
                delete *it;
                current_part_readers.erase(it);
            } else {
                ++it;
            }
        }
        _channels.remove(channel);
    } else {
        channel->scheduleDelete();
    }

    emit modified();
}

Channel* Song::channelByIndex(uint index) {
    Channel* result = NULL;
    foreach (Channel* channel, channels()) {
        qWarning(QString("Channel %1 has index %2").arg(channel->name()).arg(channel->rowIndex()));
        if (channel->rowIndex() == index) {
            result = channel;
        }
    }
    Q_ASSERT(result != NULL);
    return result;
}

void Song::freeChannelName(const QString name) {
    _unamedChanList->remove(name);
}

const QString Song::getNewPartName() {
    return _unamedPartList->getNewName();
}

void Song::freePartName(const QString name) {
    _unamedPartList->remove(name);
}

UntitledList* Song::unamedPatternList() { 
    return _unamedPatternList; 
}

void Song::freePatternName(const QString name) {
    _unamedPatternList->remove(name);
}

//EOF
