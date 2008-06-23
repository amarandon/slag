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

#include <QPainter>
#include <QPixmapCache>
#include <QPixmap>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "Fader.h"

Fader::Fader(QWidget* parent) 
    : QWidget(parent), volume_position(0) { }

void Fader::paintEvent(QPaintEvent*) {
    QString key = cacheKey(volume_position);
    QPixmap pixmap;
    if (not QPixmapCache::find(key, pixmap)) {
        pixmap = createPixmap();
        QPixmapCache::insert(key, pixmap);
    }
    bitBlt(this, 0, 0, &pixmap);
}

QString Fader::cacheKey(int volume_position) {
    return QString("Fader:Volume:") + QString::number(volume_position);
}

QPixmap Fader::createPixmap() {
    QPixmap pixmap(width(), height());
    QPainter painter(&pixmap);

    painter.setBackgroundColor(Qt::white);
    painter.setBackgroundMode(Qt::OpaqueMode);

    // Drawing background rectangle
    brush.setStyle(Qt::Dense4Pattern);

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    brush.setColor(Qt::darkGray);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    pen.setColor(Qt::black);
    painter.setPen(pen);

    brush.setColor(Qt::gray);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawRect(0, volume_position, width() - 1, 
            height() - volume_position - 1);

    return pixmap;
}

void Fader::mouseMoveEvent(QMouseEvent* e) {
    if (e->state() == Qt::LeftButton) {
        emit volumeChanged(volumePosition2Value(e->pos().y()));
    } 
    update();
}

void Fader::setVolume(int v) {
    volume_position = height() - (v / (100 / height()));
    update();
}

void Fader::wheelEvent(QWheelEvent * e) {
    if (e->delta() < 0) {
        if (volume_position < height()) {
            ++volume_position;
        }
    } else {
        if (volume_position > 0) {
            --volume_position;
        }
    }
    emit volumeChanged(volumePosition2Value(volume_position));
    update();
}

int Fader::volumePosition2Value(int position) {
    int volume_value = 100 - (position * (100 / height()));
    if (volume_value > 100) {
        volume_value = 100;
    } else if (volume_value < 0) {
        volume_value = 0;
    }
    return volume_value;
}

//EOF
