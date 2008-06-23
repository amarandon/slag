/*
 *   $Id: WavFileOutput.cpp,v 1.7 2007/01/26 07:39:55 rhizome Exp $
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

#include <qobject.h>
#include "WavFileOutput.h"
#include "../config/Config.h"

WavFileOutput::WavFileOutput(const QString & filename, int format) 
    throw(OpenException) 
    : filename(filename) 
{
    SF_INFO sf_info;
    sf_info.format = format;
    sf_info.samplerate =  Config::instance()->samplerate();
    sf_info.channels = 2;

    if (! sf_format_check(&sf_info)) {
        OpenException exception;
        exception.filename = filename;
        exception.sys_msg = "Format check failed.";
        qWarning(QObject::tr("Unable to open %1 for writing.\n%2").arg(
                 filename, QObject::tr("Format check failed.")));
        throw(exception);
    }

    if(! (sf_file = sf_open(filename, SFM_WRITE, &sf_info)) ) {
        OpenException exception;
        exception.filename = filename;
        exception.sys_msg = sf_strerror(sf_file);
        qWarning(QObject::tr("Unable to open %1 for writing.\n%2").arg(
                 filename, sf_strerror(sf_file)));
        throw(exception);
    }
}

WavFileOutput::~WavFileOutput() {
    if (sf_close(sf_file) != 0)
        qWarning(QObject::tr("Unable to close file %1. %2")
                 .arg(filename, sf_strerror(sf_file)));
}

//EOF
