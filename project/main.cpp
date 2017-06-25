# include "pub.h"
# include "settinginfo.h"
# include "webservice.h"

using namespace std;

void Init();

int main(int argc, char *argv[])
{
    try
    {
        Init();

        Webservice::GetInstance()->Start().wait();
    }
    catch (const exception& e)
    {
        cout << "Exception:" << e.what() << endl;
    }

    printf("Press Enter key to continue...\n");
    fgetc(stdin);

    return 0;
}

void Init()
{
    SettingInfo settingInfo;
    settingInfo.Load("setting.json");

    Webservice::Create(settingInfo.web);
}
