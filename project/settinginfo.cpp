#include "settinginfo.h"

SettingInfo::SettingInfo()
{

}

bool SettingInfo::Load(string filePath)
{
    bool bRet= false;
    try
    {
     ifstream_t ifs;
     ifs.open(filePath);

     value jsVal = value::parse(ifs);

     m_uri = uri(jsVal["uri"].as_string());
     m_timeout = jsVal["timeout"].as_integer();
     m_sslKey = jsVal["sslkey"].as_string();
     m_sslCrt = jsVal["sslcrt"].as_string();

     bRet = true;
    }
    catch (const exception& err)
    {
        throw err;
    }

    return bRet;
}

uri SettingInfo::Uri()
{
    return m_uri;
}

int SettingInfo::TimeOut()
{
    return m_timeout;
}

string_t SettingInfo::SSLKey()
{
    return m_sslKey;
}

string_t SettingInfo::SSLCrt()
{
    return m_sslCrt;
}
