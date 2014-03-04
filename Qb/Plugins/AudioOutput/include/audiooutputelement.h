/* Webcamod, webcam capture plasmoid.
 * Copyright (C) 2011-2013  Gonzalo Exequiel Pedone
 *
 * Webcamod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamod is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamod. If not, see <http://www.gnu.org/licenses/>.
 *
 * Email     : hipersayan DOT x AT gmail DOT com
 * Web-Site 1: http://github.com/hipersayanX/Webcamoid
 * Web-Site 2: http://kde-apps.org/content/show.php/Webcamoid?content=144796
 */

#ifndef AUDIOOUTPUTELEMENT_H
#define AUDIOOUTPUTELEMENT_H

#include <QtMultimedia>
#include <qb.h>

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
}

typedef QSharedPointer<QTimer> TimerPtr;

class AudioOutputElement: public QbElement
{
    Q_OBJECT
    Q_PROPERTY(int bufferSize READ bufferSize NOTIFY bufferSizeChanged)

    Q_PROPERTY(QString outputThread READ outputThread
                                    WRITE setOutputThread
                                    RESET resetOutputThread)

    public:
        explicit AudioOutputElement();
        ~AudioOutputElement();
        Q_INVOKABLE int bufferSize() const;
        Q_INVOKABLE QString outputThread() const;

    private:
        QbCaps m_curCaps;
        QbCaps m_inCaps;
        QString m_outputTh;
        QbElementPtr m_audioConvert;
        QAudioDeviceInfo m_audioDeviceInfo;
        QAudioOutput *m_audioOutput;
        QIODevice *m_outputDevice;
        QByteArray m_audioBuffer;
        QQueue<QbPacket> m_packetQueue;

        TimerPtr m_pullTimer;
        TimerPtr m_soundInitTimer;
        TimerPtr m_soundUninitTimer;

        QMutex m_mutex;
        QMutex m_packetQueueMutex;

        QWaitCondition m_waitCondition;
        QbThreadPtr m_outputThread;

        TimerPtr runThread(QThread *thread, const char *method);

        QbCaps findBestOptions(const QbCaps &caps,
                               const QAudioDeviceInfo &deviceInfo,
                               QAudioFormat *bestOption=NULL) const;

    protected:
        void stateChange(QbElement::ElementState from, QbElement::ElementState to);

    signals:
        void elapsedTime(double pts);
        void bufferSizeChanged(int size);
        void requestFrame(int size);

    public slots:
        void setOutputThread(const QString &outputThread);
        void resetOutputThread();
        void iStream(const QbPacket &packet);

    private slots:
        bool init(QbCaps caps=QbCaps());
        void uninit(bool lock=true);
        void processFrame(const QbPacket &packet);
        void pullFrame();
        void feedAudio();
};

#endif // AUDIOOUTPUTELEMENT_H
