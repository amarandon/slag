/*
 *   $Id: UntitledList.cpp,v 1.1 2006/03/28 01:06:34 rhizome Exp $
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

#include <qregexp.h>
#include "UntitledList.h"

const QString UntitledList::getNewName() {
    QString unamedId;
    int id = 0;

    while (true) {
        unamedId = _prefix + QString("%1").arg(id + 1, 0, 10);
        if (! contains(unamedId)) {
            append(unamedId);
            sort();
            break;
        } else {
            ++id;
        }
    }
    return unamedId;
}

void UntitledList::append(QString name) {
    QRegExp regexp( QString("^%1\\d+$").arg(_prefix) );
    if (regexp.search(name) != -1) {
        QStringList::append(name);
    }
}

//EOF 
