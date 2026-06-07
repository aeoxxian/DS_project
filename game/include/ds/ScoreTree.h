#ifndef SCORE_TREE_H
#define SCORE_TREE_H

#include <string>
#include <iostream>

struct ScoreRecord {
    std::string name;
    int         score;
    ScoreRecord() : score(0) {}
    ScoreRecord(const std::string& n, int s) : name(n), score(s) {}
};

class ScoreTree {
private:
    struct Node {
        ScoreRecord record;
        Node* left;
        Node* right;
        Node(const ScoreRecord& r) : record(r), left(nullptr), right(nullptr) {}
    };

    Node* root;

    void clearHelper(Node* node);
    void printDesc(Node* node, int& rank) const;

public:
    ScoreTree() : root(nullptr) {}
    ~ScoreTree() { clearHelper(root); }

    void insert(const ScoreRecord& r);
    void printDescending() const;
};

#endif
