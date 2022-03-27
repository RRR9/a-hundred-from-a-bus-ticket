#include <vector>
#include "catalan.h"

using namespace std;

int generateTrees(
    int numTerminalNodes,
    std::vector<Tree>& trees,
    int beginDigitIdx
) {
    trees.clear();
    if (numTerminalNodes == 0)
        return 1;
    if (numTerminalNodes == 1) {
        trees.resize(1);
        Tree& t = trees.back();
        t.numNodes = 1;
        t.beginDigitIdx = beginDigitIdx;
        t.endDigitIdx = beginDigitIdx + 1;
        t.root = nodePool.allocateNode();
        TreeNode& rootNode = nodePool.node(t.root);
        rootNode.digit = beginDigitIdx;
        return 1;
    }

    int catalanNumber = 0;
    vector<Tree> leftTrees;
    vector<Tree> rightTrees;
    for (int nLeft = 1; nLeft <= numTerminalNodes - 1; ++nLeft) {
        int nRight = numTerminalNodes - nLeft;
        int numLeft = generateTrees(
            nLeft, leftTrees, beginDigitIdx
        );
        for (int i = 0; i < numLeft; ++i) {
            Tree& tLeft = leftTrees.at(i);

            int numRight = generateTrees(
                nRight, rightTrees, beginDigitIdx + nLeft
            );

            for (int j = 0; j < numRight; ++j) {
                Tree& tRight = rightTrees.at(j);
                int rootIdx = nodePool.allocateNode();
                TreeNode& rootNode = nodePool.node(rootIdx);
                rootNode.left = tLeft.root;
                rootNode.right = tRight.root;

                trees.resize(trees.size() + 1);
                Tree& newTree = trees.back();
                newTree.root = rootIdx;
                newTree.numNodes = numTerminalNodes;
                newTree.beginDigitIdx = beginDigitIdx;
                newTree.endDigitIdx = beginDigitIdx + numTerminalNodes;

                ++catalanNumber;
            }
        }
    }
    return catalanNumber;
}

void setInitialOperations(std::vector<int>& operations) {
    operations[0] = 0;  // Not used
    for (int i = 1; i < int(digits.size()); ++i) {
        operations[i] = FIRST_OPERATION;
    }
}

bool nextOperations(std::vector<int>& operations) {
    // Zero element is not used!
    int i = int(operations.size() - 1);
    int c = 1;
    while (i > 0 && c != 0) {
        int nextOp = operations[i] + c;
        if (nextOp <= LAST_OPERATION) {
            operations[i] = nextOp;
            c = 0;
        } else {
            operations[i] = FIRST_OPERATION;
            c = 1;
        }
        --i;
    }
    return (c == 0);
}

void setInitialDigits(std::vector<int>& digits) { // 102345
    if (digits.size() < 2)
        return;
    digits[0] = 0;      // Not used
    digits[1] = 1;
    for (int i = 2; i < int(digits.size()); ++i) {
        if (i == 2) {
            digits[i] = 0;
        } else {
            digits[i] = i-1;
        }
    }
}

bool nextDigits(std::vector<int>& digits) {
    if (!incrementDigits(digits))
        return false;
    while (haveDoubleDigit(digits)) {
        if (!incrementDigits(digits))
            return false;
    }
    return true;
}

void setInitialUnMinuses() {
    for (size_t i = 0; i < unMinuses.size(); ++i)
        unMinuses[i] = false;
}

bool nextUnMinuses() {
    // Zero element is not used!
    int i = numUnMinusPositions - 1;
    bool c = true;      // Carry bit
    while (i >= 0 && c) {
        int pos = unMinusPositions[i];
        if (unMinuses[pos]) {
            unMinuses[pos] = false;
            // c = true;
        } else {
            unMinuses[pos] = true;
            c = false;
        }
        --i;
    }
    return (!c);
}

bool haveDoubleDigit(std::vector<int>& digits) {
    int n = int(digits.size());
    for (int i = 1; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (digits[i] == digits[j])
                return true;
        }
    }
    return false;
}

bool incrementDigits(std::vector<int>& digits) {
    // Zero elements is not used!
    int i = int(digits.size() - 1);
    int c = 1;
    while (i > 0 && c != 0) {
        int nextDigit = digits[i] + c;
        c = 0;
        if (nextDigit > 9) {
            nextDigit = 0;
            c = 1;
        }
        digits[i] = nextDigit;
        --i;
    }
    return (c == 0);
}
