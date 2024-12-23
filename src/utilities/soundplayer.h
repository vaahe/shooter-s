#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include "src/utilities/globalsmanager.h"

#include <QUrl>
#include <QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <memory>

class SoundPlayer {
public:
    SoundPlayer()
    {
        m_audioOutput = new QAudioOutput();
        m_player = new QMediaPlayer();

        m_player->setAudioOutput(m_audioOutput);

        setVolume();
    }

    ~SoundPlayer() {
        delete m_player;
        delete m_audioOutput;
    }

    void playSound(const QString& soundPath) {
        m_player->setSource(QUrl(soundPath));
        m_player->play();

        if (m_player->error() != QMediaPlayer::NoError) {
            qWarning() << "Error playing sound:" << m_player->errorString();
        }
    }

private:
    void setVolume() {
        bool isMuted = m_globalsManager.getMuteState();
        m_audioOutput->setVolume(isMuted ? 0 : 0.5);
    }

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
};

#endif // SOUNDPLAYER_H
