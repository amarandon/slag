/*
 *  $Id: AudioDriver.h,v 1.16 2007/01/26 07:39:55 rhizome Exp $
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

/* 
 * Abstract class for audio drivers.
 *
 * It is a `pull' type interface, which means the audio driver asks for
 * digital audio data to play back when it needs it. The code using the driver
 * has to provide a callback function as a parameter to the AudioDriver's
 * constructor. The AudioDriver calls this function with a data buffer as a
 * parameter when it needs more data.
 */

#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <QMutex>
#include "../common.h"

class WavFileOutput;
class QString;
class Song;

typedef void (* ProcessCallback)(sample_t *);
typedef void (* JackErrorCallback)(const char *);

class AudioDriver {
    private:
        bool _recording;
    protected:
        int buffer_n_samples;
        QMutex recording_mutex;
    public:
        AudioDriver(ProcessCallback callback, int buffer_n_samples = 0);
        virtual ~AudioDriver() {}
        WavFileOutput* file_output;
        ProcessCallback audioProcessCallback;
        virtual void connectDriver() = 0;
        virtual void disconnect() = 0;
        virtual void updateChannels(Song*) {}
        void enableFileOutput(const QString & filename, int format);
        void disableFileOutput();
        inline bool recording() { return _recording; }
        inline QMutex& recordingMutex() { return recording_mutex; }
};

#endif

//EOF
