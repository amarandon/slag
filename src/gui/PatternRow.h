/*
 *   $Id: PatternRow.h,v 1.7 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef PATTERN_ROW_H
#define PATTERN_ROW_H

#include <QWidget>
#include <QLabel>

class QLabel;
class QCheckBox;
class QPushButton;
class QBoxLayout;
class QTabWidget;
class QBoxLayout;

class Song;
class Channel;
class Pattern;
class Part;
class ChannelMenu;
class PartMenu;
class NamedButton;
class RowContext;

typedef QList<QWidget*>::Iterator FaderIterator;

class PatternRow : public QWidget {
    Q_OBJECT
private:
    std::auto_ptr<RowContext> row_context;
    Channel* channel;
    Pattern* pattern;
    QLabel* current_part_label;
    QList<QPushButton*> buttons;
    QList<QWidget*> volumeFaders;
    QList<QWidget*> panFaders;
    QBoxLayout* layout;
    QPushButton* new_part_button;
    PartMenu* part_menu; 
    QPushButton* part_but;
    Song* song;
    QTabWidget* tabWidget;
    QBoxLayout* vLayout;
public:
    PatternRow(QWidget* parent, Song*, RowContext*);
    ~PatternRow();
    void clearConnections();
    QString patternName();
public slots:
    void activate();
    void handlePartChanged(Part*);
    void handlePartRenamed(Part*);
    void handleFileChanged();
    void popup_part_menu();
    void deactivate(Part*);
    void showDetails(bool);
private slots:
    void tabChanged(int);
};

#endif

//EOF
