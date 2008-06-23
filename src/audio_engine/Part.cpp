/*
 *   $Id: Part.cpp,v 1.24 2007/01/26 07:39:55 rhizome Exp $
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

#include <cstdlib>

#include "Part.h"
#include "Pad.h"
#include "Channel.h"
#include "../audio_IO/AudioFile.h"
#include "../config/Config.h"
////////////////////////////////////////////////////////////////////////
/// PartReader
////////////////////////////////////////////////////////////////////////

Part::Part() 
    : _updateIter(_pads.begin())
{ 
}

Part::Part(Part & orig_part) 
  : QObject(), SafelyDeletable(), 
    _updateIter(_pads.begin()), 
    _name(orig_part.name())
{
    for(PadIterator it(orig_part.pads().begin()); 
            it != orig_part.pads().end(); ++it) {
        Pad* new_pad = new Pad(**it);
        connect(new_pad, SIGNAL(modified()), this, SIGNAL(modified()));
        addPad(new_pad);
    }
}

Part::Part(const QDomElement & dom_elem) 
    : _updateIter(_pads.begin()), 
      _name(dom_elem.attribute("id"))
{
    QDomNodeList pad_nodes = dom_elem.childNodes();

    for (uint j = 0; j < pad_nodes.length(); ++j) {
        Pad* new_pad = new Pad(pad_nodes.item(j).toElement());
        connect(new_pad, SIGNAL(modified()), this, SIGNAL(modified()));
        addPad(new_pad);
    }
}

Part::~Part() {
    qDebug("Destruction Part");
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it)
        delete (*(it++));
}

void Part::addPad(Pad* new_pad) {
    _pads.push_back(new_pad);
    emit modified();
}

QDomElement Part::getDom(QDomDocument& doc) {
    QDomElement dom_elem = doc.createElement("part");
    dom_elem.setAttribute("id", _name);
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it)
        dom_elem.appendChild((*it)->getDom(doc));
    return dom_elem;
}

void Part::getReaders(PadReaderList& readers, AudioData* audioData) {

    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it) {
        readers.push_back(new PadReader(*it, audioData));
    }

}

void Part::active1forN(int n) {
    int i = 0;
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it) {
        if (i % n == 0) 
            (*it)->setActive(true);
        else
            (*it)->setActive(false);
        ++i;
    }
    emit modified();
}

void Part::active1for2() {
    active1forN(2);
}

void Part::active1for4() {
    active1forN(4);
}

void Part::active1for8() {
    active1forN(8);
}

void Part::activeAll() {
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it)
        (*it)->setActive(true);
    emit modified();
}

void Part::unactiveAll() {
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it) {
        (*it)->setActive(false);
        (*it)->setVolume(100);
    }
    emit modified();
}

void Part::invert() {
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it)
        if ((*it)->isActive())
            (*it)->setActive(false);
        else
            (*it)->setActive(true);
    emit modified();
}

void Part::shiftLeft() {
}

void Part::shiftRight() {
    bool next_state = false;
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it) {
        bool current_state = (*it)->isActive();
        (*it)->setActive(next_state);
        next_state = current_state;
    }
    emit modified();
}

void Part::randomize() {
    for(PadIterator it(_pads.begin()); it != _pads.end(); ++it) {
        (*it)->setActive((rand() % 3) == 0);
    }
    emit modified();
}

////////////////////////////////////////////////////////////////////////
/// PartReader
////////////////////////////////////////////////////////////////////////

PartReader::PartReader(Part* part, Channel * chan) :  
    _updateIter(_padReaders.begin()), 
    _part(part), 
    _channel(chan), 
    _old(false)
{
    qDebug("Creation PartReader " + name());
    _part->addUser();
    _channel->addUser();
    // creation des _padReaders pour ce part_reader
    Q_ASSERT(_channel != NULL);
    Q_ASSERT(_channel->audioData() != NULL);
    _part->getReaders(_padReaders, _channel->audioData());
    _updateIter = _padReaders.begin();
}

PartReader::~PartReader() {
    qDebug("Destruction PartReader " + name());
    // destruction des _padReaders pour ce part_reader
    for(PadReaderIterator it(_padReaders.begin());
	    it != _padReaders.end(); ++it) {
	delete *it;
    }
    _part->rmUser();
    _channel->rmUser();
}


bool PartReader::empty() {
    qDebug("PartReader::empty()");
    for(PadReaderIterator pad_iter(_padReaders.begin()); 
	    pad_iter != _padReaders.end(); ++pad_iter) {
	// si un seul PadReader n'est pas vide, le part n'est pas vide
	if ( not (*pad_iter)->empty() ) return false;
    }
    return true;
}

void PartReader::rewind() {
    if ( _updateIter == _padReaders.end() ) _updateIter = _padReaders.begin();
}

void PartReader::oneStepMore() {
    ++_updateIter;
}

void PartReader::update() {
    //QMutexLocker locker(&_channel->mutex());
    if ( ! _old ) {

        if ( _updateIter == _padReaders.end() ) 
            _updateIter = _padReaders.begin();

        if (active() && *_updateIter != NULL) {
            (*_updateIter)->update();
        }

        ++_updateIter;
    }
}

//EOF
