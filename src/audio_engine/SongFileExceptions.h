/*
 *   $Id: SongFileExceptions.h,v 1.1 2006/02/18 22:33:37 rhizome Exp $
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

#ifndef SONG_FILE_EXCEPTIONS_H
#define SONG_FILE_EXCEPTIONS_H

#include <stdexcept>
#include <string>
using std::string;

namespace audio_engine {

    struct SongOpenException : public std::runtime_error {
        SongOpenException(const string& msg = "") : std::runtime_error(msg) {}
    };

    struct SongSaveException : public std::runtime_error {
        SongSaveException(const string& msg = "") : std::runtime_error(msg) {}
    };

} //namespace

#endif

//EOF 

