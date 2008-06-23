/*
 *   $Id: SongReader.cpp,v 1.26 2007/01/28 08:13:58 rhizome Exp $
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

#include "SongReader.h"

#include "../audio_IO/AudioFile.h"
#include "Song.h"
#include "Pattern.h"
#include "Part.h"
#include "Pad.h"
#include "Channel.h"

SongReader::SongReader(Song* song, int b_size) : 
    _song(song),
    current_pad_position(1), 
    patternIterator(_song->patterns().begin()),
    buf_size(b_size),
    pattern_mode(true),
    current_pattern(NULL),
    next_wanted_pattern(*patternIterator),
    is_playing(false),
    current_sample_number(0)
{ 
    connect(_song, SIGNAL(channelAdded(Channel*)), 
            this, SIGNAL(channelAdded(Channel*)));
    connect(_song, SIGNAL(patternAdded(Pattern*)), 
            this, SIGNAL(patternAdded(Pattern*)));
    connect(_song, SIGNAL(patternRemoved(Pattern*)), 
            this, SIGNAL(patternRemoved(Pattern*)));
}

SongReader::~SongReader() {
    qDebug("Destruction SongReader");
    deletePartReaders();
    delete _song;
}

Song* SongReader::song() { 
    return _song; 
}

void SongReader::updatePatternList(const StringItemList & items) {
    QMutexLocker pattern_locker(&pattern_mutex);
    PatternList saved_pattern_list;
    for (PatternIterator it(_song->patterns().begin()); 
            it != _song->patterns().end(); ++it) {
        saved_pattern_list.push_back(*it);
    }
    Pattern* saved_current_pattern = *patternIterator;
    _song->updatePatternList(items);

    // handle when all patterns have been deleted
    uint kept_patterns = 0;
    for (PatternIterator it(saved_pattern_list.begin()); 
            it != saved_pattern_list.end(); ++it) {
        for (PatternIterator it2(_song->patterns().begin()); 
                it2 != _song->patterns().end(); ++it2) {
            if (*it == *it2) {
                ++kept_patterns;
            }
        }// FIXME: utiliser un algorithme tout fait
    } 

    if (kept_patterns == 0) {
        // No position to preserve, the current pattern just become the first
        // one
        patternIterator = _song->patterns().begin();
    } else {
        // handle deleted patterns: if the current pattern has been deleted, the
        // current pattern becomes the first non deleted before
        for (PatternIterator it(saved_pattern_list.begin()); 
                it != saved_pattern_list.end(); ++it) {
            while(saved_current_pattern->toDelete() 
                    && *it == saved_current_pattern) {
                if (it == saved_pattern_list.begin()) {
                    it = saved_pattern_list.end() - 1;
                } else {
                    --it;
                }
                saved_current_pattern = *it;
            }
        }

        // make pattern iterator to point to the actual current pattern
        patternIterator = _song->patterns().begin();
        for (PatternIterator it(_song->patterns().begin()); 
                it != _song->patterns().end(); ++it) {
            if(*patternIterator == saved_current_pattern) {
                break;
            } else {
                ++patternIterator;
            }
        }
    }

    if(next_wanted_pattern->toDelete()) {
        next_wanted_pattern = *patternIterator;
    }
}

void SongReader::setNextWantedPattern(const QString & new_pattern_name) {
    next_wanted_pattern = _song->getPatternByName(new_pattern_name);
}

void SongReader::deletePartReaders() {
    for(PartReaderIterator it = current_part_readers.begin(); 
            it != current_part_readers.end();) {
        delete *it;
        current_part_readers.erase(it);
    }
}

void SongReader::setPatternMode() { 
    pattern_mode = true; 
    if (is_playing) {
        next_wanted_pattern = current_pattern;
    }
}

void SongReader::init() {
    current_pad_position = 1;
    qDebug("SongReader::init()");
    patternIterator = _song->patterns().begin();
    if( pattern_mode && next_wanted_pattern != NULL) {
        current_pattern = next_wanted_pattern;
        while (*patternIterator != current_pattern) {
            ++patternIterator;
        }
    } else {
        current_pattern = *patternIterator;
    }
    // vidage de la liste des part_readers qui resteraient
    deletePartReaders();

    // ajout des part_readers correspondants aux parts de la premiere pattern
    ChannelIterator channels_end = _song->channels().end();
    for(ChannelIterator it(_song->channels().begin()); 
            it != channels_end; ++it) {
        Part* part = current_pattern->part(*it);
        if (part != NULL) {
            Q_ASSERT((*it)->audioData() != NULL);
            current_part_readers.push_back(new PartReader(part, *it));
        }
    }
    emit patternChanged(current_pattern->name());
}

void SongReader::deleteChannel(Channel* channel) {
    _song->deleteChannel(channel, current_part_readers);
}

void SongReader::cleanCurrentPartReaders() {
    for(PartReaderIterator it = current_part_readers.begin(); 
            it != current_part_readers.end();) {
        if ( (*it)->empty() ) {
            delete *it;
            current_part_readers.erase(it);
        } else {
            (*it)->setOld(true);
            ++it;
        }
    }
}

void SongReader::handlePatternEnd() {
    QMutexLocker pattern_locker(&pattern_mutex);

    // on est arrivé à la fin du pattern
    current_pad_position = 1; // réinitialisation du compteur de pad

    bool current_pattern_deleted = current_pattern->toDelete();

    // delete old patterns
    _song->removeOldPatterns();

    //cleanCurrentPartReaders();

    if (! pattern_mode) { // si on est en mode song
        if (patternIterator + 1 == _song->patterns().end()) {
            // si on est arrivé à la fin du dernier pattern
            if(Config::instance()->loop())
                // on boucle en revenant au premier
                patternIterator = _song->patterns().begin();
            else // ou bien on s'arrête là
                emit stop();
        } else { // sinon on passe au suivant
            ++patternIterator;
        }
        current_pattern = *patternIterator;
        emit patternChanged(current_pattern->name());

    } else if (current_pattern_deleted) {
        if (patternIterator + 1 == _song->patterns().end()) {
            patternIterator = _song->patterns().begin();
        } else { // sinon on passe au suivant
            ++patternIterator;
        }
        current_pattern = *patternIterator;
        emit patternChanged(current_pattern->name());

    } else if (next_wanted_pattern != NULL) {
        // passage au pattern qui vient d'être selectionné si on est en 
        // mode pattern
        current_pattern = next_wanted_pattern;
        emit patternChanged(current_pattern->name());
    }

    // ajout des part_readers correspondants aux parts de la pattern 
    // suivante
    ChannelList channels = _song->channels();
    ChannelIterator channels_end = channels.end();
    for(ChannelIterator it(channels.begin()); 
            it != channels_end; ++it) {
        Part* part = current_pattern->part(*it);
        if (part != NULL && ! (*it)->toDelete() && ! part->toDelete()) {
            PartReader* new_part_reader = new PartReader(part, *it);
            current_part_readers.push_back(new_part_reader);
        }
    }
}

void SongReader::update(Channel* channel) {

    for(PartReaderIterator it = current_part_readers.begin(); 
            it != current_part_readers.end(); ++it) {
        if (channel == NULL || (*it)->channel() == channel) { 
            (*it)->update();
        }
    }

    if (channel == NULL || channel == _song->channels().last()) {
        if(current_pad_position == _song->nbPads()) {
            cleanCurrentPartReaders();
        }
        _song->update();
        if(current_pad_position++ == _song->nbPads()) {
            handlePatternEnd();
        }
        emit updated();
    }
}

void SongReader::processPartReaders(sample_t* buffer, 
        const sample_count_t nb_samples_to_read, 
        const sample_count_t buffer_offset, Channel* channel) {

    sample_t mainVolumeLeft = _song->volumeLeft();
    sample_t mainVolumeRight = _song->volumeRight();

    QMap<Channel*, uint> partReadersNbPerChannel;
    for(PartReaderIterator part_reader_iter(current_part_readers.begin()); 
            part_reader_iter != current_part_readers.end(); ++part_reader_iter) {

        Channel* my_channel = (*part_reader_iter)->channel();

        if(partReadersNbPerChannel.find(my_channel) 
                    == partReadersNbPerChannel.end()) {
            partReadersNbPerChannel[my_channel] = 1;
        } else {
            ++partReadersNbPerChannel[my_channel];
        }
    }


    for(PartReaderIterator part_reader_iter(current_part_readers.begin()); 
            part_reader_iter != current_part_readers.end(); ++part_reader_iter) {

        PartReader* part_reader = *part_reader_iter;

        if (channel != NULL && part_reader->channel() != channel) { 
            continue; // For JACK channel output
        } 

        Channel* my_channel = part_reader->channel();
        QMutexLocker channel_locker(&my_channel->mutex());
        PadReaderList pad_readers = part_reader->padReaders();

        for(PadReaderIterator pad_readers_iter(pad_readers.begin()); 
                pad_readers_iter != pad_readers.end(); ++pad_readers_iter) {

            PadReader* pad_reader = *pad_readers_iter;

            if (my_channel->changing()) {
                pad_reader->setAudioData(my_channel->audioData());
            } 
            
            if ( pad_reader->bufptr == pad_reader->endptr ) {
                pad_reader->empty(true);
            }

            sample_count_t nb_samples = pad_reader->endptr - pad_reader->bufptr;		
            if (nb_samples_to_read < nb_samples) {
                nb_samples = nb_samples_to_read;
            }

            for (sample_count_t i = 0; i < nb_samples; ++i) {
                if ((i % 2) == 0) { 
                    buffer[i + buffer_offset] += 
                        *(pad_reader->bufptr++) * pad_reader->volumeLeft() 
                        * my_channel->volumeLeft() * mainVolumeLeft;
                } else {
                    buffer[i + buffer_offset] += 
                        *(pad_reader->bufptr++) * pad_reader->volumeRight() 
                        * my_channel->volumeRight() * mainVolumeRight;
                }
            }
        }

        if (--partReadersNbPerChannel[my_channel] == 0) {
            my_channel->setChanging(false);
        }
    }
}

void SongReader::read(sample_t* buffer, sample_count_t buffer_n_samples, Channel* channel) {

    memset(buffer, 0, buffer_n_samples * sizeof(sample_t));

    if (! is_playing) return;
    //qWarning("SongReader::read " + QString::number(buffer_n_samples));

    Config* cfg = Config::instance();
    bool to_update;
    sample_count_t* my_current_sample_number;
    if (channel == NULL) {
        my_current_sample_number = &current_sample_number;
    } else {
        my_current_sample_number = &channel->current_sample_number;
    }
    sample_count_t before_smpl_nb = buffer_n_samples;
    sample_count_t after_smpl_nb = 0;

    if (*my_current_sample_number + buffer_n_samples > cfg->samplesPerPad()) {
        to_update = true;
        if (*my_current_sample_number > cfg->samplesPerPad()) {
            // Probably a tempo change
            *my_current_sample_number = cfg->samplesPerPad();
        }
        before_smpl_nb = cfg->samplesPerPad() - *my_current_sample_number;
        after_smpl_nb = *my_current_sample_number + buffer_n_samples 
                        - cfg->samplesPerPad();
        Q_ASSERT(before_smpl_nb + after_smpl_nb == buffer_n_samples);
    } else {
        *my_current_sample_number += buffer_n_samples;
        to_update = false;
    }

    processPartReaders(buffer, before_smpl_nb, 0, channel);

    while (to_update) {
        update(channel);
        to_update = false;

        sample_count_t nb_samples_to_read = after_smpl_nb;
        if (after_smpl_nb > cfg->samplesPerPad()) {
            nb_samples_to_read = cfg->samplesPerPad();
            after_smpl_nb -= cfg->samplesPerPad();
            to_update = true;
        }

        *my_current_sample_number = after_smpl_nb;

        processPartReaders(buffer, nb_samples_to_read, before_smpl_nb, channel);

        before_smpl_nb += nb_samples_to_read;
    }
}

//EOF
