#include <iostream>
#include <string>
#include <set>
#include <cctype>
#include <cmath>
#include <cassert>
#include "fullTree.h"
#include "catalan.h"

using namespace std;

const double EPS = 1e-8;
const int MAX_UNMINUS_SOLUTIONS = 100;

int main() {
    while (true) {
        string s;
        cout << "Ticket number: ";
        cin >> s;
        int lenStr = s.length();
        if (lenStr > 0 && !isdigit(s[0]))
            break;      // Exit
        for (int i = 0; i < lenStr; ++i) {
            if (!isdigit(s[i])) {
                cout << "Incorrect string (should be a number)." << endl;
                continue;
            }
        }
        int numDigits = lenStr;
        int numOperations = numDigits - 1;
        digits.resize(numDigits + 1);
        operations.resize(numOperations + 1);
        unMinuses.resize(2*numDigits);

        digits[0] = 0;
        operations[0] = 0;
        unMinuses[0] = false;

        for (int i = 0; i < numDigits; ++i) {
            digits[i + 1] = s[i] - '0';
        }

        vector<Tree> trees;
        int n = generateTrees(numDigits, trees, 1);
        // cout << n << " trees are generated." << endl;

        for (size_t t = 0; t < trees.size(); ++t) {
            Tree& tree = trees.at(t);
            tree.setOperations(1);
            tree.setUnMinuses(1);
            //+++ tree.dumpTree();
        }

        set<string> solutions;
        set<string> unminusSolutions;
        int numUnminusSolutions = 0;

        // Loop for all trees
        for (size_t t = 0; t < trees.size(); ++t) {
            Tree& tree = trees.at(t);

            // Loop for all sets of operations
            setInitialOperations(operations);
            while (true) {
                if (tree.checkOperations()) {
                    tree.defineUnminusPositions();
                    assert(numUnMinusPositions < numDigits);

                    // Loop for all sets of unary minuses
                    setInitialUnMinuses();
                    bool unminusSolution = false;
                    while (true) {
                        /*
                        //+++ Print operations
                        for (int j = 1; j <= numDigits; ++j)
                             cout << operation2char(operations[j]) << ' ';
                        cout << endl;
                        cout << "Tree before computing" << endl;
                        tree.dumpTree();
                        */

                        tree.invalidate();

                        //+++ cout << tree.toFormula() << endl;
                        //+++ tree.dumpTree();

                        if (tree.compute()) {
                            if (fabs(tree.value() - 100.) <= EPS) {
                                string s = tree.toFormula();
                                if (!unminusSolution) {
                                    if (solutions.find(s) == solutions.end()) {
                                        solutions.insert(s);

                                        // Print a solution that does not
                                        // use an unary minus
                                        cout << s << " = 100" << endl;
                                    }
                                } else {
                                    if (unminusSolutions.find(s) == unminusSolutions.end()) {
                                        unminusSolutions.insert(s);
                                        ++numUnminusSolutions;

                                        // Print a solution that
                                        // use an unary minus
                                        //... cout << s << " = 100" << endl;
                                        // Will be printed at the end
                                    }
                                }

                                /*
                                cout << "Tree after computing" << endl;
                                tree.dumpTree();
                                cout << tree.toFormula(true) << " = " <<
                                    tree.value() << endl;
                                */
                            }
                        }

                        /*
                        cout << "Tree after computing" << endl;
                        tree.dumpTree();
                        cout << tree.toFormula(true) << " = " <<
                            tree.value() << endl;
                        */
                        if (!nextUnMinuses())
                            break;
                        unminusSolution = true;
                    } // end loop for all sets of unary minuses
                } // end if (tree.checkOperations())

                if (!nextOperations(operations))
                    break;
            } // end loop for all sets of operations

        } // end loop for all trees

        // Print solutions that use an unary minus
        // (solutions without unary minus are printed alread
        set<string>::const_iterator i = unminusSolutions.begin();
        int k = 0;
        while (i != unminusSolutions.end() && k <MAX_UNMINUS_SOLUTIONS) {
            cout << *i << " = 100" << endl;
            ++i; ++k;
        }
        if (numUnminusSolutions > MAX_UNMINUS_SOLUTIONS)
            cout << "..." << endl;
        cout << "Solutions without unary minus: " 
            << solutions.size() << endl;
        cout << "Solutions with unary minus: " 
            << unminusSolutions.size() << endl;
    }
}
