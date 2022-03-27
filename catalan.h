#include <vector>
#include "fullTree.h"

int generateTrees(     // returns catalan number of numTerminalNodes - 1
    int numTerminalNodes,
    std::vector<Tree>& trees,
    int beginDigitIdx
);

void setInitialOperations(std::vector<int>& operations);
bool nextOperations(std::vector<int>& operations);

void setInitialDigits(std::vector<int>& digits);
bool nextDigits(std::vector<int>& digits);
bool incrementDigits(std::vector<int>& digits);
bool haveDoubleDigit(std::vector<int>& digits);

void setInitialUnMinuses();
bool nextUnMinuses();
