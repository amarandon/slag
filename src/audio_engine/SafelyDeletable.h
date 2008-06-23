/*
 *   $Id: SafelyDeletable.h,v 1.3 2006/04/04 13:40:53 rhizome Exp $
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

#ifndef SAFELY_DELETABLE_H
#define SAFELY_DELETABLE_H

#include "SafeData.h"

class SafelyDeletable {
public:
    //explicit SafelyDeletable() { 
        //to_delete.setValue(false);
        //usage.setValue(0);
    //}
    //inline bool used()    { return (usage.getValue() > 0) ? true : false; }
    //inline uint usageCount() { return usage.getValue(); }
    //inline void addUser() { usage.setValue(usage.getValue() + 1); }
    //inline void rmUser()  { usage.setValue(usage.getValue() - 1); }
//
    //inline bool toDelete()  { return to_delete.getValue(); }
    //inline void scheduleDelete() { 
        //to_delete.setValue(true); 
        //// emit deleted(this); desactivé pour l'instant
    //}

    explicit SafelyDeletable() : to_delete(false), usage(0) {}
    inline bool used()    { return usage > 0; }
    inline uint usageCount() { return usage; }
    inline void addUser() { ++usage; }
    inline void rmUser()  { --usage; }

    inline bool toDelete()  { return to_delete; }
    inline void scheduleDelete() { to_delete = true; }

private:
    //SafeData<bool> to_delete; /// if the part has been destroyed by the user
    //SafeData<uint> usage;     /// how many PartReaders are using this part
    bool to_delete;
    uint usage;
};

#endif

//EOF 

