/*
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

#ifndef PAN_FADER_H
#define PAN_FADER_H

#include <QBrush>
#include <QPen>
#include <QPixmap>

#include <QWheelEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWidget>

#include "Fader.h"

class PanFader : public Fader {
    Q_OBJECT
    public:
        PanFader(QWidget* parent);
    public slots:
        void setVolume(int);
    private:
        int volumePosition2Value(int);
        QPixmap createPixmap();
        QString cacheKey(int);
};


#endif

//EOF
