# include "pub.h"
# include "settinginfo.h"
# include "webservice.h"
# include "database.h"

using namespace std;

void Init();

int main(int argc, char *argv[])
{
    try
    {
        Init();
        cout << "Init Success." << endl;

        Webservice::GetInstance()->Start().wait();

        while(true){sleep(30);}
    }
    catch (const exception& e)
    {
        cout << "[main exception]" << e.what() << endl;
    }

    cout << "Application quit." << endl;
    return 0;
}

void Init()
{
    Log::Create();

    SettingInfo settingInfo;
    settingInfo.Load("setting.json");

    Webservice::Create(settingInfo.web);
    Database::Create(settingInfo.db);
}
