#include "webservice.h"

shared_ptr<Webservice> Webservice::m_webservice = nullptr;

void Webservice::Create(WebSetting setting)
{
    try
    {
        Destory();

        m_webservice = make_shared<Webservice>();

        http_listener_config listenerConfig;
        listenerConfig.set_timeout(utility::seconds(setting.timeout));

        m_webservice->m_listener = make_shared<http_listener>(setting.uri, listenerConfig);

        m_webservice->m_listener->support(
                    bind(&Webservice::DispatchRequest, m_webservice, placeholders::_1)
                    );

        m_webservice->m_htmlContentMap.clear();
        m_webservice->m_htmlContentMap[U("/adduser")] = std::bind(&Webservice::AddUser, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/addrank")] = std::bind(&Webservice::AddRank, m_webservice, std::placeholders::_1);
    }
    catch (const exception& e)
    {
        throw e;
    }
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
            catch (const exception& e)
            {
                throw e;
            }

            m_webservice->m_listener.reset();
        }

        m_webservice.reset();
    }
}

Webservice::Webservice()
{

}

Webservice::~Webservice()
{
}

task<void> Webservice::Start()
{
    task<void> retTask;

    try
    {
        retTask = m_listener->open();
    }
    catch (const exception& e)
    {
        throw e;
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
    catch (const exception& e)
    {
        throw e;
    }

    return retTask;
}

void Webservice::DispatchRequest(http_request message)
{
    try
    {
        string_t path = message.relative_uri().path();

        std::map<string_t, std::function<void(http_request&)>>::iterator findIterator = m_htmlContentMap.find(path);
        if (m_htmlContentMap.end() == findIterator)
        {
            message.reply(status_codes::NotFound,to_utf8string("Not found."));
            return;
        }

        findIterator->second(message);
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
    }
}

void Webservice::AddUser(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        string_t gameid = jsonVal["gameid"].as_string();


    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
    }
}

void Webservice::AddRank(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        string_t gameid = jsonVal["gameid"].as_string();


    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
    }
}
