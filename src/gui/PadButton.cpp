/*
 *   $Id: PadButton.cpp,v 1.7 2007/01/26 07:39:55 rhizome Exp $
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
#include <QPixmapCache>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "PadButton.h"

PadButton::PadButton(QWidget* parent) 
    : QPushButton(parent) { }

void PadButton::paintEvent(QPaintEvent*) {
    QString key;
    if (isEnabled()) {
        if (isOn()) {
            key = QString("PadButton:enabled:On");
        } else {
            key = QString("PadButton:enabled:Off");
        }
    } else {
        key = QString("PadButton:disabled");
    }
    QPixmap pixmap;
    if (not QPixmapCache::find(key, pixmap)) {
        pixmap = createPixmap();
        QPixmapCache::insert(key, pixmap);
    }
    bitBlt(this, 0, 0, &pixmap);
}

QPixmap PadButton::createPixmap() {
    QPixmap pixmap(width(), height());
    QPainter painter(&pixmap);

    painter.setBackgroundColor(Qt::white);
    painter.setBackgroundMode(Qt::OpaqueMode);

    brush.setStyle(Qt::SolidPattern);

    if (isEnabled()) {
        pen.setColor(Qt::black);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(1);
        isOn() ? brush.setColor(Qt::gray) : brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawRect(0, 0, width() - 1, height() - 1);

    } else {
        pen.setStyle(Qt::NoPen);
        brush.setColor(Qt::lightGray);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawRect(0, 0, width(), height());
    }

    return pixmap;
}

//EOF
