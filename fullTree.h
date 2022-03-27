#ifndef FULL_TREE
#define FULL_TREE

#include <vector>
#include <string>

const int MAX_DIGITS = 10;

// Operations (the orders is significant!)
const int OP_NOOP = 0;
const int OP_PLUS = 1;
const int OP_MINUS = 2;
const int OP_MUL = 3;
const int OP_DIV = 4;
const int OP_CONNECT = 5;   // Connect a digit to a number 25 & 3 = 253

const int FIRST_OPERATION = OP_PLUS;
const int LAST_OPERATION = OP_CONNECT;
const int NUM_OPERATIONS = 5;

inline char operation2char(int op) {
    switch(op) {
        case OP_NOOP: return ' ';
        case OP_PLUS: return '+';
        case OP_MINUS: return '-';
        case OP_MUL: return '*';
        case OP_DIV: return '/';
        case OP_CONNECT: return '&';
        default: return '?';
    }
}

const double ILLEGAL_VALUE = 1e30;

// All nodes of trees are in the common node pull
// that is an array of nodes. The references to child
// nodes are indices in that array.
// Operation are also listed in a common array, tree nodes
// contain indices in it.

class TreeNode {
public:
    int left;       // Index of left subtree root, 0 for terminal node
    int right;      // Index of right subtree root, 0 for terminal node
    int operation;  // Index in array of operations.
    bool isNumber;  // This is a number (1-digit, 2-digits, 3 digits...)
    int digit;      // Index in digits array, 0 for non-terminal nodes
    int unMinusIdx; // Index in array of unary minuses
    double value;
    bool computed;
    bool illegalOperations;     // Illegal combination of operations
    bool zeroDivision;

public:
    TreeNode(
        int lSubtree = 0,
        int rSubtree = 0,
        int op = 0,
        bool is_number = false,
        int dig = 0,
        int unMinIdx = 0,
        double v = 0.,
        bool comp = false,
        bool illegal = false,
        bool zero = false
    ):
        left(lSubtree),
        right(rSubtree),
        operation(op),
        isNumber(is_number),
        digit(dig),
        unMinusIdx(unMinIdx),
        value(v),
        computed(comp),
        illegalOperations(illegal),
        zeroDivision(zero)
    {}

    bool isTerminal() const {
        return (left == 0);
    }
    bool isIllegal() const { 
        return (illegalOperations || zeroDivision);
    }

    void invalidateNode() { computed = false; zeroDivision = false; }
    void invalidateSubtree();
    bool compute();
    double treeValue() {
        if (compute()) {
            return value;
        } else {
            return ILLEGAL_VALUE;
        }
    }
    bool checkOperations(); // Check correctness of OP_CONNECT operations

    int setDigits(int firstDigitIdx); // Returs number of terminal nodes
    int setOperations(int firstOperationIdx); // Returs number 
                                              // of non-terminal nodes
    int setUnMinuses(int firstUnMinusIdx); // Returs number of all nodes
    int defineUnminusPositions(     // Returns number of
        int firstUnMinusPosIdx,     // possible unminus positions
        bool leftSon = true     // Subtree root is a left son of its parent
    ) const;
    std::string toFormula(bool showConnect = false) const;
    static void dumpSubtree(int nodeIdx, int level = 0);
    void dumpNode(int level = 0) const;
};

// Pool of nodes
class TreeNodePool: public std::vector<TreeNode> {
public:
    TreeNodePool() {
        resize(1);
    }

    int allocateNode() {
        resize(size() + 1);
        return int(size() - 1);
    }

    const TreeNode& node(int i) const {
        return at(i);
    }

    TreeNode& node(int i) {
        return at(i);
    }
};

extern TreeNodePool nodePool;
extern std::vector<int> digits;
extern std::vector<int> operations;
extern std::vector<bool> unMinuses;
extern std::vector<int> unMinusPositions;
extern int numUnMinusPositions;

class Tree {
public:
    int numNodes;       // Number of all nodes (terminal and non-terminal)
    int beginDigitIdx;  // Index of the first digit
    int endDigitIdx;    // Index after the last digit
    int root;           // Index of root node
public:
    Tree(
        int num_nodes = 0,
        int begin_digit_idx = 0,
        int end_digit_idx = 0,
        int r = 0
    ):
        numNodes(num_nodes),
        beginDigitIdx(begin_digit_idx),
        endDigitIdx(end_digit_idx),
        root(r)
    {}
    bool checkOperations() { // Check correctness of OP_CONNECT operations
        return nodePool.node(root).checkOperations();
    }
    int defineUnminusPositions() const {
        if (root == 0)
            return 0;
        numUnMinusPositions = 
            nodePool.node(root).defineUnminusPositions(0, true);
        return numUnMinusPositions;
    }
    bool compute() {
        return nodePool.node(root).compute();
    }
    double value() {
        return nodePool.node(root).treeValue();
    }
    void invalidate() {
        nodePool.node(root).invalidateSubtree();
    }
    int setDigits(int firstDigitIdx) {
        if (root == 0)
            return 0;
        return nodePool.node(root).setDigits(firstDigitIdx);
    }
    int setOperations(int firstOperationIdx) {
        if (root == 0)
            return 0;
        return nodePool.node(root).setOperations(firstOperationIdx);
    }
    bool isIllegalOperations() const {
        if (root == 0)
            return true;
        return nodePool.node(root).illegalOperations;
    }

    bool isIllegal() const {
        if (root == 0)
            return true;
        return nodePool.node(root).isIllegal();
    }

    int setUnMinuses(int firstUnMinusIdx) {
        if (root == 0)
            return 0;
        return nodePool.node(root).setUnMinuses(firstUnMinusIdx);
    }
    std::string toFormula(bool showConnect = false) const {
        return nodePool.node(root).toFormula(showConnect);
    }
    void dumpTree() const;
};

#endif
