/*
 *   $Id: Serialize.cpp,v 1.19 2007/01/26 07:39:55 rhizome Exp $
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
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <q3textstream.h>

#include "../config.h"

#include "Serialize.h"

#ifdef Q_WS_WIN
QString SongFile::defaultFileName = QString("examples/basic_kit.xml");
#else
QString SongFile::defaultFileName = 
    QString("%1/share/%2/examples/basic_kit.xml").arg(
        CONFIG_PREFIX, PACKAGE_TARNAME);
#endif

SongFile::SongFile(const QString&  name) throw(audio_engine::SongOpenException)
    : filename(name) {

    QDomDocument doc("slag file");
    QFile file(filename);

    if ( !file.open( QIODevice::ReadOnly ) ) {
        throw(audio_engine::SongOpenException(
                    QObject::tr("Unable to open file %1. %2")
                              .arg(file.name(), file.errorString()).latin1()));
    }
    QString error_msg;
    int error_line;
    int error_col;
    if ( !doc.setContent(&file, false, &error_msg, &error_line, &error_col) ) {
	file.close();
        throw(audio_engine::SongOpenException(
            QObject::tr("XML parsing of %1 failed.\n%2 at line %3, column %4.")
             .arg(file.name()).arg(error_msg).arg(error_line).arg(error_col).latin1()));
    }
    file.close();
    root_elem = doc.documentElement();
}

QDomElement SongFile::getRootElem() {
    return root_elem;
}

void SongFile::setPath(const QString& name) {
    filename = name;
}

void SongFile::save(const QDomDocument & dom_doc) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        Q3TextStream stream(&file);
        stream << dom_doc.toString(2) << endl;
        file.close();
    } else {
        throw(audio_engine::SongSaveException(
                    QObject::tr("Unable to save file %1. %2.")
                    .arg(file.name(), file.errorString()).latin1()));
    }
}

QString SongFile::getBaseName() {
     QFileInfo fi(filename);
     return fi.baseName();
}

QString SongFile::getDirPath() {
     QFileInfo fi(filename);
     return fi.dirPath();
}

QString SongFile::getFileName() {
    return filename;
}

bool SongFile::isDefaultSongFile() {
    if (filename == SongFile::defaultFileName)
        return true;
    else
        return false;
}

//EOF
