// TestAssignment1.cpp : Defines the entry point for the application.
//

#include "TestAssignment1.h"

using namespace std;

enum class mode {database_creation = 0};

constexpr std::string_view modeTypes[] =
{
    "database_creation"
};

int main(int argc, char* argv[])
{
    int modeNum;
    switch (argc) {
    case 1:
        std::cout << "No arguement detected\n";
        break;
    case 2:
        modeNum = atoi(argv[1]);
        modeNum--;
        std::cout << "Usage mode: " << modeTypes[modeNum] << "\n";
        break;
    default:
        std::cout << "Too many arguements detected\n";
    }
    return 0;
}
