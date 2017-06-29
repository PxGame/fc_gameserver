#include "log.h"

shared_ptr<Log> Log::m_log = nullptr;

void Log::Create()
{
    m_log = make_shared<Log>();
    m_log->_Create();
}

Log::Log()
{

}

void Log::_Create()
{
    //校验路径
    if (!exists("./log"))
    {//创建路径
        create_directories("./log");
    }

    spdlog::set_pattern("[%D %T.%e][%l]%v");

    m_main = spdlog::rotating_logger_mt("main", "./log/log", 1024 * 1024 * 5, 1024);
    m_main->flush_on(level::err);
}
