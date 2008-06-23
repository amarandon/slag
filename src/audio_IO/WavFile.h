/*
 *   $Id: WavFile.h,v 1.2 2005/01/15 14:05:25 rhizome Exp 
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
 */

#ifndef WAVFILE_H
#define WAVFILE_H

#include "AudioFile.h"
#include "AudioFileExceptions.h"
#include <sndfile.h>

class WavFile : public AudioFile {
    SNDFILE* sf_file;
    SF_INFO sf_info;
    sample_t* original_buffer;
    sample_t* resampled_buffer;
    int frames;
public:
    WavFile(const char* name) throw (audio_IO::AudioFileException);
    virtual ~WavFile();
    virtual void get_data(AudioData*);
};

#endif

//EOF
