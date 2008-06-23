/*
 *   $Id: Pattern.cpp,v 1.16 2007/01/26 07:39:55 rhizome Exp $
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
#include "Pattern.h"
#include "Part.h"
#include "Channel.h"

Pattern::Pattern(const QDomElement & dom_elem, NameChanMap & chan_map, 
        NamePartMap & part_map) 
    : _name(dom_elem.attribute("id").latin1())
{
    QDomNodeList channel_nodes = dom_elem.childNodes();
    // Associations des patterns avec leurs parts
    for (unsigned int i = 0; i < channel_nodes.length(); ++i) {
        QDomElement chan_elem = channel_nodes.item(i).toElement();

        // Testing than channels and parts exist (already declared in the XML)
        Channel* channel = chan_map[chan_elem.attribute("ref")];

        if ( chan_elem.attribute("tab_index").isNull() ) {
            _channelTabIndexMap[channel] = 0;
        } else {
            bool ok = false;
            _channelTabIndexMap[channel] = chan_elem.attribute("tab_index").toInt(&ok);
            if (not ok) {
                qWarning(
                    QString("%1 is not a valid tab_index value, falling back to 0.").arg(
                        chan_elem.attribute("tab_index")));
                _channelTabIndexMap[channel] = 0;
            }

        }

        if (channel == NULL) {
            continue;
        } else if (part_map[chan_elem.attribute("part")] == NULL) {
            qDebug(tr("Part with id \"%1\" not found.").arg(chan_elem.attribute("part")));
            continue;

        } else {
            _channelPartMap[channel] = part_map[chan_elem.attribute("part")];
        }


    }
}

Pattern::~Pattern() {
    qDebug("Destruction Pattern " + _name);
}

QDomElement Pattern::getDom(QDomDocument& doc) {
    QDomElement dom_elem = doc.createElement("pattern");
    dom_elem.setAttribute("id", name());
    int x = 1;
    for (ChanPartMap::Iterator it = _channelPartMap.begin();
            it != _channelPartMap.end(); ++it) {
        QDomElement channel_elem = doc.createElement("channel");
        Q_ASSERT(it.key() != NULL);
        qWarning(QString("Item %1 out of %2").arg(x++).arg(_channelPartMap.count()));
        qWarning(QString("Trying to save channel %1").arg(it.key()->name()));
        channel_elem.setAttribute("ref", it.key()->name());
        if (it.data() != NULL)
            channel_elem.setAttribute("part", it.data()->name());
        channel_elem.setAttribute("tab_index", _channelTabIndexMap[it.key()]);
        dom_elem.appendChild(channel_elem);

    }
    return dom_elem;
}

void Pattern::setPart(Channel* channel, Part* part) {
    _channelPartMap.replace(channel, part);
    emit modified();
}

Part* Pattern::part(Channel* channel) {
    return _channelPartMap[channel];
}

void Pattern::setTabIndex(Channel* channel, int newIndex) {
    _channelTabIndexMap[channel] = newIndex;
}

int Pattern::tabIndex(Channel* channel) {
    return _channelTabIndexMap[channel];
}

void Pattern::removeChannel(Channel* channel) {
    qWarning(QString("Map contains %1.").arg(_channelPartMap.count()));
    _channelPartMap.remove(channel);
    qWarning(QString("Removed channel %1. %2 items left.").arg(channel->name()).arg(_channelPartMap.count()));
    _channelTabIndexMap.remove(channel);
}

//EOF
