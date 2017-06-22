# include "pub.h"
# include "webservice.h"

using namespace std;

int main(int argc, char *argv[])
{
    try {

        if(Webservice::Create())
        {
            Webservice::GetInstance()->Start().wait();
        }
        else
        {
            cout << "create error." << endl;
        }
    }
    catch (const exception& e)
    {
        cout << "Exception[" << __FUNCTION__ << "]:" << e.what() << endl;
    }

    printf("Press Enter key to continue...\n");
    fgetc(stdin);

    return 0;
}
