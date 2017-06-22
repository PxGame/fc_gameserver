#include "webservice.h"

# include "settinginfo.h"

shared_ptr<Webservice> Webservice::m_webservice = nullptr;

bool Webservice::Create()
{
    bool bRet = false;
    try
    {
        Destory();

        m_webservice = make_shared<Webservice>();

        SettingInfo settingInfo;
        if(!settingInfo.Load("setting.json"))
        {
            throw runtime_error("setting is load failed.");
        }

        http_listener_config listenerConfig;
        listenerConfig.set_timeout(utility::seconds(settingInfo.web.timeout));

        m_webservice->m_listener = make_shared<http_listener>(settingInfo.web.uri, listenerConfig);

        m_webservice->m_listener->support(
                    bind(&Webservice::DispatchRequest, m_webservice, placeholders::_1)
                    );

        m_webservice->m_htmlContentMap.clear();
        m_webservice->m_htmlContentMap[U("/adduser")] = std::bind(&Webservice::AddUser, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/addrank")] = std::bind(&Webservice::AddRank, m_webservice, std::placeholders::_1);

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

            m_webservice->m_listener.reset();
        }

        m_webservice.reset();
    }
}

shared_ptr<Webservice> Webservice::GetInstance()
{
    return m_webservice->shared_from_this();
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
    web::uri targetUri = message.absolute_uri();
    string_t targetQuery = targetUri.query();
    string_t targetFragment = targetUri.fragment();

    ucout << targetFragment << endl;

    std::map<string_t, string_t> queryMap = web::uri::split_query(targetQuery);

    message.reply(status_codes::OK, to_utf8string(targetFragment));
}

void Webservice::AddRank(http_request &message)
{
    web::uri targetUri = message.absolute_uri();
    string_t targetQuery = targetUri.query();
    string_t targetFragment = targetUri.fragment();

    ucout << targetFragment << endl;

    std::map<string_t, string_t> queryMap = web::uri::split_query(targetQuery);

    message.reply(status_codes::OK, to_utf8string(targetFragment));
}
