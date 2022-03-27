#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>

using namespace std;

const char* RES_FILE = "badNumbers6.txt";

int main(int argc, char *argv[]) {
    ifstream f;
    string path = RES_FILE;
    if (argc > 1)
        path = argv[1];
    f.open(path, ios::in);
    if (!f.is_open()) {
        cerr << "Cannot open a file " << path << endl;
        return (-1);
    }
    while (true) {
        string s;

        //f >> s;
        string line;
        std::getline(f, line);
        if (f.fail())
            break;
        istringstream str(line);
        str >> s;
        if (str.fail())
            continue;

        int len = s.length();
        bool zeroFound = false;
        for (int i = 0; !zeroFound && i < len; ++i) {
            if (s[i] == '0') {
                zeroFound = true;
            }
        }
        if (!zeroFound) {
            cout << "Bad number: " << s << endl;
        }
    }
    f.close();
    return 0;
}
