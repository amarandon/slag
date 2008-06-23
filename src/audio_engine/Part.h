/*
 *   $Id: Part.h,v 1.14 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef PART_H
#define PART_H

#include <vector>

#include <qobject.h>
#include <qdom.h>
#include <qstring.h>

#include "SafelyDeletable.h"
#include "Channel.h"

class Pad;
class PadReader;

typedef std::vector<Pad*> PadList;
typedef std::vector<Pad*>::iterator PadIterator;

typedef std::vector<PadReader*> PadReaderList;
typedef std::vector<PadReader*>::iterator PadReaderIterator;

class Part : public QObject, public SafelyDeletable {
    Q_OBJECT
public:
    Part();
    Part(Part&);
    Part(const QDomElement &);
    ~Part();
    void addPad(Pad* new_pad);
    inline const QString& name() { return _name; }
    inline void setName(const QString& n) { _name = n; emit renamed(this); }
    QDomElement getDom(QDomDocument&);
    void getReaders(PadReaderList&, AudioData*);
    inline PadList & pads() { return _pads; }
    inline void scheduleDelete() {
        SafelyDeletable::scheduleDelete();
        emit deleted(this);
    }

public slots:
    void active1for2();
    void active1for4();
    void active1for8();
    void unactiveAll();
    void activeAll();
    void invert();
    void shiftLeft();
    void shiftRight();
    void randomize();
signals:
    void modified();
    void renamed(Part*);
    void deleted(Part*);
private:
    PadList _pads;
    PadIterator _updateIter;
    QString _name;
    void active1forN(int n);
};

class PartReader {
public:
    PartReader(Part*, Channel*);
    ~PartReader();
    void update();
    void oneStepMore();
    void rewind();
    bool empty();
    bool active() { return _channel->isActive(); }
    inline Channel*const channel() { return _channel; }
    inline void setOld(bool b) { _old = b; }
    inline const QString & name() { return _part->name(); }
    inline const PadReaderList & padReaders() { return _padReaders; }
private:
    PadReaderList _padReaders; /// une collection de PadReaders
    PadReaderIterator _updateIter;
    Part * _part; /// le part associé
    Channel* _channel;
    bool _old;
};

#endif

//EOF 

