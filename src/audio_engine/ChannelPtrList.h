/*
 *   $Id: ChannelPtrList.h,v 1.3 2006/12/27 06:35:10 rhizome Exp $
 * 
 *      Copyright (C) 2004, 2005 Alex Marandon
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

#ifndef CHANNEL_PTR_LIST
#define CHANNEL_PTR_LIST

#include <q3ptrlist.h>
#include "Channel.h"

template <class T>
class ChannelPtrList : public Q3PtrList<T> {
    int compareItems(Q3PtrCollection::Item, Q3PtrCollection::Item);
};

template <class T>
int ChannelPtrList<T>::compareItems(Q3PtrCollection::Item item1, 
        Q3PtrCollection::Item item2) {
    if (((Channel*)item1)->rowIndex() == ((Channel*)item2)->rowIndex())
        return 0;
    else if (((Channel*)item1)->rowIndex() > ((Channel*)item2)->rowIndex())
        return 1;
    return -1;
}

#endif

//EOF 
