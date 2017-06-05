#ifndef WEBSERVICE_H
#define WEBSERVICE_H

# include "pub.h"

using namespace web::http;
using namespace web::http::experimental::listener;
using namespace pplx;
using namespace utility;
using namespace utility::conversions;
using namespace std;

class Webservice
{
protected:

    static Webservice* m_webservice;

public:

    static bool Create();
    static void Destory();
    static Webservice* GetInstance();

protected:
    Webservice();
    ~Webservice();

    http_listener* m_listener;

    map<string_t, function<void(http_request&)>> m_htmlContentMap;

    void DispatchRequest(http_request& message);

public:
    task<void> Start();
    task<void> Stop();

};

#endif // WEBSERVICE_H
