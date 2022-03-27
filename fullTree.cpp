#include <cmath>
#include <cassert>
#include <string>
#include <iostream>
#include <cstdio>
#include "fullTree.h"

using namespace std;

// For dump
static void indent(int level) {
    cout << endl;
    for (int i = 0; i < level; ++i)
        cout << "  ";
}

TreeNodePool nodePool;
vector<int> digits(MAX_DIGITS + 1);
vector<int> operations(MAX_DIGITS);
vector<bool> unMinuses(2*MAX_DIGITS);
vector<int> unMinusPositions(MAX_DIGITS);
int numUnMinusPositions = 0;

bool TreeNode::compute() {
    /*???
    if (computed)
        return true;

    if (illegalOperations) {
        return false;
    }
    ???*/

    if (isTerminal()) {
        value = double(digits[digit]);
        if (unMinuses[unMinusIdx]) {
            if (digits[digit] != 0) {
                value = (-value);
                isNumber = false;
            } else {
                // Let us exclude (-0)
                computed = false;
                illegalOperations = true;
                value = ILLEGAL_VALUE;
                return false;
            }
        } else {
            isNumber = true;
        }
        computed = true;
        illegalOperations = false;
        return true;
    }

    assert(left != 0 && right != 0);
    double leftValue;

    TreeNode& leftNode = nodePool.node(left);
    if (!leftNode.compute()) {
        illegalOperations = true;
        computed = false;
        value = ILLEGAL_VALUE;
        return false;
    }

    TreeNode& rightNode = nodePool.node(right);
    if (!rightNode.compute()) {
        illegalOperations = true;
        computed = false;
        value = ILLEGAL_VALUE;
        return false;
    }

    int oper = operations[operation];
    switch (oper) {
        case OP_PLUS:
            value = leftNode.value + rightNode.value;
            isNumber = false;
            break;
        case OP_MINUS:
            value = leftNode.value - rightNode.value;
            isNumber = false;
            break;
        case OP_MUL:
            value = leftNode.value * rightNode.value;
            isNumber = false;
            break;
        case OP_DIV:
            if (fabs(rightNode.value) <= 0.) { // Zero division
                computed = false;
                illegalOperations = true;
                value = ILLEGAL_VALUE;
                return false;
            } else {
                value = leftNode.value / rightNode.value;
            }
            isNumber = false;
            break;
        case OP_CONNECT:
            if (
                (!leftNode.isTerminal() || digits[leftNode.digit] != 0) &&
                leftNode.isNumber && 
                (rightNode.isTerminal() && rightNode.isNumber)
            ) {
                value = leftNode.value * 10. + rightNode.value;
                isNumber = true;
            } else {
                computed = false;
                illegalOperations = true;
                isNumber = false;
                value = ILLEGAL_VALUE;
                return false;
            }
            break;
        default:
            assert(false);
            computed = false;
            illegalOperations = true;
            value = ILLEGAL_VALUE;
            return false;
    }

    computed = true;
    illegalOperations = false;
    if (unMinuses[unMinusIdx]) {
        if (value != 0.) {
            value = (-value);
            isNumber = false;
        } else {
            // Let us exclude formulas like (-0) or (-(1+5-6))
            computed = false;
            illegalOperations = true;
            value = ILLEGAL_VALUE;
            return false;
        }
    }
    return true;
}

int TreeNode::setDigits(int firstDigitIdx) { // Returs number of terminal nodes
    computed = false;
    illegalOperations = false;
    if (isTerminal()) {
        isNumber = true;
        digit = firstDigitIdx;
        return 1;
    }
    int numLeft = nodePool.node(left).setDigits(firstDigitIdx);
    int numRight = nodePool.node(right).setDigits(firstDigitIdx + numLeft);
    isNumber = false;
    return numRight + numLeft;
}

