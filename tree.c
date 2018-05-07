#include "tree.h"
#include <math.h>
#define MAX_DEPTH 500
unsigned depth = 0;
unsigned aPos = 0;

/*
Funkcja wybierająca dziecko, do którego cząstka ma trafić
*/
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

/*
Funkcja 'wkładająca' cząstkę do węzła
*/
void insertObject(Node* root, Data* obj) {
    if(obj->position.x >= 1 || obj->position.x <= -1 || obj->position.y >=1 || obj->position.y <= -1) {
        //jeżeli poza obszarem nie uwzględniamy
        return;
    }
    depth++;
    root->size = root->size+1;
    if (root->size == 1) { //jeśli pierwsza cząstka
        root->obj = obj;
    }
    else {
        double offset = root->dim / 2;

        //tworzenie węzłów potomnych
        createNode(&(root->ne), offset, root->position.x + offset, root->position.y + offset); 
        createNode(&(root->nw), offset, root->position.x - offset, root->position.y + offset);
        createNode(&(root->se), offset, root->position.x + offset, root->position.y - offset);
        createNode(&(root->sw), offset, root->position.x - offset, root->position.y - offset);
        if(root->size == 2) {
            if(depth == MAX_DEPTH || (fabs(root->obj->position.x - obj->position.x) < 0.01 && fabs(root->obj->position.y - obj->position.y) < 0.01)) {
                obj->mass += root->obj->mass;//jeśli cząstki są zbyt blisko siebie, jedna jest ignorowana, druga zwiększa masę
                root->obj = obj;
                return;
            }
            insertIntoQuad(root, root->obj);//dodaj cząstkę do dziecka jeśli była jedyna w węźle
            root->obj = NULL;
        }
        insertIntoQuad(root, obj);
    }
    root->isCleared = false;
}

/*
Funkcja dodająca cząstkę do drzewa
*/
void insert(Tree* tree, Data* obj) {
    depth = 0;
    insertObject(tree->head, obj); //dodaj do korzenia
}

/*
Funkcja budująca drzewo na podstawie otrzymanych cząstek
*/
void buildTree(Tree* tree, Data* objects, size_t num) {
    for(int i = 0; i < num; i++) {
        aPos = i;
        insert(tree, &objects[i]);//dodaj do drzewa
    }
}

/*
Funkcja tworząca drzewo
*/
void createTree(Tree** tree) {
    *tree = (Tree*)malloc(sizeof(Tree));//alokacja pamięci
    (*tree)->head = NULL;
    createNode(&((*tree)->head), 1, 0, 0);//tworzenie korzenia
}

