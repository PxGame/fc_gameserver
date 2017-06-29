#ifndef LOG_H
#define LOG_H

# include <spdlog/spdlog.h>

using namespace std;
using namespace spdlog;

class Log
        : public enable_shared_from_this<Log>
{
private:

    static shared_ptr<Log> m_log;

public:

    static void Create();

    static inline shared_ptr<Log> GetInstance(){
        return m_log->shared_from_this();
    }

    static inline const shared_ptr<logger>& Main() {
        return m_log->m_main;
    }

public:

    Log();

    shared_ptr<logger> m_main;


private:

    void _Create();
};

#endif // LOG_H
