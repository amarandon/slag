/* 
 *   $Id: MainWidget.h,v 1.37 2007/01/26 07:39:55 rhizome Exp $
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

#ifndef WIDGETS_H
#define WIDGETS_H

#include <QMainWindow> 

#include "../config/Config.h"
#include "song_properties/song_properties_dialog.ui.h"
#include "preferences/preferences_dialog.ui.h"

class Q3ProgressDialog;
//class QStackedWidget;
class QAction;
class QLabel;
class QEvent;
class QCloseEvent;
class Q3FileDialog;
class QMenu;
class ToolBar;

class Slag;
class Part;
class Channel;
class Channel;
class Pattern;
class TimeLine;
class ChannelPanel;
class PatternRowStack;

class MainWidget : public QMainWindow {
    Q_OBJECT

public:
    MainWidget(Slag* slag, Q3ProgressDialog*);
    ~MainWidget();
    void songChanged(Q3ProgressDialog*);
    void setModified(bool val) { modified = val; }
    void patternWidgetChange(const QString&);

protected:
    void closeEvent(QCloseEvent*);
    void customEvent(QEvent*);

signals:
    void channelMovedUp(Channel*);
    void channelMovedDown(Channel*);
    void channelRename(Channel*);
    void partAdded(Part*);

public slots:
    void addChannelToSong();
    void addChannelWidgets(Channel*);
    void editPatternList();
    void deleteChannel(Channel*);
    void renameChannel(Channel*);
    void rewind();
    void exportSndFile();
    void addStackedWidget(Pattern*);
    void removeStackedWidget(Pattern*);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void openRecentFile(int param);
    void showSongProperties();
    void showPreferences();
    void songModified();
    void stopAndClose();
    void syncChannelPosition(Channel*);

private:
    QMap<QString, QWidget*> pattern_widget_map; // XXX maybe not needed
    QMap<Channel*, QWidget*> channel_row_map;
    QMap<Channel*, PatternRowStack*> pattern_row_map;
    QWidget* containerWidget;
    TimeLine* timeline;
    QWidget* scrollWidget;
    QGridLayout* scrollWidgetLayout;

    void createActions();
    QAction* createAnAction(
            const QString& label,
            const QString& shortCut,
            const QString& statusTip,
            const QString& connectionTarget,
            const QString& iconFilename = NULL
            );
    void createMenus();
    void createFileToolBar();
    void createStatusBar();
    void buildStack(Q3ProgressDialog*);
    void clearStack();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void updateRecentFileItems();
    void openSong(const QString&);
    bool overWriteFile(const QString&);

    SongPropertiesDialog* song_properties_dialog;
    PreferencesDialog* preferences_dialog;
    Q3FileDialog* slag_file_dialog;
    Q3FileDialog* wav_file_dialog;

    ToolBar* toolBar;
    Slag* slag;
    bool modified;
    QStringList recentFiles;
    enum { MaxRecentFiles = 5 };
    int recentFileIds[MaxRecentFiles];
    QLabel* patternLabel;

    QMenu *file_menu;
    QMenu *edit_menu;
    QMenu *options_menu;
    QMenu *help_menu;

    /// Usual actions
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *globalSettingsAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *exitAct;

    /// Specific actions
    QAction *songPropertiesAct;
    QAction *addChannelAct;
    QAction *editPatternListAct;
    QAction *exportSndFileAct;

    /// Status bar labels
    QLabel* modifLabel;

    bool openLastSong;
    Config* config;
};

#endif

//EOF
