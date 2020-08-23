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

    long long GetNoPlayMs()
    {
        mux.lock();
        if (!out)
        {
            mux.unlock();
            return 0;
        }

        long long pts = 0;
        // 还未播放的字节数
        double size = out->bufferSize() - out->bytesFree();

        // 1秒音频字节大小
        double secSize = sampleRate * (sampleSize / 8) * channels;
        if (secSize <=0)
        {
            pts = 0;
        }
        else
        {
            pts = (size / secSize) * 1000;
        }


        mux.unlock();
        return pts;
    }

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
        io = out->start(); // 开始播放
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

	void Clear()
	{
		mux.lock();
		if (io)
		{
			io->reset();
		}
		mux.unlock();
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

	void SetPause(bool isPause)
	{
		mux.lock();
		if (out == nullptr)
		{
			mux.unlock();
			return;
		}

		if (isPause)
		{
			out->suspend();
		}
		else {
			out->resume();
		}
		mux.unlock();
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

