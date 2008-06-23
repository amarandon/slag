/*
 *   $Id: UntitledList.h,v 1.2 2007/01/20 20:14:23 rhizome Exp $
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

#ifndef UNTITLED_LIST_H
#define UNTITLED_LIST_H

#include <QStringList>

class UntitledList : public QStringList {
    QString _prefix;
public:
    UntitledList(QString p) : _prefix(p) {}
    const QString getNewName();
    inline void setPrefix(QString s) { _prefix = s; }
    inline QString prefix() { return _prefix; }
    void append(QString);
};

#endif

//EOF 

