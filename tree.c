#include "tree.h"
#include <math.h>

void insertIntoQuad(Node* root, Data* obj) {
    if (obj->position.x > root->position.x) {
        if(obj->position.y > root->position.y) {
            insertObject(root->ne, obj);
        }
        else {
            insertObject(root->se, obj);
        }
    }
    else {
        if(obj->position.y > root->position.y) {
            insertObject(root->nw, obj);
        }
        else {
            insertObject(root->sw, obj);
        }
    }
}

void insertObject(Node* root, Data* obj) {
    root->size = root->size+1;
    if (root->size == 1) {
        root->obj = obj;
    }
    else {
        double offset = root->dim / 2;
        createNode(&(root->ne), offset, root->position.x + offset, root->position.y + offset); 
        createNode(&(root->nw), offset, root->position.x - offset, root->position.y + offset);
        createNode(&(root->se), offset, root->position.x + offset, root->position.y - offset);
        createNode(&(root->sw), offset, root->position.x - offset, root->position.y - offset);
        insertIntoQuad(root, obj);
        if(root->size == 2) {
            insertIntoQuad(root, root->obj);
            root->obj = NULL;
        }
    }
    root->isCleared = false;
}

void insert(Tree* tree, Data* obj) {
    insertObject(tree->head, obj);
}

void buildTree(Tree* tree, Data* objects, size_t num) {
    for(int i = 0; i < num; i++) {
        insert(tree, &objects[i]);
    }
}

void createTree(Tree** tree) {
    *tree = (Tree*)malloc(sizeof(Tree));
    (*tree)->head = NULL;
    createNode(&((*tree)->head), 1, 0, 0);
}

void createNode(Node** node, double dim, double x, double y) {
    if (*node != NULL) 
        return;
    (*node) = (Node*)malloc(sizeof(Node));
    (*node)->nw = NULL;
    (*node)->ne = NULL;
    (*node)->sw = NULL;
    (*node)->se = NULL;
    (*node)->dim = dim;
    (*node)->position.x = x;
    (*node)->position.y = y;
    (*node)->com.mass = 0;
    (*node)->com.position.x = 0;
    (*node)->com.position.y = 0;
    (*node)->size = 0;
    (*node)->isCleared = false;
}

void clearNode(Node* node) {
    if(node == NULL)
        return;
    
    node->size = 0;
    node->isCleared = true;
    node->obj = NULL;
    
    clearNode(node->ne);
    clearNode(node->nw);
    clearNode(node->se);
    clearNode(node->sw);
}

void clearTree(Tree* tree) {
    clearNode(tree->head);
}

void deleteNode(Node* node) {
    if(node == NULL){
        return;
    }
    deleteNode(node->nw);
    deleteNode(node->ne);
    deleteNode(node->sw);
    deleteNode(node->se);
    free(node);
}

void deleteTree(Tree* tree) {
    if(tree == NULL)
        return;
    deleteNode(tree->head);
    free(tree);
}

void computeCOM(Tree* tree) {
    computeNodeCOM(tree->head);
}

Com computeNodeCOM(Node* node) {
    Com com;
    if (node == NULL || node->size == 0 || node->isCleared) {
        com.position.x = 0;
        com.position.y = 0;
        com.mass = 0;
        return com;
    }
    else if (node->obj != NULL) {
        com.position.x = node->obj->position.x;
        com.position.y = node->obj->position.y;
        com.mass = node->obj->mass;
        printf("pos x: %lf y: %lf mass: %lf\n", com.position.x, com.position.y, com.mass);
    }
    else {
        Com comNE = computeNodeCOM(node->ne);
        Com comNW = computeNodeCOM(node->nw);
        Com comSE = computeNodeCOM(node->se);
        Com comSW = computeNodeCOM(node->sw);
        com.mass = (comNE.mass + comNW.mass + comSE.mass + comSW.mass);
        if (com.mass == 0) {
            com.position.x = 0;
            com.position.y = 0;
            com.mass = 0;
            return com;
        }
        com.position.x = (comNE.position.x * comNE.mass + comNW.position.x * comNW.mass + comSE.position.x * comSE.mass + comSW.position.x * comSW.mass) / com.mass;
        com.position.y = (comNE.position.y * comNE.mass + comNW.position.y * comNW.mass + comSE.position.y * comSE.mass + comSW.position.y * comSW.mass) / com.mass;
        printf("pos x: %lf y: %lf mass: %lf\n", com.position.x, com.position.y, com.mass);
    }
    node->com = com;
    return com;
}

