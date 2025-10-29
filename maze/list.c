#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct Node {
    int neigbor[4];
    int posI;
    int posJ;
    struct Node * next;
} Node;

typedef struct list
{
  struct Node * firstNode;
  int size;
} List;

List * createList();
void removeList(List * l);
bool isEmpty(List * l);
void addNode(List * l, int posI, int posJ, int left, int down, int right, int up);
struct Node * createNode(int posI, int posJ, int left, int down, int right, int up);
bool removeNode(List * l, int posI, int posJ);
bool isMember(List * l, int posI, int posJ);
struct Node * getNode(List * l, int posI, int posJ);
void printList(List * l);
bool updateNode(List * l, int posI, int posJ, int newRight, int newDown, int newLeft, int newUp);



List * createList()
{
    List * newList = malloc(sizeof(List));
    newList->firstNode = NULL;
    newList->size = 0;
    return newList;

}

void removeList(List * l)
{
    free(l);
}

bool isEmpty(List * l)
{
    return (l->size == 0);
}

void addNode(List * l, int posI, int posJ, int right, int down, int left, int up)
{
    struct Node * newNode = createNode(posI, posJ, right, down, left, up);
    if (isEmpty(l)){        // if the list is empty then create the first node
        l->firstNode = newNode;
        l->size = l->size + 1;
    }
    else{                   //if the list got more nodes, then it cycle until it gets the last node
        struct Node * currentNode = l->firstNode;
        while(currentNode->next != NULL)
        currentNode = currentNode->next;
        currentNode->next = newNode; // then we add the new Node to the last node next value
        l->size = l->size + 1;      // and increment the size of the list
    }
}

struct Node * createNode(int posI, int posJ, int right, int down, int left, int up)
{
    struct Node * node = malloc(sizeof(struct Node));
    node->posI = posI;
    node->posJ = posJ;
    node->neigbor[0] = right;
    node->neigbor[1] = down;
    node->neigbor[2] = left;
    node->neigbor[3] = up;
    node->next = NULL;
    return node;
     
}

bool removeNode(List * l, int posI, int posJ)
{
    if(isEmpty(l)) 
        return false;
    
    struct Node * currentNode = l->firstNode;      // this points to the first node
    struct Node * lastNode = NULL;                 // this points to the last visited node
    struct Node * nextNode = currentNode->next;    // this points to the next node

    

    for(int i = 0; i < l->size; i++){       // we cycle through the list until we get to the node to be deleted or to the final of the list
        if ((currentNode->posI == posI) && (currentNode->posJ == posJ)){ // when we find the node
            if(currentNode == l->firstNode){                             // if is the first node
                l->firstNode = nextNode;                                 // we update the first node with the next node
                free(currentNode);                                       // we free the memory space of the deleted node
                l->size = l-> size - 1;                                  // we update the size of the list
                return true;                                             // and return true to tell the program it was a succesfully deletion
            }
            else{
                lastNode->next = nextNode;                              // we update the current node with the next node
                l->size = l->size - 1;                                     // we update the size of the list
                free(currentNode);                                      // we free the memory space of the deleted node
                return true;                                            // and return true to tell the program it was a succesfully deletion
            }
        }
        else{
            lastNode = currentNode;                                     // if we havent found the correct node to be deleted we update the nodes 
            currentNode = nextNode;                                     // lastNode gets the values of currentNode, currentNode gets the values 
            nextNode = nextNode->next;                                  // nextNode, and nextNode becomes nextNode->next
        }
            
    }

    return false;                                                       // if we didnt find the correct node we return false

    
}

bool isMember(List * l, int posI, int posJ)
{
    if(isEmpty(l)) 
        return false;
    
    struct Node * currentNode = l->firstNode;      // this points to the first node

    for(int i = 0; i < l->size; i++){       // we cycle through the list until we get to the node to be deleted or to the final of the list
        if ((currentNode->posI == posI) && (currentNode->posJ == posJ))
                return true;                                             // if we find the node return true
        else
            currentNode = currentNode->next;                             // if we havent found the correct node, we update currentNode to the next Node
    }

    return false;                                                       // if we didnt find the correct node we return false

    
}

struct Node * getNode(List * l, int posI, int posJ)
{
    if(isEmpty(l)) 
        return NULL;
    
    struct Node * currentNode = l->firstNode;      // this points to the first node

    if(isMember(l, posI, posJ)){
            for(int i = 0; i < l->size; i++){       // we cycle through the list until we get to the node to be deleted or to the final of the list
                if ((currentNode->posI == posI) && (currentNode->posJ == posJ))
                    return currentNode;                                             // if we find the node return true
                else
                    currentNode = currentNode->next;                             // if we havent found the correct node, we update currentNode to the next Node
            }
    }
    else
        return NULL;
    
        
}

void printList(List * l)
{
    if(isEmpty(l)) 
        printf("empty list\n");
    
    struct Node * currentNode = l->firstNode;      // this points to the first node
    int i = 0;
    while(currentNode != NULL){       // we cycle through the list until we get to the node to be deleted or to the final of the list
        printf("Node %d: ", i);
        printf("i=%d ", currentNode->posI);
        printf("i=%d ", currentNode->posJ);
        printf("neigbors = [%d, ", currentNode->neigbor[0]);
        printf("%d, ", currentNode->neigbor[1]);
        printf("%d, ", currentNode->neigbor[2]);
        printf("%d]\n", currentNode->neigbor[3]);
        i++;
        currentNode = currentNode->next;                             // if we havent found the correct node, we update currentNode to the next Node
    }
}

bool updateNode(List * l, int posI, int posJ, int newRight, int newDown, int newLeft, int newUp)
{
    if (isMember(l, posI, posJ)){
        struct Node * temp = getNode(l, posI, posJ);
        temp->neigbor[0] = temp->neigbor[0] + newRight;
        temp->neigbor[1] = temp->neigbor[1] + newDown;
        temp->neigbor[2] = temp->neigbor[2] + newLeft;
        temp->neigbor[3] = temp->neigbor[3] + newUp;
        return true;
    }
    else return false;
}




/* int main(){
    List * listica = createList();

    printList(listica);
    printf("%d\n", removeNode(listica, 0,9));

    addNode(listica, 0, 0,0,0,0,0);
    addNode(listica, 0, 1,0,0,0,0);
    addNode(listica, 0, 2,0,0,0,0);
    addNode(listica, 0, 3,0,0,0,0);
    addNode(listica, 0, 4,0,0,0,0);
    addNode(listica, 0, 5,0,0,0,0);
    addNode(listica, 0, 6,0,0,0,0);
    addNode(listica, 0, 7,0,0,0,0);
    addNode(listica, 0, 8,0,0,0,0);
    addNode(listica, 0, 9,0,0,0,0);
    printList(listica);
    printf("\n");
    removeNode(listica, 0,3);
    printList(listica);
    printf("\n");

    updateNode(listica, 0,0,0,1,2,4);
    printList(listica);

    removeList(listica);
}*/


 