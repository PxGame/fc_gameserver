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

        //ssl support

        Log::Main()->info("Using SSL[{0}].", setting.ssl.use);
        if(setting.ssl.use)
        {
            m_webservice->m_cert = setting.ssl.CrtBuffer();
            m_webservice->m_key = setting.ssl.KeyBuffer();
            listenerConfig.set_ssl_context_callback(std::bind(&Webservice::SslContentCallback, m_webservice, std::placeholders::_1));
        }

        m_webservice->m_listener = make_shared<http_listener>(setting.uri, listenerConfig);

        m_webservice->m_listener->support(
                    methods::POST,
                    bind(&Webservice::DispatchRequest, m_webservice, placeholders::_1)
                    );

        m_webservice->m_htmlContentMap.clear();
        m_webservice->m_htmlContentMap[U("/adduser")] = std::bind(&Webservice::AddUser, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/queryuser")] = std::bind(&Webservice::QueryUser, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/addrank")] = std::bind(&Webservice::AddRank, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/addrankex")] = std::bind(&Webservice::AddRankEx, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/queryrank")] = std::bind(&Webservice::QueryRank, m_webservice, std::placeholders::_1);
        m_webservice->m_htmlContentMap[U("/deleteuser")] = std::bind(&Webservice::DeleteUser, m_webservice, std::placeholders::_1);
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

void Webservice::SslContentCallback(boost::asio::ssl::context &context)
{
    context.set_options(boost::asio::ssl::context::default_workarounds);
    context.use_certificate_chain(m_cert);
    context.use_private_key(m_key, boost::asio::ssl::context::pem);
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
        Log::Main()->error(e.what());
    }
}

void Webservice::AddUser(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        Log::Main()->info("AddUser:{0}.", jsonVal.serialize());

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
        Log::Main()->error(e.what());
    }
}

void Webservice::QueryUser(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        Log::Main()->info("QueryUser:{0}.", jsonVal.serialize());

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
        Log::Main()->error(e.what());
    }
}

void Webservice::DeleteUser(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        Log::Main()->info("DeleteUser:{0}.", jsonVal.serialize());

        string_t gameid = jsonVal[U("gameid")].as_string();
        string_t deviceid = jsonVal[U("deviceid")].as_string();

        Database::GetInstance()->DeleteUser(
                    gameid,
                    deviceid
                    );

        message.reply(status_codes::OK);
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
        Log::Main()->error(e.what());
    }
}

void Webservice::AddRank(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        Log::Main()->info("AddRank:{0}.", jsonVal.serialize());

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

        message.reply(status_codes::OK);
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
        Log::Main()->error(e.what());
    }
}

void Webservice::AddRankEx(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        Log::Main()->info("AddRankEx:{0}.", jsonVal.serialize());

        string_t gameid = jsonVal[U("gameid")].as_string();
        string_t deviceid = jsonVal[U("deviceid")].as_string();
        int level = jsonVal[U("level")].as_integer();
        int score = jsonVal[U("score")].as_integer();
        int cleartime = jsonVal[U("cleartime")].as_integer();
        string_t ip = "";

        shared_ptr<RankNumber> rankNum = Database::GetInstance()->AddRankEx(
                    gameid,
                    deviceid,
                    level,
                    score,
                    cleartime,
                    ip
                    );

        json::value retVal;
        json::value rootVal;

        if (nullptr != rankNum)
        {
            rootVal["current"] = rankNum->current;
            rootVal["best"] = rankNum->best;
        }

        retVal["rank"] = rootVal;

        message.reply(status_codes::OK, retVal);
    }
    catch(const exception& e)
    {
        message.reply(status_codes::ExpectationFailed, to_utf8string(e.what()));
        Log::Main()->error(e.what());
    }
}

void Webservice::QueryRank(http_request &message)
{
    try
    {
        task<json::value> tk = message.extract_json();
        tk.wait();

        json::value jsonVal = tk.get();

        Log::Main()->info("QueryRank:{0}.", jsonVal.serialize());

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
        Log::Main()->error(e.what());
    }
}
