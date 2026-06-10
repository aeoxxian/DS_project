#include "ds/ScoreTree.h"

ScoreTree::Node* ScoreTree::copyTree(const Node* node) const {
    if (!node) return nullptr;
    Node* n    = new Node(node->record);
    n->left    = copyTree(node->left);
    n->right   = copyTree(node->right);
    return n;
}

void ScoreTree::clearHelper(Node* node) {
    if (!node) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void ScoreTree::insert(const ScoreRecord& r) {
    Node** cur = &root;
    while (*cur) {
        if (r.score < (*cur)->record.score) cur = &(*cur)->left;
        else                                cur = &(*cur)->right;
    }
    *cur = new Node(r);
}

void ScoreTree::printDescending() const {
    int rank = 1;
    printDesc(root, rank);
}

void ScoreTree::printDesc(Node* node, int& rank) const {
    if (!node) return;
    printDesc(node->right, rank);
    std::cout << "  " << rank++ << ". " << node->record.name
              << "  (효율 점수: " << node->record.score << ")\n";
    printDesc(node->left, rank);
}
