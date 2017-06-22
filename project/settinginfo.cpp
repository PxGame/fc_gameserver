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


     // web ====================================
     value jsWeb = jsVal["web"];
     web.uri = uri(jsWeb["uri"].as_string());
     web.timeout = jsWeb["timeout"].as_integer();

     value jsSsl = jsWeb["ssl"];
     web.ssl.key = jsSsl["key"].as_string();
     web.ssl.crt = jsSsl["crt"].as_string();


     // db =====================================
     value jsDb = jsVal["db"];
     db.host = jsDb["host"].as_string();
     db.username = jsDb["username"].as_string();
     db.password = jsDb["password"].as_string();
     db.connpoolsize = jsDb["connpoolsize"].as_integer();
     db.reconnmaxsize = jsDb["reconnmaxsize"].as_integer();

     bRet = true;
    }
    catch (const exception& err)
    {
        throw err;
    }

    return bRet;
}

