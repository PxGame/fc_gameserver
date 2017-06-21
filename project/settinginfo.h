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

public:
    uri Uri;
    int Timeout;
    string_t SSLKey;
    string_t SSLCrt;
};

#endif // SETTINGINFO_H
