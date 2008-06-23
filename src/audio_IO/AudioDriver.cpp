/*
 *  $Id: AudioDriver.cpp,v 1.4 2007/01/26 07:39:55 rhizome Exp $
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

#include <QString>
#include "AudioDriver.h"
#include "WavFileOutput.h"

AudioDriver::AudioDriver(ProcessCallback callback, int buffer_n_samples) : 
    _recording(false),
    buffer_n_samples(buffer_n_samples), 
    file_output(NULL), 
    audioProcessCallback(callback)
{ }

void AudioDriver::enableFileOutput(const QString & filename, int format) {
    file_output = new WavFileOutput(filename, format);
    _recording = true;
}

void AudioDriver::disableFileOutput() { 
    QMutexLocker locker(&recording_mutex);
    _recording = false; 
    if (file_output != NULL) {
        delete file_output;
        file_output = NULL;
    }
}