int TreeNode::setOperations(int firstOperationIdx) { // Returs number
                                                     // of non-terminal nodes
    if (isTerminal())
        return 0;
    computed = false;
    illegalOperations = false;
    int numLeft = nodePool.node(left).setOperations(firstOperationIdx);
    operation = firstOperationIdx + numLeft;
    int numRight = nodePool.node(right).
        setOperations(firstOperationIdx + numLeft + 1);
    isNumber = false;
    return numRight + numLeft + 1;
}

int TreeNode::setUnMinuses(int firstUnMinusIdx) { // Returs number
                                                  // of all nodes
    if (isTerminal()) {
        unMinusIdx = firstUnMinusIdx;
        return 1;
    }
    int numLeft = nodePool.node(left).setUnMinuses(firstUnMinusIdx);
    unMinusIdx = firstUnMinusIdx + numLeft;
    int numRight = nodePool.node(right).
        setUnMinuses(firstUnMinusIdx + numLeft + 1);
    return numRight + numLeft + 1;
}

int TreeNode::defineUnminusPositions(     // Returns number of
    int firstUnMinusPosIdx,     // possible unminus positions
    bool leftSon                // Subtree root is a left son of its parent
) const {
    if (illegalOperations)
        return 0;
    if (isTerminal()) {
        if (leftSon) {
            unMinusPositions[firstUnMinusPosIdx] = unMinusIdx;
            return 1;
        }
        return 0;
    }

    int oper = operations[operation];
    if (oper == OP_CONNECT) {
        if (leftSon) {
            unMinusPositions[firstUnMinusPosIdx] = unMinusIdx;
            return 1;
        }
        return 0;
    }

    const TreeNode& leftNode = nodePool.node(left);
    const TreeNode& rightNode = nodePool.node(right);
    int numLeftPos = leftNode.defineUnminusPositions(
        firstUnMinusPosIdx, true
    );
    int numRightPos = rightNode.defineUnminusPositions(
        firstUnMinusPosIdx + numLeftPos, false
    );
    return numLeftPos + numRightPos;
}

string TreeNode::toFormula(bool showConnect) const {
    bool unMinus = unMinuses[unMinusIdx];
    if (isNumber) {
        assert(!unMinus);
        int n = int(value);

        //... return to_string(n);
        char str[32];
        sprintf(str, "%d", n);
        return string(str);
    }
    if (isTerminal()) {
        // assert(unMinus);
        int n = int(value);

        //... return to_string(n);
        char str[32];
        sprintf(str, "%d", n);
        return string(str);
    }

    const TreeNode& leftNode = nodePool.node(left);
    const TreeNode& rightNode = nodePool.node(right);

    string leftStr = leftNode.toFormula(showConnect);
    assert(leftStr != "");

    string rightStr = rightNode.toFormula(showConnect);
    assert(rightStr != "");

    bool leftUnMinus = unMinuses[leftNode.unMinusIdx];
    if (leftUnMinus) {
        leftStr = "(" + leftStr + ")";
    }
    bool rightUnMinus = unMinuses[rightNode.unMinusIdx];
    if (rightUnMinus) {
        rightStr = "(" + rightStr + ")";
    }

    string res;
    int oper = operations[operation];
    if (oper == OP_PLUS) {
        res = leftStr + operation2char(oper) + rightStr;
    } else if (oper == OP_MINUS) {
        res = leftStr + operation2char(OP_MINUS);
        if (
            (!rightNode.isNumber) && 
            (
                operations[rightNode.operation] == OP_MINUS ||
                operations[rightNode.operation] == OP_PLUS
            ) &&
            (!rightUnMinus)
        ) {
            res += "(" + rightStr + ")";
        } else {
            res += rightStr;
        }
    } else if (oper == OP_MUL) {
        if (
            (!leftNode.isNumber) &&
            operations[leftNode.operation] != OP_MUL &&
            (!leftUnMinus)
        ) {
            res = "(" + leftStr + ")";
        } else {
            res = leftStr;
        }
        res += operation2char(oper);
        if (
            rightNode.isNumber ||
            operations[rightNode.operation] == OP_MUL ||
            operations[rightNode.operation] == OP_DIV ||
            rightUnMinus
        ) {
            res += rightStr;
        } else {
            res += "(" + rightStr + ")";
        }
    } else if (oper == OP_DIV) {
        if (
            (!leftNode.isNumber) &&
            operations[leftNode.operation] != OP_MUL &&
            (!leftUnMinus)
        ) {
            res = "(" + leftStr + ")";
        } else {
            res = leftStr;
        }
        res += operation2char(oper);
        if (rightNode.isNumber || rightUnMinus) {
            res += rightStr;
        } else {
            res += "(" + rightStr + ")";
        }
    } else if (oper == OP_CONNECT) {
        if (!illegalOperations) {
            assert(!leftUnMinus && !rightUnMinus);
            assert(unMinus);
            int n = int(value);

            //... res = to_string(n);
            char str[32];
            sprintf(str, "%d", n);
            return string(str);
        } else {
            if (unMinus) {
                res += "(-";
            }
            res += "(";
            res += leftStr;
            res += operation2char(oper);
            res += rightStr;
            res += ")";
            if (unMinus) {
                res += ")";
            }
        }
    }
    if (unMinus && oper != OP_CONNECT) {
        res = "-(" + res + ")";
    }
    return res;
}

