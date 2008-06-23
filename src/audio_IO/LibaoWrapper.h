/*
 *   $Id: LibaoWrapper.h,v 1.9 2007/01/26 07:39:55 rhizome Exp $
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

/// Wrapper C++ au dessus de libao <http://www.xiph.org/ao/>

#ifndef LIBAOWRAPPER_H
#define LIBAOWRAPPER_H

#include "../common.h"
#include <ao/ao.h>
#include <stdexcept>
#include <string>

class LibaoWrapper {
    private:
	ao_device* device; 
	ao_sample_format format;
        ao_option* options;
    public:
	LibaoWrapper(const int bits = 16, const int channels = 2, 
            const int rate = 44100, const int byte_format = AO_FMT_LITTLE);
	~LibaoWrapper();
	void play(sample_t* buffer, const int size);
};

#endif

//EOF
