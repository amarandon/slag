/*
 *   $Id: Slag.cpp,v 1.66 2007/01/27 08:03:27 rhizome Exp $
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
#include <ctime>
#include <iostream>
#include <QSettings>
#include <QTextCodec>
#include <QTranslator>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>
#include <QEvent>
#include <Q3PtrList>
#include <q3progressdialog.h>

#include "common.h"
#include "config.h"
#include "constants.h"
#include "Slag.h"
#include "config/Config.h"
#include "audio_engine/Serialize.h"
#include "audio_engine/SongReader.h"
#include "audio_engine/Song.h"
#include "audio_IO/Exceptions.h"
#include "audio_IO/AudioDriverFactory.h"
#include "gui/MainWidget.h"

using std::cerr;
using std::endl;

void jackErrorCallBack(const char* msg) {
    QMessageBox::critical(NULL, "Slag - JACK error", QString(msg));
    qFatal(QString(msg));
}

void msgHandler(QtMsgType type, const char *msg) {
    const char* time_str = 
        QTime::currentTime().toString("hh:mm:ss.zzz").toLatin1().constData();
    switch (type) {
        case QtDebugMsg:
            if (Config::instance()->debug()) {
	       cerr << time_str << " => " << msg << endl;
            }
            break;
        case QtWarningMsg:
            cerr << time_str << " WARNING: " << msg << endl;
            break;
        case QtCriticalMsg:
            cerr << time_str << " CRITICAL ERROR: " << msg << endl;
            break;
        case QtFatalMsg:
            cerr << time_str << " FATAL ERROR: " << msg << endl;
            abort();     // deliberately core dump
    }
}

void Slag::restartAudioDriver() {
    initAudioDriver();
#ifdef WITH_JACK
    if (config->audioDriverName() == JACK_LABEL && config->jackTrackOutput()) {
        driver->updateChannels(song());
    }
#endif
    driver->connectDriver();
}

void Slag::showJackErrorPopup() {
    QMessageBox::critical(NULL, tr("Slag - JACK error"), 
            tr("Unable to connect JACK server. Is jackd running?"
               "\nDefaults to Null driver."));
}

void Slag::showLibaoErrorPopup() {
    QMessageBox::critical(NULL, tr("Slag - libao error"), 
            tr("Unable to use libao for audio output"
               "\nDefaults to Null driver."));
}

void Slag::showRtAudioErrorPopup() {
    QMessageBox::critical(NULL, tr("Slag - RtAudio error"), 
            tr("Unable to use RtAudio for audio output"
               "\nDefaults to Null driver."));
}

void Slag::initAudioDriver() {

    if (driver != NULL) {
        qDebug("Disconnecting previous audio driver.");
        driver->disconnect();
        delete driver;
    }


    qDebug(QString("Initializing audio driver %1.")
                   .arg(config->audioDriverName()));

    try {
        driver = audioDriverFactory->create();
    } catch (audio_IO::JackConnectionException & e) {
        qWarning(e.what());
        showJackErrorPopup();
        driver = audioDriverFactory->createNullDriver();
    } catch (audio_IO::LibaoConnectionException & e) {
        qWarning(e.what());
        showLibaoErrorPopup();
        driver = audioDriverFactory->createNullDriver();
    } catch (audio_IO::RtAudioConnectionException & e) {
        qWarning(e.what());
        showRtAudioErrorPopup();
        driver = audioDriverFactory->createNullDriver();
    }

    if (songReader != NULL) {
        songReader->setBufferSize(config->buffer_n_samples());
    }
}

Slag::Slag(int argc, char**argv)
    : QApplication(argc, argv), 
      driver(NULL), 
      main_widget(NULL), 
      songReader(NULL),
      song_file(NULL),
      audioDriverFactory(NULL)
{

    QCoreApplication::setOrganizationName("Alex Marandon");
    QCoreApplication::setApplicationName("Slag");

#if defined(Q_WS_MAC)
    SongFile::defaultFileName = 
        QString("%1/../Resources/examples/basic_kit.xml")
        .arg(applicationDirPath());
#endif

    qInstallMsgHandler(msgHandler);
    installTranslators();

    // Traitement des options de la ligne de commande
    try {
        config = Config::instance(argc, argv);
    } catch (BadOption & bad_opt) {
        cerr << endl << bad_opt.msg() << endl << endl;
        die_usage(argv);
    }

    srand(static_cast<unsigned>(time(0)));


    QSettings settings;

    QStringList recentFiles = settings.value("recentFiles").toStringList();
    if(settings.value("openLastSong").toBool() && recentFiles.count() > 0) {
        try {
            openSong(recentFiles[0]);
        } catch (audio_engine::SongOpenException e) {
            QMessageBox::critical(NULL, tr("Slag - Song open error"), 
                    tr("Unable to open last song file %1."
                        "\nStarting with a new song.").arg(recentFiles[0]));
            openSong(config->songfile());
        }
    } else {
        openSong(config->songfile());
    }
}

void Slag::installTranslators() {
#if defined(Q_WS_MAC)
    const QString slag_translation_file = 
        QString("%1/../Resources/l10n/%2").arg(
                applicationDirPath(), QTextCodec::locale());
#elif defined(Q_WS_WIN)
    const QString slag_translation_file = 
        QString("l10n/") + QTextCodec::locale();
#else
    const QString slag_translation_file = 
        QString("%1/share/%2/l10n/").arg(CONFIG_PREFIX, PACKAGE_TARNAME) 
        + QTextCodec::locale();
#endif
    QTranslator* slag_translator = new QTranslator;
    if(!slag_translator->load(slag_translation_file)) {
        qWarning(tr("Unable to load Slag translation file %1 for locale %2.")
                .arg(slag_translation_file, QTextCodec::locale()));
    }
    installTranslator(slag_translator);

    // FIXME il faudrait trouver quelque chose de plus portable pour trouver
    // les fichiers de traduction de Qt
    const QString qt_translation_file = QString(getenv("QTDIR")) + 
            "/translations/" + QString("qt_") + QTextCodec::locale();

        QTranslator* qt_translator = new QTranslator;
        if(!qt_translator->load(qt_translation_file)) {
            qWarning(tr("Unable to load Qt translation file %1 for locale %2.")
                    .arg(qt_translation_file, QTextCodec::locale()));
        }
        installTranslator(qt_translator);
    }

int Slag::start() {
    return exec();
}

void Slag::openSong(const QString & filename) {
    Q3ProgressDialog progress_dialog(
            tr("Loading song %1...").arg(filename),
            0, 0, main_widget, 0, TRUE);
    progress_dialog.setMinimumDuration(2000);
    SongFile* old_song_file = NULL;
    if (song_file != NULL) { 
        old_song_file = song_file;
    }
    song_file = new SongFile(filename);
    QDir::setCurrent(song_file->getDirPath());
    delete old_song_file;
    // Construction et initialisation de la song
    if (songReader != NULL) {
        driver->disconnect();
        delete songReader;
    }

    Song* new_song;
    if(config->no_gui()) {
        new_song = new Song(song_file->getRootElem());
    } else {
        new_song = new Song(song_file->getRootElem(), &progress_dialog);
    }
    processEvents();

    songReader = new SongReader(new_song, config->buffer_n_samples());
    if (audioDriverFactory == NULL) {
        audioDriverFactory = new AudioDriverFactory(songReader, 
                jackErrorCallBack);
    } else {
        audioDriverFactory->setSongReader(songReader);
    }
    initAudioDriver();
    connect(songReader, SIGNAL(stop()), this, SLOT(stop()));

    setSamplesPerPad(song()->tempo());

    if (! config->no_gui()) {
        initGui(&progress_dialog);
    }

    songReader->init();
    //song_updater->start(QThread::TimeCriticalPriority);
    connect(songReader, SIGNAL(patternAdded(Pattern*)), 
            main_widget, SLOT(addStackedWidget(Pattern*)));
    connect(songReader, SIGNAL(patternRemoved(Pattern*)), 
            main_widget, SLOT(removeStackedWidget(Pattern*)));
#ifdef WITH_JACK
    if (config->audioDriverName() == JACK_LABEL && config->jackTrackOutput()) {
        driver->updateChannels(song());
    }
#endif
    driver->connectDriver();

    main_widget->rewind();

    connect(new_song,  SIGNAL(channelAdded(Channel*)),
            main_widget, SLOT(addChannelWidgets(Channel*)));

    if (config->no_gui()) play();
}

void Slag::initGui(Q3ProgressDialog* progress_dialog) {
    if (main_widget == NULL) {
        main_widget = new MainWidget(this, progress_dialog);
    } else {
        main_widget->songChanged(progress_dialog);
    }

    connect(song(), SIGNAL(modified()), 
            main_widget,           SLOT(songModified()));
    main_widget->setModified(false);

    connect(songReader, SIGNAL(patternChanged(const QString &)),
            this,  SLOT(autoPatternChange(const QString &)));
    connect(songReader, SIGNAL(updated()), this,  SLOT(oneStepForward()));
    main_widget->show();
    setMainWidget(main_widget);
}


void Slag::autoPatternChange(const QString & name) {
    QApplication::postEvent(main_widget, new PatternChangeEvent(name));
}


void Slag::oneStepForward() {
    QApplication::postEvent(main_widget, new SongUpdateEvent);
}

bool Slag::patternMode() {
    return songReader->patternMode();
}

void Slag::updatePatternList(const StringItemList& list) {
    songReader->updatePatternList(list);
}

void Slag::play() {
    qDebug("Play");
    if (songReader->isPlaying()) {
        songReader->setPlaying(false);
        main_widget->rewind();
    }
    songReader->init();
    songReader->setPlaying(true);
}

void Slag::stop() {
    qDebug("Stop");
    songReader->setPlaying(false);
    main_widget->rewind();
    driver->disableFileOutput();
}

void Slag::setTempo(int tempo) {
    song()->setTempo(tempo);
    setSamplesPerPad(tempo);
}

void Slag::setSamplesPerPad(int tempo) {
    unsigned int new_val = static_cast<unsigned int>(
            ((static_cast<float>(60) / tempo) / 4)
            * Config::instance()->samplerate() * 2
            );
    if (new_val % 2 != 0) {
        ++new_val; // must be even
    }
    Config::instance()->samplesPerPad(new_val);
}

void Slag::setVolume(int v) {
    song()->setVolume(v);
}

void Slag::setSongMode() {
    songReader->setSongMode();
}

void Slag::setPatternMode() {
    songReader->setPatternMode();
}

void Slag::setLoopMode(int v) {
    songReader->setLoopMode(v == Qt::Checked);
}

void Slag::patternChange(const QString& pattern_name) {
    if (not songReader->isPlaying()) {
        main_widget->patternWidgetChange(pattern_name);
    }
    songReader->setNextWantedPattern(pattern_name);
}

void Slag::deleteChannel(Channel* channel) {
    songReader->deleteChannel(channel);
    main_widget->deleteChannel(channel);
}

void Slag::exportWav(const QString &filename, int format) {
    driver->enableFileOutput(filename, format);
    play();
}

void Slag::save() {
    song_file->save(song()->getDom());
}

Song* Slag::song() { 
    return songReader->song(); 
}

SongFile* Slag::songFile() {
    return song_file;
}

void Slag::die_usage(char**argv) {
    cerr << tr("Usage: %2 [OPTIONS] [file.xml]").arg(argv[0]).toLatin1().constData()
        << endl
        << tr("Options :").toLatin1().constData()
        << endl
        << " -h\t" << tr("Print this message.").toLatin1().constData()
        << endl
        << " -a\t" << tr("Use libao for audio output.").toLatin1().constData()
        << endl
#ifdef WITH_JACK
        << " -j\t" << tr("Use JACK with two output ports for audio output "
                "(this is the default).") .toLatin1().constData()
        << endl
        << " -t\t" 
        << tr("Use JACK with separate ports for each tracks for audio output.").toLatin1().constData()
        << endl
#endif
        << " -n\t" << tr("No audio output.").toLatin1().constData()
        << endl 
        << " -p\t" << tr("Player mode : Slag runs without graphical user interface and file supplied as argument is played.").toLatin1().constData()
        << endl
        << " -v\t" << tr("Enable verbose debugging output.").toLatin1().constData()
        << endl;
    std::exit(1);
}

int main(int argc, char **argv) {
    Slag slag(argc, argv);
    slag.start();
}

//EOF
