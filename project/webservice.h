#ifndef WEBSERVICE_H
#define WEBSERVICE_H

# include "pub.h"
# include "settinginfo.h"
# include "database.h"

using namespace web::http;
using namespace web::http::experimental::listener;
using namespace pplx;
using namespace utility;
using namespace utility::conversions;
using namespace std;

class Webservice
        : public enable_shared_from_this<Webservice>
{
protected:

    static shared_ptr<Webservice> m_webservice;

public:

    static void Create(WebSetting setting);
    static void Destory();
    static inline shared_ptr<Webservice> GetInstance(){return m_webservice;}

public:

    Webservice();
    ~Webservice();

    task<void> Start();
    task<void> Stop();

protected:
    boost::asio::const_buffer m_cert;
    boost::asio::const_buffer m_key;

    shared_ptr<http_listener> m_listener;

    std::map<string_t, std::function<void(http_request&)>> m_htmlContentMap;

    void SslContentCallback(boost::asio::ssl::context& context);
    void DispatchRequest(http_request message);

    void AddUser(http_request& message);
    void QueryUser(http_request& message);
    void DeleteUser(http_request& message);
    void AddRank(http_request& message);
    void AddRankEx(http_request& message);
    void QueryRank(http_request& message);


};

#endif // WEBSERVICE_H
