/*
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

#include "PanFader.h"

PanFader::PanFader(QWidget* parent) 
    : Fader(parent) { }

QString PanFader::cacheKey(int volume_position) {
    return QString("Fader:Balance:") + QString::number(volume_position);
}

void PanFader::setVolume(int v) {
    volume_position = height() - ((v + 50) / (100 / height()));
    update();
}

int PanFader::volumePosition2Value(int position) {
    int volume_value = 100 - (position * (100 / height()));
    if (volume_value > 100) {
        volume_value = 100;
    } else if (volume_value < 0) {
        volume_value = 0;
    }
    return volume_value - 50;
}

QPixmap PanFader::createPixmap() {
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

    int middle = height() / 2;

    if (volume_position < middle) {
        painter.drawRect(0, volume_position, width() - 1, middle - volume_position);
    } else if (volume_position > middle) {
        int h = volume_position - middle - 1;
        if (h == 0) h = 1;
        painter.drawRect(0, middle, width() - 1, h);
    } else {
        painter.drawLine(0, middle, width(), middle);
    }

    return pixmap;
}

//EOF
