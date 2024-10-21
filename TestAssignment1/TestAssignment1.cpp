#include "TestAssignment1.h"

using namespace std;

enum class Mode {database_creation = 0};

constexpr std::string_view modeTypes[] =
{
    "database_creation"
};

int main(int argc, char* argv[])
{
    switch (argc) {
    case 1: {
        std::cout << "No arguement detected\n";
    }
        break;
    case 2: {
        int modeNum;
        modeNum = atoi(argv[1]);
        modeNum--;
        const int modesNum = 0; // maximum amount of modes minus 1
        if (modeNum < 0 || modeNum > modesNum) {
            std::cout << "Unknown usage mode: " << modeNum + 1 << "\n";
            break;
        }
        
        std::cout << "Usage mode: " << modeTypes[modeNum] << "\n";
        Mode mode = static_cast<Mode>(modeNum);
        switch (mode) {
        case Mode::database_creation:
            break;
        }
    }
        break;
    default: {
        std::cout << "Too many arguements detected\n";
    }
    }
    return 0;
}
