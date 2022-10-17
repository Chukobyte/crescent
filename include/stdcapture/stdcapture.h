#ifndef STDCAPTURE_H
#define STDCAPTURE_H

#ifdef _WIN32
#include <io.h>
#define popen _popen
#define pclose _pclose
#define stat _stat
#define dup _dup
#define dup2 _dup2
#define fileno _fileno
#define close _close
#define pipe _pipe
#define read _read
#define eof _eof
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <functional>
#include <cstdio>
#include <chrono>
#include <thread>

#ifndef STD_OUT_FD
#define STD_OUT_FD (fileno(stdout))
#endif

#ifndef STD_ERR_FD
#define STD_ERR_FD (fileno(stderr))
#endif

namespace std {
    namespace capture {

        class CaptureOutput
        {
            FILE* stream;
            int fd;

            enum PIPES { READ, WRITE };

            int pipes[2];
            int streamOld;

            std::function<void(const char*, size_t)> callback;

        public :

            CaptureOutput(FILE* stream_, int fd_, std::function<void(const char*, size_t)> callback_) :
                    stream(stream_), fd(fd_), callback(callback_)
            {
                // Make output stream unbuffered, so that we don't need to flush
                // the streams before capture and after capture (fflush can cause a deadlock)
                setvbuf(stream, nullptr, _IONBF, 0);

                // Start capturing.
                secure_pipe(pipes);
                streamOld = secure_dup(fd);
                secure_dup2(pipes[WRITE], fd);
#ifndef _WIN32
                secure_close(pipes[WRITE]);
#endif
            }

            ~CaptureOutput()
            {
                // End capturing.
                secure_dup2(streamOld, fd);

                const int bufSize = 1025;
                char buf[bufSize];
                int bytesRead = 0;
                bool fd_blocked(false);
                do
                {
                    bytesRead = 0;
                    fd_blocked = false;
#ifdef _WIN32
                    if (!eof(pipes[READ]))
                        bytesRead = read(pipes[READ], buf, bufSize - 1);
#else
                    bytesRead = read(pipes[READ], buf, bufSize - 1);
#endif
                    if (bytesRead > 0)
                    {
                        buf[bytesRead] = 0;
                        callback(buf, bytesRead);
                    }
                    else if (bytesRead < 0)
                    {
                        fd_blocked = (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
                        if (fd_blocked)
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
                while(fd_blocked || bytesRead == (bufSize - 1));

                secure_close(streamOld);
                secure_close(pipes[READ]);
#ifdef _WIN32
                secure_close(pipes[WRITE]);
#endif
            }

        private :

            int secure_dup(int src)
            {
                int ret = -1;
                bool fd_blocked = false;
                do
                {
                    ret = dup(src);
                    fd_blocked = (errno == EINTR ||  errno == EBUSY);
                    if (fd_blocked)
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                while (ret < 0);
                return ret;
            }

            void secure_pipe(int * pipes)
            {
                int ret = -1;
                bool fd_blocked = false;
                do
                {
#ifdef _WIN32
                    ret = pipe(pipes, 65536, O_BINARY);
#else
                    ret = pipe(pipes) == -1;
#endif
                    fd_blocked = (errno == EINTR ||  errno == EBUSY);
                    if (fd_blocked)
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                while (ret < 0);
            }

            void secure_dup2(int src, int dest)
            {
                int ret = -1;
                bool fd_blocked = false;
                do
                {
                    ret = dup2(src, dest);
                    fd_blocked = (errno == EINTR ||  errno == EBUSY);
                    if (fd_blocked)
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                while (ret < 0);
            }

            void secure_close(int & fd)
            {
                int ret = -1;
                bool fd_blocked = false;
                do
                {
                    ret = close(fd);
                    fd_blocked = (errno == EINTR);
                    if (fd_blocked)
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                while (ret < 0);

                fd = -1;
            }

        };

        class CaptureStdout : public CaptureOutput
        {
        public :

            CaptureStdout(std::function<void(const char*, size_t)> callback) :
                    CaptureOutput(stdout, STD_OUT_FD, callback) { }
        };

        class CaptureStderr : public CaptureOutput
        {
        public :

            CaptureStderr(std::function<void(const char*, size_t)> callback) :
                    CaptureOutput(stderr, STD_ERR_FD, callback) { }
        };

    } // namespace capture
} // namespace std

#endif // STDCAPTURE_H

