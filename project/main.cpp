# include "pub.h"
# include "webservice.h"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;

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
    catch (const exception& err)
    {
        cout << "Exception[" << __FUNCTION__ << "]:" << err.what() << endl;
    }

    printf("Press Enter key to continue...\n");
    fgetc(stdin);

    return 0;
}
