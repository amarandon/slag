/*
 *   $Id: WavFileOutput.h,v 1.6 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef WavFileOutput_h
#define WavFileOutput_h

#include "../common.h"
#include "AudioFileExceptions.h"

#include <sndfile.h>
#include <samplerate.h>

using audio_IO::OpenException;

void src_double_to_short_array (const double *in, short *out, int len);

class WavFileOutput {
    SNDFILE* sf_file;
    const QString& filename;
public:
    WavFileOutput(const QString & filename, int format) throw(OpenException);
    inline void write(sample_t* buffer, const int size) {
        short* short_buffer = new short[size];
        src_sample_to_short_array(buffer, short_buffer, size);
        sf_write_short(sf_file, short_buffer, size);
        delete[] short_buffer;
    }
    ~WavFileOutput();
};

#endif

//EOF 

