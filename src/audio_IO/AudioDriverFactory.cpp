/*
 *   $Id: AudioDriverFactory.cpp,v 1.6 2007/01/26 07:39:55 rhizome Exp $
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

#include <QMessageBox>

#include "../constants.h"
#include "../common.h"
#include "../config/Config.h"
#include "../audio_engine/SongReader.h"

// Inclusion des drivers audio
#include "NullDriver.h"
#include "WavFileOutput.h"

#ifdef WITH_JACK
  #include "JackDriver.h"
  #include "ChannelJackDriver.h"
#endif

#ifdef WITH_LIBAO
  #include "LibaoDriver.h"
#endif

#include "AudioDriverFactory.h"

const QString not_compiled_str("Not compiled with %1 support.");

SongReader* globalSongReader;
sample_count_t global_buffer_n_samples;

inline void process(sample_t * buffer) {
    globalSongReader->read(buffer, global_buffer_n_samples);
}

inline void processChannel(Channel* channel, sample_t * buffer) {
    globalSongReader->read(buffer, global_buffer_n_samples, channel);
}

AudioDriverFactory::AudioDriverFactory( SongReader* songReader,
        JackErrorCallback jackErrorCallBack) :
    jackErrorCallBack(jackErrorCallBack) 
{
    globalSongReader = songReader;
}

void AudioDriverFactory::setSongReader(SongReader* songReader) {
    globalSongReader = songReader;
}

AudioDriver* AudioDriverFactory::create() {
    AudioDriver* driver = NULL;
    config = Config::instance();
    if (config->audioDriverName() == LIBAO_LABEL)
        driver = createLibaoDriver();
    else if (config->audioDriverName() == NULL_LABEL)
        driver = createNullDriver();
    else if (config->jackTrackOutput())
        driver = createJackTrackDriver();
    else
        driver = createJackDriver();

    Q_ASSERT(driver != NULL);
    return driver;
}

AudioDriver* AudioDriverFactory::createJackDriver() {
#ifdef WITH_JACK
    AudioDriver* driver = new JackDriver(process, jackErrorCallBack);
    config->setAudioDriverName(JACK_LABEL);
    global_buffer_n_samples = config->buffer_n_samples();
    Q_ASSERT(driver != NULL);
    return driver;
#else
    qFatal(not_compiled_str.arg("JACK"));
    return NULL;
#endif
}

AudioDriver* AudioDriverFactory::createJackTrackDriver() {
#ifdef WITH_JACK
    AudioDriver* driver = 
        new ChannelJackDriver(processChannel, jackErrorCallBack);
    config->setAudioDriverName(JACK_LABEL);
    global_buffer_n_samples = config->buffer_n_samples();
    Q_ASSERT(driver != NULL);
    return driver;
#else
    qFatal(not_compiled_str.arg("JACK"));
    return NULL;
#endif
}

AudioDriver* AudioDriverFactory::createLibaoDriver() {
#ifdef WITH_LIBAO
    global_buffer_n_samples = config->buffer_n_samples();
    AudioDriver* driver = new LibaoDriver(process, global_buffer_n_samples);
    config->setAudioDriverName(LIBAO_LABEL);
    Q_ASSERT(driver != NULL);
    return driver;
#else
    qFatal(not_compiled_str.arg("libao"));
    return NULL;
#endif
}

AudioDriver* AudioDriverFactory::createNullDriver() {
    global_buffer_n_samples = config->buffer_n_samples();
    config->setAudioDriverName(NULL_LABEL);
    qDebug("Using Null driver.");
    return new NullDriver(process, global_buffer_n_samples); 
}

//EOF
