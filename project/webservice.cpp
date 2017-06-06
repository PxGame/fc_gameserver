#include "webservice.h"

# include "settinginfo.h"

Webservice* Webservice::m_webservice = nullptr;

bool Webservice::Create()
{
    bool bRet = false;
    try
    {
        Destory();

        m_webservice = new Webservice();

        SettingInfo settingInfo;
        if(!settingInfo.Load("setting.json"))
        {
            throw runtime_error("setting is load failed.");
        }

        http_listener_config listenerConfig;
        listenerConfig.set_timeout(utility::seconds(settingInfo.TimeOut()));

        m_webservice->m_listener = new http_listener(settingInfo.Uri(), listenerConfig);


        m_webservice->m_listener->support(
                    bind(&Webservice::DispatchRequest, m_webservice, placeholders::_1)
                    );

        m_webservice->m_htmlContentMap.clear();
        m_webservice->m_htmlContentMap[U("/test")] =
                bind(&Webservice::Test, m_webservice, placeholders::_1);

        bRet = true;
    }
    catch (const exception& err)
    {
        throw err;
    }

    return bRet;
}

void Webservice::Destory()
{
    if(nullptr != m_webservice)
    {
        if(nullptr!= m_webservice->m_listener)
        {
            try
            {
                m_webservice->m_listener->close().wait();
            }
            catch (const exception& err)
            {
                throw err;
            }

            delete(m_webservice->m_listener);
            m_webservice->m_listener = nullptr;
        }

        delete(m_webservice);
        m_webservice = nullptr;
    }
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

void Webservice::DispatchRequest(http_request message)
{
    string_t path = message.relative_uri().path();

    std::map<string_t, std::function<void(http_request&)>>::iterator contextData = m_htmlContentMap.find(path);
    if (m_htmlContentMap.end() == contextData)
    {//没有找到
        message.reply(status_codes::NotFound);
        return;
    }

    //分发调用
    contextData->second(message);
}

void Webservice::Test(http_request &message)
{
    message.reply(status_codes::OK, to_utf16string("噎屎拉！"));
}

task<void> Webservice::Start()
{
    task<void> retTask;

    try
    {
        retTask = m_listener->open();
    }
    catch (const exception& err)
    {
        throw err;
    }

    return retTask;
}

task<void> Webservice::Stop()
{
    task<void> retTask;

    try
    {
        retTask = m_listener->close();
    }
    catch (const exception& err)
    {
        throw err;
    }

    return retTask;
}
