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
        m_webservice->m_htmlContentMap[U("/queryuser")] = std::bind(&Webservice::QueryUser, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/addrank")] = std::bind(&Webservice::AddRank, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/queryrank")] = std::bind(&Webservice::QueryRank, m_webservice, std::placeholders::_1);
    }
    catch (const exception& e)
    {
        std::stringstream ostr;
        ostr << "[web]" << e.what();
        throw runtime_error(ostr.str());
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
                std::stringstream ostr;
                ostr << "[web]" << e.what();
                throw runtime_error(ostr.str());
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
        std::stringstream ostr;
        ostr << "[web]" << e.what();
        throw runtime_error(ostr.str());
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
        std::stringstream ostr;
        ostr << "[web]" << e.what();
        throw runtime_error(ostr.str());
    }

    return retTask;
}

void Webservice::DispatchRequest(http_request message)
{
    try
    {
        if(0 != message.method().compare(U("POST")))
        {
            message.reply(status_codes::MethodNotAllowed,to_utf8string("Method Not Allowed."));
            return;
        }

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

        ucout << U("AddUser:") << jsonVal.serialize().c_str() << endl;

        string_t gameid = jsonVal[U("gameid")].as_string();
        string_t deviceid = jsonVal[U("deviceid")].as_string();
        string_t username = jsonVal[U("username")].as_string();
        string_t devicetype = jsonVal[U("devicetype")].as_string();
        string_t devicemodel = jsonVal[U("devicemodel")].as_string();
        int auth = jsonVal[U("auth")].as_integer();

        Database::GetInstance()->AddUser(
                    gameid,
                    deviceid,
                    username,
                    devicetype,
                    devicemodel,
                    auth
                    );

        message.reply(status_codes::OK);
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
    }
}

void Webservice::QueryUser(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        ucout << U("QueryUser:") << jsonVal.serialize().c_str() << endl;

        string_t gameid = jsonVal[U("gameid")].as_string();
        string_t deviceid = jsonVal[U("deviceid")].as_string();

        shared_ptr<UserItem> item = Database::GetInstance()->QueryUser(
                    gameid,
                    deviceid
                    );

        json::value retVal;
        json::value rootVal;

        if(nullptr != item)
        {
            rootVal[U("username")] = json::value(to_string_t(item->username));
            rootVal[U("auth")] = json::value(item->auth);
        }

        retVal[U("info")] = rootVal;

        message.reply(status_codes::OK, retVal);
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

        ucout << U("AddRank:") << jsonVal.serialize().c_str() << endl;

        string_t gameid = jsonVal[U("gameid")].as_string();
        string_t deviceid = jsonVal[U("deviceid")].as_string();
        int level = jsonVal[U("level")].as_integer();
        int score = jsonVal[U("score")].as_integer();
        int cleartime = jsonVal[U("cleartime")].as_integer();
        string_t ip = "";

        Database::GetInstance()->AddRank(
                    gameid,
                    deviceid,
                    level,
                    score,
                    cleartime,
                    ip
                    );

        message.reply(status_codes::OK,message.to_string());
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
    }
}

void Webservice::QueryRank(http_request &message)
{

    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        ucout << U("QueryRank:") << jsonVal.serialize().c_str() << endl;

        string_t gameid = jsonVal[U("gameid")].as_string();
        int level = jsonVal[U("level")].as_integer();
        int cnt = jsonVal[U("cnt")].as_integer();

        shared_ptr<list<shared_ptr<RankItem>>> items = Database::GetInstance()->QueryRank(
                    gameid,
                    level,
                    cnt
                    );

        json::value retVal;
        json::value rootVal;

        int index = 0;
        for(shared_ptr<RankItem>& item : *items)
        {
            json::value jsItem;

            jsItem[U("username")] = json::value(to_string_t(item->username));
            jsItem[U("score")] = json::value(item->score);

            rootVal[index] = jsItem;
            index++;
        }

        retVal[U("ranks")] = rootVal;

        message.reply(status_codes::OK, retVal);
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
    }
}
