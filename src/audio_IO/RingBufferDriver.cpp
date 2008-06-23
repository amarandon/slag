/*
 *  $Id: RingBufferDriver.cpp,v 1.2 2007/01/26 07:39:55 rhizome Exp $
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

#include <QSemaphore>
#include "RingBufferDriver.h"

const int ringBufferScale = 3;

RingBufferDriver::RingBufferDriver(
        ProcessCallback processCallback, int buffer_n_samples) : 
    AudioDriver(processCallback, buffer_n_samples),
    audio_buffer(new sample_t[buffer_n_samples]),
    sem(new QSemaphore(ringBufferScale)),
    active(0),
    // TODO: sample_t should be a typedef of jack_default_audio_sample_t
    // when runnin using the JACK driver
    buffer_size(buffer_n_samples * sizeof(sample_t)),
    ringbuffer_size(buffer_size * ringBufferScale)
{ 
    memset(audio_buffer, 0, buffer_size);
    ringbuffer = jack_ringbuffer_create(ringbuffer_size);
    qDebug("JACK ringbuffer size: " + QString::number(ringbuffer_size));
    Q_ASSERT(ringbuffer != NULL);
}

RingBufferDriver::~RingBufferDriver() {
    delete[] audio_buffer;
    delete sem;
    jack_ringbuffer_free(ringbuffer);
}

void RingBufferDriver::run () { 
    while(active) {
        sem->acquire();
        audioProcessCallback(audio_buffer);
        Q_ASSERT(jack_ringbuffer_write_space(ringbuffer) >= buffer_size);
        jack_ringbuffer_write(ringbuffer, (char*)audio_buffer, buffer_size);
    }
}

void RingBufferDriver::read(sample_t* client_buffer) { 
        memset(client_buffer, 0, buffer_size);
        if (sem->available() < ringBufferScale 
                && jack_ringbuffer_read_space(ringbuffer) >= buffer_size) {
            jack_ringbuffer_read(ringbuffer, (char*)client_buffer, buffer_size);
            sem->release();
        }
}

void RingBufferDriver::connectDriver()    { 
    active = 1; 
    start(); 
}

void RingBufferDriver::disconnect() { 
    active = 0; 
    wait();
}

//EOF
