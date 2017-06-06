#ifndef SETTINGINFO_H
#define SETTINGINFO_H

# include "pub.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::json;
using namespace concurrency::streams;
using namespace std;

class SettingInfo
{
public:
    SettingInfo();
    bool Load(string filePath);

protected:
    uri m_uri;
    int m_timeout;
    string_t m_sslKey;
    string_t m_sslCrt;

public:
    uri Uri();
    int TimeOut();
    string_t SSLKey();
    string_t SSLCrt();
};

#endif // SETTINGINFO_H
