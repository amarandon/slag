/*
 *   $Id: Pattern.h,v 1.9 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef PATTERN_H
#define PATTERN_H

#include <qstring.h>
#include <qdom.h>
#include <qmap.h>
#include <qobject.h>

#include "SafelyDeletable.h"

class Channel;
class Part;

typedef QMap<Channel*, Part*> ChanPartMap;
typedef QMap<Channel*, int> ChanTabIndexMap;
typedef QMap<QString, Channel*> NameChanMap;
typedef QMap<QString, Part*> NamePartMap;

class Pattern : public QObject, public SafelyDeletable {
    Q_OBJECT
public:
    Pattern(const QDomElement &, NameChanMap &, NamePartMap &);
    Pattern(QString n) : _name(n) {}
    ~Pattern();
    QDomElement getDom(QDomDocument&);
    inline QString name() { return _name; }
    inline void setName(QString n) { _name = n; }
    void setTabIndex(Channel*, int);
    int tabIndex(Channel*);
    Part* part(Channel*);
    void removeChannel(Channel*);
public slots:
    void setPart(Channel*, Part*);
signals:
    void modified();
private:
    QString _name;
    ChanPartMap _channelPartMap;
    ChanTabIndexMap _channelTabIndexMap;
};

#endif

//EOF 