long double distance(Data* p1, Data* p2)
{
    printf("distance obj pos x1: %le, y1: %le, x2: %le, y2: %le\n", p1->position.x,p1->position.y,p2->position.x,p2->position.y);
    long double r = pow(p1->position.x - p2->position.x, 2) + pow(p1->position.y - p2->position.y, 2);//r^2
    return r;
}

long double distanceNode(Data* p1, Node* p2)
{
    printf("distance node pos x1: %le, y1: %le, x2: %le, y2: %le\n", p1->position.x,p1->position.y,p2->position.x,p2->position.y);
    long double r = pow(p1->position.x - p2->com.position.x, 2) + pow(p1->position.y - p2->com.position.y, 2);//r^2
    return r;
}

Vector2D calculateForce(Tree* tree, Data* data) {
    if(tree == NULL) {
        Vector2D ret;
        ret.x = 0;
        ret.y = 0;
        return ret;
    }    
    return calculateForceNode(data, tree->head);
}

Vector2D forceObjects(Data* data, Data* data2) {
    Vector2D f;
    long double r = distance(data, data2);
    double value = G * data->mass * data2->mass / r;
    f.x = value * (data->position.x - data2->position.x) / sqrt(r); //Fx = F*cos(phi)=F*x/r
    f.y = value * (data->position.y - data2->position.y) / sqrt(r); //Fy = F*sin(phi)=F*y/r
    printf("obj r: %lf, x: %lf, y: %lf\n", r, f.x,f.y);
    return f;
}

Vector2D forceNode(Data* data, Node* data2) {
    Vector2D f;
    long double r = distanceNode(data, data2);
    double value = G * data->mass * data2->com.mass / r;
    f.x = value * (data->position.x - data2->com.position.x) / sqrt(r); //Fx = F*cos(phi)=F*x/r
    f.y = value * (data->position.y - data2->com.position.y) / sqrt(r); //Fy = F*sin(phi)=F*y/r
    printf("node r: %lf, x: %lf, y: %lf\n", r, f.x,f.y);
    return f;
}

Vector2D calculateForceNode(Data* data, Node* node) {
    long double dist = 0.0;
    long double D = node->dim;
    long double theta = 1.0;
    Vector2D ret;
    ret.x = 0;
    ret.y = 0;
    if(node->size == 0 || &data == &(node->obj))
    {
        return ret;
    }

    else if(node->size == 1)
    {
	    return forceObjects(data,node->obj);
    }
    else
    {
        dist = distanceNode(data, node);
        if(D/dist < theta)
        {
            return forceNode(data, node);
        }
        else
        {
            Vector2D vec;
            vec.x = 0;
            vec.y = 0;
            vec = calculateForceNode(data, node->ne);
            printf("vec x: %lf y: %lf\n", vec.x, vec.y);
            ret.x += vec.x;
            ret.y += vec.y;
            vec = calculateForceNode(data, node->nw);
            printf("vec x: %lf y: %lf\n", vec.x, vec.y);
            ret.x += vec.x;
            ret.y += vec.y;
            vec = calculateForceNode(data, node->se);
            printf("vec x: %lf y: %lf\n", vec.x, vec.y);
            ret.x += vec.x;
            ret.y += vec.y;
            vec = calculateForceNode(data, node->sw);
            printf("vec x: %lf y: %lf\n", vec.x, vec.y);
            ret.x += vec.x;
            ret.y += vec.y;
            printf("vec x: %lf y: %lf\n", vec.x, vec.y);
            return ret;
        }
    }
}