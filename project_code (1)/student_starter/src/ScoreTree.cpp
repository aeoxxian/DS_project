#include "ds/ScoreTree.h"
#include <iostream>

ScoreTree::ScoreTree() : root(nullptr), count(0) {}

ScoreTree::~ScoreTree() {
    clear();
}

void ScoreTree::insert(const ScoreRecord& record) {
    // TODO(Student): Implement BST insertion.
    // Suggested ordering: lower scores go left, higher or equal scores go right.
    root = insertRecursive(root, record);
}

ScoreTree::Node* ScoreTree::insertRecursive(Node* current, const ScoreRecord& record) {
    // TODO(Student): Implement recursive insertion.
    // Remember to increment count when a new node is created.
    (void)record;
    return current;
}

bool ScoreTree::containsPlayer(const std::string& playerName) const {
    return containsRecursive(root, playerName);
}

bool ScoreTree::containsRecursive(Node* current, const std::string& playerName) const {
    // TODO(Student): Implement tree search by player name.
    // Since the tree is ordered by score, searching by name may require checking both subtrees.
    (void)current;
    (void)playerName;
    return false;
}

void ScoreTree::printDescending() const {
    if (root == nullptr) {
        std::cout << "No scores recorded.\n";
        return;
    }
    printDescendingRecursive(root);
}

void ScoreTree::printDescendingRecursive(Node* current) const {
    // TODO(Student): Implement reverse in-order traversal: right, current, left.
    (void)current;
    std::cout << "[TODO] ScoreTree::printDescendingRecursive is not implemented yet.\n";
}

void ScoreTree::clearRecursive(Node* current) {
    if (current == nullptr) {
        return;
    }
    clearRecursive(current->left);
    clearRecursive(current->right);
    delete current;
}

int ScoreTree::size() const {
    return count;
}

bool ScoreTree::isEmpty() const {
    return count == 0;
}

void ScoreTree::clear() {
    clearRecursive(root);
    root = nullptr;
    count = 0;
}
