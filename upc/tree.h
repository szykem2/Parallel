#ifndef __TREE_H__
#define __TREE_H__
#include "data.h"
#include <stdbool.h>

#include "data.h"
#define G 0.001 //6.67*pow(10,-11)

typedef struct Com {
    Vector2D position;
    double mass;
} Com;

typedef struct Node {
    Vector2D position;
    Com com;
    double dim;
    size_t size;
    bool isCleared;
    struct Node* ne;
    struct Node* nw;
    struct Node* se;
    struct Node* sw;
    Data* obj;
} Node;

typedef struct Tree {
    Node *head;
} Tree;

void insertIntoQuad(Node* root, Data* obj);
void insertObject(Node* root, Data* obj);
void insert(Tree* tree, Data* obj);
void buildTree(Tree* tree, Data* objects, size_t num);
void createTree(Tree** tree);
void createNode(Node** node, double dim, double x, double y);
void clearNode(Node* node);
void clearTree(Tree* tree);
void deleteNode(Node* node);
void deleteTree(Tree* tree);
void computeCOM(Tree* tree);
Com computeNodeCOM(Node* node);
double distance(Data* p1, Data* p2);
double distanceNode(Data* p1, Node* p2);
Vector2D calculateForce(Tree* tree, Data* data);
Vector2D forceObjects(Data* data, Data* data2);
Vector2D forceNode(Data* data, Node* data2);
Vector2D calculateForceNode(Data* data, Node* node);
#endif
