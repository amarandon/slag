/*   
 *   $Id: MainWidget.cpp,v 1.67 2007/01/27 08:03:27 rhizome Exp $
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
#include <q3filedialog.h>
#include <q3progressdialog.h>

#include <QDir>
#include <QMap> 
#include <QStackedWidget> 
#include <QMenuBar> 
#include <QStatusBar> 
#include <QMessageBox>
#include <QLabel>
#include <QAction>
#include <QRegExp>
#include <QSettings>
#include <QScrollArea> 
#include <QHBoxLayout>
#include <QEvent>
#include <QPixmap>
#include <QMenu>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QCloseEvent>

#include "../constants.h"
#include "../config.h"
#include "../Slag.h"
#include "../audio_engine/Serialize.h"
#include "../audio_engine/Pattern.h"
#include "../audio_engine/Part.h"
#include "../audio_engine/Channel.h"
#include "../audio_engine/Song.h"
#include "../audio_engine/SongFileExceptions.h"
#include "../audio_IO/AudioFileExceptions.h"
#include "StringListEdit.h"
#include "MainWidget.h"
#include "PatternRowStack.h"
#include "ChannelRow.h"
#include "ChannelMenu.h"
#include "ToolBar.h"
#include "TimeLine.h"
#include "SndFileFormatDialog.h"


MainWidget::MainWidget(Slag* slag, Q3ProgressDialog* progress_dialog) : 
    containerWidget(new QWidget),
    timeline(new TimeLine(32)),
    scrollWidget(new QWidget),
    song_properties_dialog(new SongPropertiesDialog),
    preferences_dialog(new PreferencesDialog),
    slag_file_dialog(new Q3FileDialog(this, "slag file dialog", TRUE)),
    wav_file_dialog(new Q3FileDialog(this, "wav file dialog", TRUE)),
    toolBar(new ToolBar(slag)),
    slag(slag),
    patternLabel(new QLabel("")),
    file_menu(new QMenu(this)),
    edit_menu(new QMenu(this)),
    options_menu(new QMenu(this)),
    help_menu(new QMenu(this)),
    openLastSong(false),
    config(Config::instance())
{
    createActions();
    createMenus();
    createFileToolBar();
    addToolBar(toolBar);
    createStatusBar();
    readSettings();
    slag_file_dialog->setFilter( tr("Slag files (*.xml *.slag)") );

    scrollWidgetLayout = new QGridLayout;
    scrollWidget->setLayout(scrollWidgetLayout);
    scrollWidgetLayout->setMargin(10);
    scrollWidgetLayout->setSpacing(10);
    scrollWidgetLayout->setSizeConstraint(QLayout::SetFixedSize);

    QVBoxLayout* verticalLayout = new QVBoxLayout;
    verticalLayout->setMargin(0);

    QBoxLayout* timeLineLayout = new QHBoxLayout;
    timeLineLayout->addSpacing(20);
    patternLabel->setMaximumWidth(188);
    timeLineLayout->addWidget(patternLabel);
    timeLineLayout->addWidget(timeline);
    timeLineLayout->setResizeMode(QLayout::Fixed);

    verticalLayout->addSpacing(5);
    verticalLayout->addLayout(timeLineLayout);
    verticalLayout->addSpacing(5);
    QScrollArea* area = new QScrollArea;
    area->setWidget(scrollWidget);
    verticalLayout->addWidget(area);


    containerWidget->setLayout(verticalLayout);
    setCentralWidget(containerWidget);

    songChanged(progress_dialog);
}

void MainWidget::stopAndClose() {
    slag->stop();
    close();
}

void MainWidget::createActions() {

    newAct = createAnAction(tr("&New"), tr("Ctrl+N"), 
            tr("Create a new song file"), SLOT(newFile()), "new.png");

    openAct = createAnAction(tr("&Open..."), "Ctrl+O", 
            tr("Open an existing song file"), SLOT(open()), "open.png");

    saveAct = createAnAction(tr("&Save"), "Ctrl+S", 
            tr("Save the song to disk"), SLOT(save()), "save.png");

    saveAsAct = createAnAction(tr("Save &As"), "Ctrl+Shift+S", 
            tr("Save the song under a new name"), SLOT(saveAs()));

    exitAct = createAnAction(tr("&Quit"), "Ctrl+Q", 
            tr("Exit the application"), SLOT(stopAndClose()));

    globalSettingsAct = createAnAction(tr("Global settings"), "Ctrl-G", 
            tr("Global settings for the application"), SLOT(showPreferences()));

    aboutAct = createAnAction(tr("About Slag"), 0, tr("About this application"), 
            SLOT(about()));
    
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("About the Qt library"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    
    songPropertiesAct = createAnAction(tr("Song &properties"), "Ctrl-P", 
            tr("Properties of the current song"), SLOT(showSongProperties()));

    exportSndFileAct = createAnAction(tr("Export Sound File"), "Ctrl+Shift+F", 
            tr("Export song as a sound file"), SLOT(exportSndFile()));

    addChannelAct = createAnAction(tr("Add &track"), "Ctrl+Shift+C", 
            tr("Add a track to the current Song"), SLOT(addChannelToSong()));

    editPatternListAct = createAnAction(tr("Edit pattern list"), "Ctrl-Alt-P", 
            tr("Edit the list of patterns"), SLOT(editPatternList()));

}

QAction* MainWidget::createAnAction(
            const QString& label,
            const QString& shortCut,
            const QString& statusTip,
            const QString& connectionTarget,
            const QString& iconFilename
            ) 
{
    QAction* action = new QAction(tr(label), this);
    action->setShortcut(tr(shortCut));
    action->setStatusTip(tr(statusTip));
    if (! QString(iconFilename).isNull() ) {
        action->setIcon(QIcon(":/icons/" + iconFilename));
    }
    connect(action, SIGNAL(triggered()), this, connectionTarget);
    return action;
}
                

void MainWidget::createMenus() {

    menuBar()->insertItem( tr("&File"), file_menu );
    file_menu->addAction(newAct);
    file_menu->addAction(openAct);
    file_menu->addAction(saveAct);
    file_menu->addAction(saveAsAct);
    file_menu->insertSeparator();
    file_menu->addAction(exportSndFileAct);
    file_menu->insertSeparator();
    file_menu->addAction(exitAct);
    for (int i = 0; i < MaxRecentFiles; ++i)
        recentFileIds[i] = -1;

    menuBar()->insertItem( tr("&Edit"), edit_menu );
    addChannelAct->addTo(edit_menu);
    editPatternListAct->addTo(edit_menu);
    songPropertiesAct->addTo(edit_menu);

    menuBar()->insertItem( tr("&Options"), options_menu );
    globalSettingsAct->addTo(options_menu);

    menuBar()->insertItem( tr("&Help"), help_menu );
    aboutAct->addTo(help_menu);
    aboutQtAct->addTo(help_menu);
}

void MainWidget::createFileToolBar() {
    QToolBar* fileToolBar = addToolBar( tr("File") );
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
}

void MainWidget::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWidget::addChannelToSong() {
    slag->song()->addChannel();
}

void MainWidget::editPatternList() {
    QStringList list = slag->song()->patternNames();
    StringListEdit listEdit(list, slag->song()->unamedPatternList());
    listEdit.setAskBeforeRemoving(true);
    listEdit.setCaption(tr("Pattern List"));
    listEdit.setTexts(tr("Add Pattern"), tr("&Name:"), tr("Edit Pattern Name"));
    if (listEdit.exec()) {
        StringItemList itemList = listEdit.list();
        slag->updatePatternList(itemList);
        toolBar->sync();
        songModified();
        // Handle renamed patterns
        for (StringItemList::const_iterator it = itemList.begin(); 
                it != itemList.end(); ++it) {
            const StringItem* patternItem = &(*it);
            if (patternItem->isRenamed && ! patternItem->isNew) {
                pattern_widget_map[patternItem->text()] = 
                    pattern_widget_map[patternItem->originalText];
                pattern_widget_map.remove(patternItem->originalText);
            }
        }
    } else {
        for (QStringList::Iterator it = list.begin(); it != list.end();
                ++it ) {
            slag->song()->freePatternName(*it);
        }
    }
}

void MainWidget::buildStack(Q3ProgressDialog* progress_dialog) {
    int i = 0;
    foreach (Channel* channel, slag->song()->channels()) {
        addChannelWidgets(channel);
        progress_dialog->setProgress(++i);
        slag->processEvents();
    }
}

void MainWidget::addChannelWidgets(Channel* channel) {
    ChannelRow* channelRow = new ChannelRow(this, slag->song(), channel);
    int row_index = scrollWidgetLayout->count() / 2;
    scrollWidgetLayout->addWidget(channelRow, row_index, 0, Qt::AlignTop);
    channel_row_map[channel] = channelRow;

    connect(channelRow->channelMenu(), SIGNAL(channelRemoved(Channel*)), 
            slag,                      SLOT(deleteChannel(Channel*)));

    PatternRowStack* patternRowStack = new PatternRowStack(this, slag->song(), channel);
    connect(channelRow, SIGNAL(showDetails(bool)), patternRowStack, SIGNAL(showDetails(bool)));
    scrollWidgetLayout->addWidget(patternRowStack, row_index, 1, Qt::AlignTop);
    pattern_row_map[channel] = patternRowStack;
}

void MainWidget::clearStack() {
    /// Delete all layers of the PatternRowStack widgets
    for (QMap<Channel*, QWidget*>::iterator it = channel_row_map.begin(); 
            it != channel_row_map.end(); ++it) {
        delete it.data();
    }
    channel_row_map.clear();

    for (QMap<Channel*, PatternRowStack*>::iterator it = pattern_row_map.begin(); 
            it != pattern_row_map.end(); ++it) {
        delete it.data();
    }
    pattern_row_map.clear();
}

void MainWidget::addStackedWidget(Pattern* pattern) {
    foreach (Channel* channel, slag->song()->channels()) {
        pattern_row_map[channel]->addPatternRow(pattern);
    }
}

void MainWidget::removeStackedWidget(Pattern* pattern) {
    foreach (Channel* channel, slag->song()->channels()) {
        pattern_row_map[channel]->removePatternRow(pattern);
    }
}

void MainWidget::syncChannelPosition(Channel* channel) {
    QWidget* channelRow = channel_row_map[channel];
    QWidget* patternRow = pattern_row_map[channel];

    Q_ASSERT(patternRow != NULL);
    Q_ASSERT(channelRow != NULL);
    scrollWidgetLayout->removeWidget(channelRow);
    scrollWidgetLayout->removeWidget(patternRow);

    scrollWidgetLayout->addWidget(channelRow, channel->rowIndex(), 0);
    scrollWidgetLayout->addWidget(patternRow, channel->rowIndex(), 1);
}

void MainWidget::deleteChannel(Channel* channel) {
    uint removed_index = channel->rowIndex();
    QWidget* channelRow = channel_row_map[channel];
    QWidget* patternRow = pattern_row_map[channel];
    scrollWidgetLayout->removeWidget(channelRow);
    scrollWidgetLayout->removeWidget(patternRow);
    channel_row_map.remove(channel);
    pattern_row_map.remove(channel);
    delete channelRow;
    delete patternRow;
    foreach (Channel* c, channel_row_map.keys()) {
        if (c->rowIndex() >= removed_index) {
            syncChannelPosition(c);
        }
    }
}

void MainWidget::renameChannel(Channel* channel) {
    ChannelRow* row = (ChannelRow*) channel_row_map[channel];
    row->setName(channel->name());
}

MainWidget::~MainWidget() {
    qDebug("Destruction Mainwidget");
}

void MainWidget::customEvent(QEvent* event) {
    if (event->type() == PatternChangeEvent::Type) {
        PatternChangeEvent* patternChangeEvent = (PatternChangeEvent*) event;
        patternWidgetChange(patternChangeEvent->name());
    } else if (event->type() == SongUpdateEvent::Type) {
        timeline->oneStepForward();
    } else {
        QMainWindow::customEvent(event);
    }
}

void MainWidget::patternWidgetChange(const QString& name) {
//    Q_ASSERT(pattern_widget_map.contains(name));
    for (QMap<Channel*, PatternRowStack*>::iterator it = pattern_row_map.begin(); 
            it != pattern_row_map.end(); ++it) {
        static_cast<PatternRowStack*>(it.value())->setCurrentPattern(name);
    }
    patternLabel->setText(name);
    qDebug(QString("Current pattern changed to %1.").arg(name));
}

void MainWidget::newFile() {
    if (maybeSave()) {
        openSong(SongFile::defaultFileName);
    }
}

void MainWidget::open() {
    if (maybeSave()) {
        slag_file_dialog->setMode(Q3FileDialog::ExistingFile);
        slag_file_dialog->setCaption(tr("Open a file"));
        if ( slag_file_dialog->exec() == QDialog::Accepted ) {
            openSong(slag_file_dialog->selectedFile());
        }
    }
}

void MainWidget::openRecentFile(int param) {
    if (maybeSave()) {
        openSong(recentFiles[param]);
    }
}

void MainWidget::openSong(const QString& filename) {
    try {
        disconnect(slag->song(),  SIGNAL(channelAdded(Channel*)),
                this, SLOT(addChannelWidgets(Channel*)));
        slag->openSong(filename);
    } catch(audio_engine::SongOpenException& e) {
        connect(slag->song(),  SIGNAL(channelAdded(Channel*)),
                this, SLOT(addChannelWidgets(Channel*)));
        QMessageBox::critical(this, tr("Slag - Opening song"), e.what());
    }
}

bool MainWidget::save() {
    try {
        if (slag->songFile()->isDefaultSongFile()) {
            return saveAs();
        } else {
            slag->save();
            modifLabel->setText( tr("Saved") );
            modified = false;
            return true;
        }
    } catch (audio_engine::SongSaveException& e) {
        QMessageBox::critical(this, tr("Slag - Saving song"), e.what());
        return false;
    }
}

bool MainWidget::overWriteFile(const QString& filename) {
    if (QFile::exists(filename)) {
        int ret = QMessageBox::warning(this, tr("Slag"),
                tr("File %1 already exists.\n"
                    "Do you want to overwrite it?")
                .arg(QDir::convertSeparators(filename)),
                QMessageBox::Yes | QMessageBox::Default,
                QMessageBox::No | QMessageBox::Escape);
        if (ret == QMessageBox::No) {
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}

bool MainWidget::saveAs() {
    slag_file_dialog->setMode(Q3FileDialog::AnyFile);
    slag_file_dialog->setCaption(tr("Save as new file"));
    if ( slag_file_dialog->exec() == QDialog::Accepted ) {
        QString filename = slag_file_dialog->selectedFile().stripWhiteSpace();
        if (filename.isEmpty())
            return false;

        // The user shouldn't have to worry about whether he has given the
        // right suffix or not.
        if (! QRegExp("^.+\\.(slag|xml)$").exactMatch(filename) ) 
            filename.append(".slag");

        if (not overWriteFile(filename)) {
            return true;
        }

        const QString old_filename = slag->songFile()->getFileName();
        slag->songFile()->setPath(filename);
        try {
            slag->save();
        } catch (audio_engine::SongSaveException& e) {
            slag->songFile()->setPath(old_filename);
            QMessageBox::critical(this, tr("Slag - Saving song"), e.what());
            return false;
        }
        modified = false;
        slag_file_dialog->rereadDir();
        recentFiles.remove(filename);
        recentFiles.push_front(filename);
        updateRecentFileItems();
        return true;
    } else {
        return false;
    }
}

bool MainWidget::maybeSave() {
    if (modified) {
        int ret = QMessageBox::warning(this, tr("Slag"),
                     tr("The song has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWidget::exportSndFile() {
    const QString popup_title = tr("Slag - sound file export");
    if (config->audioDriverName() == JACK_LABEL && config->jackTrackOutput()) {
        QMessageBox::warning(
                this, popup_title,
            tr( "Sorry, sound file export is not implemented when JACK track "
                "output is enabled.\nPlease use the global settings "
                "dialog to disable it."));
        return;
    }
    SndFileFormatDialog formatDialog(this);
    if ( formatDialog.exec() == QDialog::Rejected ) {
        return;
    }
    qDebug(formatDialog.formatName());
    wav_file_dialog->setFilter( 
            tr("%1 (*.%2 *.%3)").arg(
                formatDialog.formatName(), 
                formatDialog.formatExtension(), 
                formatDialog.formatExtension().upper()
                )
            );
    wav_file_dialog->setMode(Q3FileDialog::AnyFile);
    wav_file_dialog->rereadDir();
    if ( wav_file_dialog->exec() == QDialog::Accepted ) {
        QString filename = wav_file_dialog->selectedFile();
        QRegExp extensionRegex(
                QString("^.+\\.%1$").arg(formatDialog.formatExtension())
                );
        extensionRegex.setCaseSensitive(false);
        if (not extensionRegex.exactMatch(filename)) {
            filename.append("." + formatDialog.formatExtension());
        }
        if (not overWriteFile(filename)) {
            return;
        }
        QMessageBox::information(
                this, popup_title,
                tr("Audio output will be recorded into %1 \n"
                    "until you stop playback.")
                   .arg(filename)
                );
        try {
            slag->exportWav(filename, formatDialog.formatId());
        } catch (audio_IO::OpenException e) {
            QMessageBox::critical(
                    this, popup_title,
                    tr("Unable to open %1 for writing.\n%2")
                       .arg(e.filename, e.sys_msg)
                    );
        }
    }
}

void MainWidget::showSongProperties() {
    song_properties_dialog->show();
}

void MainWidget::songModified() {
    modified = true;
    modifLabel->setText( tr("<b>%1</b>").arg( tr("Unsaved") ) );
}

void MainWidget::songChanged(Q3ProgressDialog* progress_dialog) {
    clearStack();
    buildStack(progress_dialog);
    toolBar->sync();
    QString filename = slag->songFile()->getFileName();

    if (filename != SongFile::defaultFileName) {
        setCaption("Slag - " + slag->songFile()->getBaseName());
        recentFiles.remove(filename);
        recentFiles.push_front(filename);
    } else {
        setCaption("Slag - New Song");
    }
    updateRecentFileItems();

    modified = false;
    modifLabel->setText( tr("Unchanged") );

}

void MainWidget::updateRecentFileItems() {
    while ((int)recentFiles.size() > MaxRecentFiles)
        recentFiles.pop_back();

    for (int i = 0; i < (int)recentFiles.size(); ++i) {
        QString text = tr("&%1 %2")
                       .arg(i + 1)
                       .arg(QFileInfo(recentFiles[i]).fileName());
        if (recentFileIds[i] == -1) {
            if (i == 0)
                file_menu->insertSeparator(file_menu->count() - 2);
            recentFileIds[i] =
                    file_menu->insertItem(text, this,
                                         SLOT(openRecentFile(int)),
                                         0, -1,
                                         file_menu->count() - 2);
            file_menu->setItemParameter(recentFileIds[i], i);
        } else {
            file_menu->changeItem(recentFileIds[i], text);
        }
    }
}

void MainWidget::rewind() {
    if ( ! Config::instance()->no_gui() ) {
        timeline->init();
    }
}

void MainWidget::createStatusBar() {
    modifLabel = new QLabel( tr("Unchanged"), this);

    //statusBar()->addWidget(new QLabel(tr(" PLOP ! "), this));
    //statusBar()->addWidget(new QLabel(tr(" Foo "), this), 1);
    statusBar()->addWidget(modifLabel);
}

void MainWidget::writeSettings() {
    qDebug("Saving application settings");
    QSettings settings;
    settings.setValue("geometry/x", x());
    settings.setValue("geometry/y", y());
    settings.setValue("geometry/width", width());
    settings.setValue("geometry/height", height());
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("openLastSong", openLastSong);
}

void MainWidget::readSettings() {
    qDebug("Reading application settings");
    QSettings settings;
    int x = settings.value("geometry/x", 50).toInt();
    int y = settings.value("geometry/y", 50).toInt();
    int w = settings.value("geometry/width", 930).toInt();
    int h = settings.value("geometry/height", 220).toInt();
    recentFiles = settings.value("recentFiles").toStringList();
    openLastSong = settings.value("openLastSong").toBool();

    move(x, y);
    resize(w, h);
}

void MainWidget::showPreferences() {
    
    // Setting preferences dialog fields acording to current config
    preferences_dialog->audioDriverSelected(config->audioDriverName());

    if(openLastSong)
        preferences_dialog->setOpenLastSong(true);
    else
        preferences_dialog->setOpenLastSong(false);

    if (! config->audioDriverName().isEmpty())
        preferences_dialog->setAudioDriver(config->audioDriverName());

    preferences_dialog->setAudioBufferSize(config->buffer_n_samples());
    preferences_dialog->setJackMulti(config->jackTrackOutput());
    preferences_dialog->setJackAutoConnect(config->jackAutoconnect());

    if(preferences_dialog->exec() == QDialog::Accepted) {
        openLastSong = preferences_dialog->getOpenLastSong();
        config->buffer_n_samples(preferences_dialog->getAudioBufferSize());
        config->setJackTrackOutput(preferences_dialog->getJackMulti());
        config->setAudioDriverName(preferences_dialog->getAudioDriver());
        config->setJackAutoconnect(preferences_dialog->getJackAutoConnect());

        writeSettings();
        config->save();

        slag->restartAudioDriver();
    }
}

void MainWidget::about() {
    QMessageBox::about(this, tr("About Slag"),
            tr("<h2>%1</h2>"
                "<p>A pattern-based audio sequencer.</p>"
                "<p>Copyright &copy; 2005-2007<br>Alex Marandon &lt;al@alpage.org&gt;<br>Licensed under the GNU GPL.</p>"
                "<p>Included drum kit is \"Drumatic3 sounds\" by opm, from http://freesound.iua.upf.edu/, licensed under the Creative Commons Sampling Plus 1.0 License.</p>"
                "<p>More informations at http://www.nongnu.org/slag</p>"
                ).arg(QString(PACKAGE_STRING)));
}

//EOF