void TreeNode::invalidateSubtree() {
    invalidateNode();
    if (!isTerminal()) {
        nodePool.node(left).invalidateSubtree();
        nodePool.node(right).invalidateSubtree();
    }
}

void TreeNode::dumpSubtree(int nodeIdx, int level) {
    const TreeNode& node = nodePool.node(nodeIdx);
    indent(level);
    cout << "nodeIdx=" << nodeIdx;
    node.dumpNode(level+1);
    if (node.left != 0)
        dumpSubtree(node.left, level + 1);
    if (node.right != 0)
        dumpSubtree(node.right, level + 1);
}

void TreeNode::dumpNode(int level) const {
    indent(level);
    cout << "left=" << left;
    cout << " right=" << right;
    cout << " opIdx=" << operation;
    cout << " operation=" << operation2char(operations[operation]);
    indent(level);
    cout << "isNumber=" << isNumber;
    cout << " digitIdx=" << digits[digit];
    cout << " unMinusIdx=" << unMinusIdx;
    cout << (unMinuses[unMinusIdx]? " -": " +");
    cout << " value=" << value;
    indent(level);
    cout << " computed=" << computed;
    cout << " illegal=" << illegalOperations;
}

void Tree::dumpTree() const {
    cout << "Tree:";
    TreeNode::dumpSubtree(root, 1);
    cout << endl << "End Tree" << endl;
}

bool TreeNode::checkOperations() { // Check correctness of OP_CONNECT
    if (isTerminal()) {
        illegalOperations = false;
        return true;
    }
    TreeNode& leftNode = nodePool.node(left);
    bool leftOk = leftNode.checkOperations();

    TreeNode& rightNode = nodePool.node(right);
    bool rightOk = rightNode.checkOperations();

    if (!leftOk || !rightOk) {
        illegalOperations = true;
        return false;
    }
    int oper = operations[operation];
    if (oper != OP_CONNECT) {
        illegalOperations = false;
        return true;
    }
    if (!rightNode.isTerminal()) {
        illegalOperations = true;
        return false;
    }
    if (leftNode.isTerminal()) {

        /*???
        if (digits[digit] != 0) {
            illegalOperations = false;
            return true;
        } else {
            // The number should not start from zero, like 012
            illegalOperations = true;
            return false;
        }
        ???*/

        illegalOperations = false;
        return true;
    }
    int leftOper = operations[leftNode.operation];
    illegalOperations = (leftOper != OP_CONNECT);
    return !illegalOperations;
}
