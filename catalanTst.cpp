#include <iostream>
#include "fullTree.h"
#include "catalan.h"

using namespace std;

int main() {
    int n;
    while (true) {
        cout << "n: ";
        cin >> n;
        if (cin.fail())
            break;

        digits.resize(n);
        operations.resize(n);
        unMinuses.resize(2*n);

        vector<Tree> trees;
        int cn = generateTrees(n, trees, 1);
        cout << cn << "trees are generated." << endl << endl;
        for (size_t t = 0; t < trees.size(); ++t) {
            Tree& tree = trees.at(t);
            tree.setOperations(1);
            tree.setUnMinuses(1);
        }

        int operationSets = 0;
        int correctTrees = 0;
        setInitialOperations(operations);
        while (true) {
            cout << "Operation set: ";
            for (size_t i = 1; i < operations.size(); ++i) {
                cout << operation2char(operations[i]) << " ";
            }
            cout << endl;

            for (size_t i = 0; i < trees.size(); ++i) {
                trees[i].invalidate();
                if (trees[i].checkOperations())
                    ++correctTrees;
            }
            ++operationSets;
            if (!nextOperations(operations))
                break;
        }
        cout << "Number of all operation sets = " << operationSets << endl;
        cout << "Number of correct trees = " << correctTrees << endl;

        int numberSets = 0;
        setInitialDigits(digits);
        while (true) {
            for (int i = 1; i <= n; ++i) {
                cout << char(digits[i] + '0');
            }
            cout << endl;
            ++numberSets;
            if (!nextDigits(digits))
                break;
        }
        cout << "Number of digit sets=" << numberSets << endl << endl;

        /*
        vector<bool> unMinuses(2*n);
        setInitialUnMinuses(unMinuses);
        while (true) {
            for (int i = 1; i <= 2*n - 1; ++i) {
                cout << unMinuses[i];
            }
            cout << endl;
            if (!nextUnMinuses(unMinuses))
                break;
        }
        cout << endl;
        */
    }
    return 0;
}
