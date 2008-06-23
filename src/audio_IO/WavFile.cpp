/*
 *   $Id: WavFile.cpp,v 1.16 2007/01/26 07:39:55 rhizome Exp $
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

///
/// Implémentation de la classe d'accès aux fichiers WAV
///

#include <samplerate.h>
#include <qobject.h>
#include "WavFile.h"
#include "../config/Config.h"

using namespace audio_IO;

WavFile::WavFile (const char* name) throw (AudioFileException) 
    : AudioFile(name) 
{

    sf_info.format = 0;

    if(! (sf_file = sf_open(name, SFM_READ, & sf_info)) ) {
        OpenException exception;
        exception.filename = name;
        exception.sys_msg = sf_strerror(sf_file);
        throw(exception);
    }

    if ((sf_info.channels != 1) && (sf_info.channels != 2)) {
        NbOfChannelsException exception;
        throw(exception);
    }

    frames = sf_info.frames;

    // On réserve de la place pour un fichier stéréo
    original_buffer = new sample_t[frames * 2];

    int read_frames;
    if( (read_frames = sf_readf_sample(sf_file, original_buffer, frames)) 
            == frames) {
        qDebug(QString("File %1 loaded.").arg(name));
    } else {
        ReadException e;
        e.filename = name;
        e.read_frames = read_frames;
        e.expected_frames = frames;
        throw(e);
    }

    if (sf_close(sf_file) != 0)
        qWarning(QObject::tr("Unable to close WAV file %1.").arg(name));

    Config * config = Config::instance();

    if (sf_info.channels == 1) {
        qDebug(QString("Conversion mono -> stereo for %1").arg(name));
        sample_t* stereo_buffer = new sample_t[frames * 2];
        int i = 0;
        int j = 0;
        while(j < frames) {
            // chaque échantillon est dupliqué
            stereo_buffer[i++] = original_buffer[j];
            stereo_buffer[i++] = original_buffer[j++];
        }
        delete[] original_buffer;
        original_buffer = stereo_buffer;
    }

    /// Conversion frequence d'echantillonage
    if (config->samplerate() != sf_info.samplerate) {
        // Configuration du convertisseur de taux d'echantillonage
        SRC_DATA src_config;

        src_config.src_ratio = (double)config->samplerate() 
            / (double)sf_info.samplerate;
        //qDebug("sf_info.channels:"+QString::number(sf_info.channels));
        //qDebug("orig samplerate:"+QString::number(sf_info.samplerate));
        //qDebug("dst samplerate:"+QString::number(config->samplerate()));
        //qDebug("src_ratio:"+QString::number(src_config.src_ratio));

        if(!src_is_valid_ratio (src_config.src_ratio)) {
            qWarning(QObject::tr("Ratio de conversion invalide"));
            SRCRatioException e;
            throw(e);
        }

        int resampled_array_len = (int)(2 * frames * src_config.src_ratio);
        sample_t* resampled_buffer = new sample_t[resampled_array_len];
        //qDebug("samples:"+QString::number(2 * frames));
        //qDebug("resampled_array_len:"+QString::number(resampled_array_len));

        src_config.input_frames = sf_info.frames;
        frames = (int) (sf_info.frames * src_config.src_ratio);
        src_config.output_frames = (long) frames;
        src_config.data_in = (float*) original_buffer;
        src_config.data_out = (float*) resampled_buffer;
        //qDebug("src_config.output_frames:"+
                //QString::number(src_config.output_frames));

        // conversion du taux d'échantillonage
        int src_retval;
        if ((src_retval = src_simple(&src_config, SRC_SINC_BEST_QUALITY, 2))
                == 0) {
            qDebug(QString("Conversion %1 Hz -> %2 Hz for %3").arg(
                        QString::number(sf_info.samplerate), 
                        QString::number(config->samplerate()), 
                        name));
        } else {
            SRCException e;
            throw(e);
        }

        delete[] original_buffer;
        original_buffer = resampled_buffer;
    }
}

WavFile::~WavFile() {
    qDebug("Destruction WavFile destruction");
    delete[] original_buffer;
}

void WavFile::get_data(AudioData* audiodata) {
    audiodata->buffer = original_buffer;
    audiodata->nb_frames = frames;
}

//EOF