/*
Funkcja tworząca węzeł drzewa
*/
void createNode(Node** node, double dim, double x, double y) {
    if (*node != NULL) 
        return;
    (*node) = (Node*)malloc(sizeof(Node)); //alokacja pamięci
    //parametry domyślne
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

/*
Funkcja czyszcząca węzeł drzewa
*/
void clearNode(Node* node) {
    if(node == NULL)
        return;
    
    //ustawienie parametrów
    node->size = 0;
    node->isCleared = true;
    node->obj = NULL;
    
    //czyszczenie dzieci
    clearNode(node->ne);
    clearNode(node->nw);
    clearNode(node->se);
    clearNode(node->sw);
}

/*
Funkcja czyszcząca drzewo
*/
void clearTree(Tree* tree) {
    clearNode(tree->head);//wyczyść korzeń
}

/*
Funkcja usuwająca węzeł
*/
void deleteNode(Node* node) {
    if(node == NULL){
        return;
    }
    //usuń dzieci
    deleteNode(node->nw);
    deleteNode(node->ne);
    deleteNode(node->sw);
    deleteNode(node->se);
    free(node); //zwolnienie pamięci
}
/*
Funkcja usuwająca drzewo
*/
void deleteTree(Tree* tree) {
    if(tree == NULL)
        return;
    deleteNode(tree->head); //usuń korzeń
    free(tree);//zwolnienie pamięci
}

/*
Funkcja licząca  środek masy
*/
void computeCOM(Tree* tree) {
    computeNodeCOM(tree->head); //liczenie środka masy dla korzenia
}

/*
Funkcja licząca środek masy dla węzłą
*/
Com computeNodeCOM(Node* node) {
    Com com;
    if (node == NULL || node->size == 0 || node->isCleared) {
        //jeśli węzeł nie istenieje lub jest pusty lub wyczyszczony, brak środka masy
        com.position.x = 0;
        com.position.y = 0;
        com.mass = 0;
        return com;
    }
    else if (node->obj != NULL) { //jeśli ma dokładnie jedną cząstkę
        //środek masy ma takie same parametry jak ta cząstka
        com.position.x = node->obj->position.x;
        com.position.y = node->obj->position.y;
        com.mass = node->obj->mass;
    }
    else {
        Com comNE = computeNodeCOM(node->ne);
        Com comNW = computeNodeCOM(node->nw);
        Com comSE = computeNodeCOM(node->se);
        Com comSW = computeNodeCOM(node->sw);
        //środek masy jest średnią ważoną środków mas dzieci
        com.mass = (comNE.mass + comNW.mass + comSE.mass + comSW.mass);
        if (com.mass == 0) {
            com.position.x = 0;
            com.position.y = 0;
            com.mass = 0;
            return com;
        }
        com.position.x = (comNE.position.x * comNE.mass + comNW.position.x * comNW.mass + comSE.position.x * comSE.mass + comSW.position.x * comSW.mass) / com.mass;
        com.position.y = (comNE.position.y * comNE.mass + comNW.position.y * comNW.mass + comSE.position.y * comSE.mass + comSW.position.y * comSW.mass) / com.mass; 
    }
    node->com = com;
    return com;
}

/*
Funkcja licząca odległość między cząstkami
*/
double distance(Data* p1, Data* p2)
{
    double r = pow(p1->position.x - p2->position.x, 2) + pow(p1->position.y - p2->position.y, 2);//r^2
    return r;
}

/*
Funkcja licząca odległość między cząstką a środkiem masy węzła
*/
double distanceNode(Data* p1, Node* p2)
{
    double r = pow(p1->position.x - p2->com.position.x, 2) + pow(p1->position.y - p2->com.position.y, 2);//r^2
    return r;
}

/*
Funcka licząca siłę oddziałującą na cząstkę podaną w argumencie
*/
Vector2D calculateForce(Tree* tree, Data* data) {
    if(tree == NULL) {
        Vector2D ret;
        ret.x = 0;
        ret.y = 0;
        return ret;
    }    
    return calculateForceNode(data, tree->head); //licz siłę z korzenia
}

/*
Funckja licząca siłę działającą na cząstkę pochodzącą od innej cząstki
*/
Vector2D forceObjects(Data* data, Data* data2) {
    Vector2D f;
    f.x = 0;
    f.y = 0;
    double r = distance(data, data2);//kwadrat odległości
    if(fabs(r) < 0.01) {//jeśli to ta sama cząstka
        return f;
    }
    double value = G * data->mass * data2->mass / r;//F=GMm/r^2
    f.x = value * (data2->position.x - data->position.x) / sqrt(r); //Fx = F*cos(phi)=F*x/r
    f.y = value * (data2->position.y - data->position.y) / sqrt(r); //Fy = F*sin(phi)=F*y/r
    return f;
}

/*
Funckja licząca siłę działającą na cząstkę pochodzącą od środka masy
*/
Vector2D forceNode(Data* data, Node* data2) {
    Vector2D f;
    f.x = 0;
    f.y = 0;
    double r = distanceNode(data, data2);//kwadrat odległości
    if(fabs(r) < 0.01) {//jeśli to ta sama cząstka
        return f;
    }
    double value = G * data->mass * data2->com.mass / r;//F=GMm/r^2
    f.x = value * (data2->com.position.x - data->position.x) / sqrt(r); //Fx = F*cos(phi)=F*x/r
    f.y = value * (data2->com.position.y - data->position.y) / sqrt(r); //Fy = F*sin(phi)=F*y/r
    return f;
}

/*
Funkcja liccząca siłę działającą na cząstkę wynikającą z podanego węzła
*/
Vector2D calculateForceNode(Data* data, Node* node) {
    Vector2D ret;
    ret.x = 0;
    ret.y = 0;
    double dist = 0.0;
    double D = node->dim;
    double theta = 0.1;
    if(node->size == 0 || (node->obj != NULL && fabs(data->position.x - node->obj->position.x) < 0.01 && fabs(data->position.y - node->obj->position.y) < 0.01) || node->isCleared)
    {
        //jeśli nie ma cząstek lub jest to ta, dla której liczona jest siła
        return ret;
    }
    else if(node->size == 1) //jeśli jest jedna cząstka
    {
        Vector2D ff = forceObjects(data,node->obj);//licz siłę bezpośrednio
	    return ff;
    }
    else //jeśli więcej cząstek
    {
        dist = sqrt(distanceNode(data, node));//odległość
        if(D/dist < theta) //odległość 10x większa niż szerokość węzłą 
        {
            return forceNode(data, node); //licz siłę wynikającą ze środka masy
        }
        else
        {
            //sumuj siły z każdego z dzieci
            Vector2D vec;
            vec.x = 0;
            vec.y = 0;
            vec = calculateForceNode(data, node->ne);
            ret.x += vec.x;
            ret.y += vec.y;
            vec = calculateForceNode(data, node->nw);
            ret.x += vec.x;
            ret.y += vec.y;
            vec = calculateForceNode(data, node->se);
            ret.x += vec.x;
            ret.y += vec.y;
            vec = calculateForceNode(data, node->sw);
            ret.x += vec.x;
            ret.y += vec.y;
            return ret;
        }
    }
}
