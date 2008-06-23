/*
 *   $Id: cpp_impl.cpp,v 1.1 2007/02/15 08:14:16 al Exp $
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

#include <QWidget>

#include "../audio_engine/Song.h"
#include "../audio_engine/Channel.h"
#include "../audio_engine/Pattern.h"

#include "PatternRowStack.h"
#include "RowContext.h"
#include "PatternRow.h"

PatternRowStack::PatternRowStack(QWidget* parent, Song* song, Channel* channel) 
    : QStackedWidget(parent), song(song), channel(channel)
{
    foreach (Pattern* pattern, song->patterns()) {
        addPatternRow(pattern);
    } 
}

void PatternRowStack::setCurrentPattern(const QString& patternName) {
    for (int i = 0; i < count(); ++i) {
        if (((PatternRow*)widget(i))->patternName().compare(patternName) == 0) {
            setCurrentIndex(i);
        } 
    }
}

void PatternRowStack::addPatternRow(Pattern* pattern) {
    RowContext* context = new RowContext(channel, pattern);
    PatternRow* row = new PatternRow(this, song, context);
    connect(this, SIGNAL(showDetails(bool)), row, SLOT(showDetails(bool)));
    addWidget(row);
    pattern_row_map[pattern] = row;
    if ( pattern->part(channel) != NULL ) 
        row->activate();
}

void PatternRowStack::removePatternRow(Pattern* pattern) {
    PatternRow* row = pattern_row_map[pattern];
    disconnect(this, SIGNAL(showDetails(bool)), row, SLOT(showDetails(bool)));
    removeWidget(row);
    pattern_row_map.remove(pattern);
    delete row;
}

//EOF
