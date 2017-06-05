#include "webservice.h"

bool Webservice::Create()
{

    try
    {
        m_webservice = new Webservice();

        http_listener_config listenerConfig;
        listenerConfig.set_timeout(utility::seconds(12));


    }
    catch (const exception& err)
    {
    }
}

void Webservice::Destory()
{

}

Webservice *Webservice::GetInstance()
{
    return m_webservice;
}

Webservice::Webservice()
{

}

Webservice::~Webservice()
{

}

void Webservice::DispatchRequest(http_request &message)
{

}

task<void> Webservice::Start()
{

}

task<void> Webservice::Stop()
{

}
