#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "fullTree.h"
#include "catalan.h"

using namespace std;

const double EPS = 1e-8;

const char* const CONTINUE_FILE = "t100.txt";

// File name without ext.
const char* const RES_UNMINUS_NUMBERS = "unMinusNumbers"; 
const char* const RES_BAD_NUMBERS = "badNumbers";
const char* const RES_EXT = ".txt";

string pathBadNumbers;
string pathUnminusNumbers;

static bool saveState();
static bool restoreState();
static void printDigits();
static bool saveUnminusNumber(bool bad = false);
static bool saveBadNumber();

static int numDigits = 6;

const int SAVE_FREQUENCY = 100;

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        if (!restoreState()) {
            cout << "Number of digits: ";
            cin >> numDigits;
            if (numDigits < 2 || numDigits > MAX_DIGITS) {
                cout << "Incorrect number of digits" << endl;
                return (-1);
            }
            digits.resize(numDigits + 1);
            setInitialDigits(digits);
        }
    } else {
        numDigits = atoi(argv[1]);
        if (numDigits < 2 || numDigits > MAX_DIGITS) {
            cout << "Incorrect number of digits" << endl;
            return (-1);
        }
        digits.resize(numDigits + 1);
        setInitialDigits(digits);
    }

    // Define output paths
    pathBadNumbers = RES_BAD_NUMBERS;
    //... pathBadNumbers += std::to_string(numDigits);
    char numDig[16];
    sprintf(numDig, "%d", numDigits);
    pathBadNumbers += numDig;
    pathBadNumbers += RES_EXT;

    pathUnminusNumbers = RES_UNMINUS_NUMBERS;
    //... pathUnminusNumbers += std::to_string(numDigits);
    pathUnminusNumbers += numDig;
    pathUnminusNumbers += RES_EXT;

    int numOperations = numDigits - 1;
    operations.resize(numOperations + 1);
    unMinuses.resize(2*numDigits);
    digits[0] = 0;
    operations[0] = 0;
    unMinuses[0] = false;

    vector<Tree> trees;
    int n = generateTrees(numDigits, trees, 1);
    // cout << n << " trees are generated." << endl;

    for (size_t t = 0; t < trees.size(); ++t) {
        Tree& tree = trees.at(t);
        tree.setOperations(1);
        tree.setUnMinuses(1);
        //+++ tree.dumpTree();
    }

    // Loop for all numbers
    int step = 0;
    while (true) {
        bool solved = false;

        cout << "Step=" << (step + 1) << " Digits: ";
        printDigits();

        // First pass: not using unary minus
        bool unminusSolution = false;
        // Loop for all trees
        for (size_t t = 0; !solved && t < trees.size(); ++t) {
            Tree& tree = trees.at(t);

            // Loop for all sets of operations
            setInitialOperations(operations);

            // First pass: not using unary minus
            setInitialUnMinuses();
            while (!solved) {
                if (tree.checkOperations()) {
                    //... tree.defineUnminusPositions();

                    tree.invalidate();
                    if (tree.compute()) {
                        if (fabs(tree.value() - 100.) <= EPS) {
                            solved = true;

                            cout << " " << tree.toFormula(true) << " = "
                                << tree.value() << endl;

                            break;
                        }
                    }
                } // end if (tree.checkOperations())

                if (!nextOperations(operations))
                   break;
            } // end loop for all sets of operations
        } // end loop for all trees
        // End of first pass without using an unary minus

        if (!solved) {
            // Second pass: using unary minus
            unminusSolution = true;
            // Loop for all trees
            for (size_t t = 0; !solved && t < trees.size(); ++t) {
                Tree& tree = trees.at(t);

                // Loop for all sets of operations
                setInitialOperations(operations);

                while (!solved) {
                    if (tree.checkOperations()) {
                        tree.defineUnminusPositions();

                        // Loop for all sets of unary minuses
                        setInitialUnMinuses();
                        while (!solved && nextUnMinuses()) {

                            tree.invalidate();
                            if (tree.compute()) {
                                if (fabs(tree.value() - 100.) <= EPS) {
                                    solved = true;

                                    cout << " with un.minus " <<
                                        tree.toFormula(true) << " = "
                                        << tree.value() << endl;

                                    break;
                                }
                            }

                        } // end loop for all sets of unary minuses
                    } // end if (tree.checkOperations())

                    if (!nextOperations(operations))
                       break;
                } // end loop for all sets of operations

            } // end loop for all trees
            // End of second pass with using an unary minus

        } // end if (!solved)   

        if (!solved) {
            cout << endl << "Bad number: ";
            printDigits();
            cout << endl;
            saveBadNumber();
            saveUnminusNumber(true);
        } else if (solved && unminusSolution) {
            cout << "Needs unary minus: ";
            printDigits();
            cout << endl;
            saveUnminusNumber(false);
        }

        ++step;
        if (step % SAVE_FREQUENCY == 0) {
            saveState();
            // cout << ".";
            // cout.flush();
        }

        if (!nextDigits(digits))
            break;
    }
    cout << "Finished." << endl;
    return 0;
}

static bool saveState() {
    ofstream f;
    f.open(CONTINUE_FILE, ios::out | ios::trunc);
    if (!f.is_open())
        return false;
    f << numDigits << endl;
    for (size_t i = 1; i < digits.size(); ++i) {
        if (i > 1)
            f << " ";
        f << digits.at(i);
    }
    f << endl;
    f.close();
    return true;
}

static bool restoreState() {
    ifstream f;
    f.open(CONTINUE_FILE, ios::in);
    if (!f.is_open())
        return false;
    f >> numDigits;
    if (f.fail())
        return false;
    if (numDigits <= 0 || numDigits > MAX_DIGITS) {
        f.close();
        return false;
    }

    digits.resize(numDigits + 1);
    for (int i = 1; i <= numDigits; ++i) {
        f >> digits.at(i);
        if (f.fail()) {
            f.close();
            return false;
        }
    }
    f.close();
    return true;
}

static void printDigits() {
    for (size_t i = 1; i < digits.size(); ++i)
        cout << digits[i];
    // cout << endl;
}

static bool saveUnminusNumber(bool bad) {
    ofstream f;
    //... f.open(pathUnminusNumbers, ios::out | ios::app);
    f.open(pathUnminusNumbers.c_str(), ios::out | ios::app);
    if (!f.is_open())
        return false;
    for (int i = 1; i <= numDigits; ++i) {
        f << digits.at(i);
    }
    if (bad) {
        f << " cannot make 100 even with un. minus";
    }
    f << endl;
    f.close();
    return true;
}

static bool saveBadNumber() {
    ofstream f;
    //... f.open(pathBadNumbers, ios::out | ios::app);
    f.open(pathBadNumbers.c_str(), ios::out | ios::app);
    if (!f.is_open())
        return false;
    for (int i = 1; i <= numDigits; ++i) {
        f << digits.at(i);
    }
    f << endl;
    f.close();
    return true;
}
