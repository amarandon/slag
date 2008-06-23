/*   
 *   $Id: Serialize.h,v 1.13 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <qdom.h>
#include <string>

#include "SongFileExceptions.h"

using std::string;

class SongFile {
    QDomElement root_elem;
    QString filename;
public:
    SongFile(const QString&) throw(audio_engine::SongOpenException);
    QDomElement getRootElem();
    void setPath(const QString&);
    void save(const QDomDocument&);
    QString getBaseName();
    QString getFileName();
    QString getDirPath();
    static QString defaultFileName;
    bool isDefaultSongFile();
};

#endif

//EOF 
