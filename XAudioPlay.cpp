#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>

class CXaudioPlay : public XAudioPlay
{
public:
    CXaudioPlay()
    {
    }
    ~CXaudioPlay()
    {
    }

    QAudioOutput* out = nullptr;
    QIODevice *io = nullptr;
    std::mutex mux;
    virtual bool Open()
    {
        Close();

        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setSampleSize(sampleSize);
        fmt.setChannelCount(channels);
        fmt.setCodec("audio/pcm");
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::UnSignedInt);

        mux.lock();
        out = new QAudioOutput(fmt);
        io = out->start(); // ¿ªÊ¼²¥·Å
        mux.unlock();

        if (io == nullptr)
        {
            return false;
        }
        return true;
    }

    virtual bool Close()
    {
        mux.lock();

        if (io != nullptr)
        {
            io->close();
            io = nullptr;
        }

        if (out != nullptr)
        {
            out->stop();
            delete out;
            out = nullptr;
        }
        mux.unlock();

        return true;
    }

    virtual bool WriteData(char* buf, int size)
    {
        if (buf == nullptr || size <= 0)
        {
            return false;
        }

        mux.lock();
        if (!out || !io)
        {
            mux.unlock();
            return false;
        }

        int datasize = io->write(buf, size);
        mux.unlock();
        if (datasize != size)
        {
            return false;
        }
        return true;
    }

    virtual int GetFree()
    {
        mux.lock();
        if (!out)
        {
            mux.unlock();
            return 0;
        }

        int free = out->bytesFree();
        mux.unlock();
        return free;

    }

private:

};


XAudioPlay * XAudioPlay::Get()
{
    static CXaudioPlay play;
    return &play;
}

XAudioPlay::XAudioPlay()
{
}

XAudioPlay::~XAudioPlay()
{
}

