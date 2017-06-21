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

     Uri = uri(jsVal["uri"].as_string());
     Timeout = jsVal["timeout"].as_integer();
     SSLKey = jsVal["sslkey"].as_string();
     SSLCrt = jsVal["sslcrt"].as_string();

     bRet = true;
    }
    catch (const exception& err)
    {
        throw err;
    }

    return bRet;
}

