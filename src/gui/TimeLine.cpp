/*
 *   $Id: TimeLine.cpp,v 1.9 2007/01/26 07:39:55 rhizome Exp $
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

#include <QPainter>
#include <QPaintEvent>

#include "../common.h"
#include "../constants.h"

#include "TimeLine.h"

TimeLine::TimeLine(int nb_pads) 
    : current_pad(-1), 
      nb_pads(nb_pads),
      width(0),
      height(10)
{ 
    for (int i = 1; i < nb_pads + 1; ++i) {
        width += buttonGap;
        width += padButtonWidth;
        if ((i % 4) == 0) width += stepGap;
    }
    width -= buttonGap;
    width -= stepGap;
    width -= 1;
    setMinimumHeight(height + 1);
}

void TimeLine::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter.setBrush(Qt::white);
    painter.drawRect(1, 1, width - 1, height);
    painter.setBrush(Qt::gray);
    int left_side = 1;
    for (int i = 1; i < current_pad + 1; ++i) {
        left_side += buttonGap;
        left_side += padButtonWidth;
        if ((i % 4) == 0) left_side += stepGap;
    }
    painter.drawRect(left_side, 1, padButtonWidth - 1, height);
}

void TimeLine::init() {
    current_pad = -1;
    update();
}

void TimeLine::oneStepForward() {
    if (current_pad == (nb_pads - 1))
        current_pad = 0;
    else
        ++current_pad;
    update();
}

TimeLine::~TimeLine() {
}

//EOF
