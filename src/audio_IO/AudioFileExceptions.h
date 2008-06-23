/*
 *   $Id: AudioFileExceptions.h,v 1.2 2006/02/20 12:50:08 rhizome Exp $
 * 
 *      Copyright (C) 2004, 2005 Alex Marandon
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

#ifndef AUDIO_FILE_EXCEPTIONS_H
#define AUDIO_FILE_EXCEPTIONS_H

#include <qstring.h>

typedef unsigned int uint;

namespace audio_IO {

    struct AudioFileException {
        QString filename;
    };

    struct OpenException : public AudioFileException {
        QString sys_msg;
    };

    struct NbOfChannelsException : public AudioFileException {
        uint channels;
    };

    struct ReadException : public AudioFileException {
        uint expected_frames;
        uint read_frames;
    };

    struct SRCRatioException : public AudioFileException {
        double ratio;
    };

    struct SRCException : public AudioFileException {
        QString sys_msg;
    };

}

#endif

//EOF
