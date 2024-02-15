#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 63

typedef struct node
{
    char* name;
    int generation;
    struct node *mom; //left
    struct node *dad; //right
    int family;
    
} t_node;

t_node* createNode(char* name) {
    t_node* newNode = malloc(sizeof(t_node));
    if (newNode == NULL) {
        fprintf(stdout, "Memory allocation failed.\n");
        exit(1);
    }
    newNode->name = strdup(name); // Allocate memory for the name and copy it
    newNode->mom = NULL;
    newNode->dad = NULL;
    newNode->generation = 0;
    newNode->family = 0;

    return newNode;
}

void freeMemory(t_node* nodeArray[], int numNodes){
    for (int i = 0; i < numNodes; i++) {
        free(nodeArray[i]->name);
        free(nodeArray[i]);
    }
}

void printFamily(t_node* nodeArray[], int numNodes, int family) {
    // Find the maximum generation within the family
    int maxGeneration = 0;
    for (int i = 0; i < numNodes; i++) {
        if (nodeArray[i]->family == family && nodeArray[i]->generation > maxGeneration) {
            maxGeneration = nodeArray[i]->generation;
        }
    }
    
    // Print family members in descending order by generation
    for (int generation = maxGeneration; generation >= 0; generation--) {
        for (int i = 0; i < numNodes; i++) {
            if (nodeArray[i]->family == family && nodeArray[i]->generation == generation) {
                printf("%s\n", nodeArray[i]->name);
            }
        }
    }
    printf("\n");
}

void printNodes(t_node* nodeArray[], int numNodes) {
    int maxFamily = 0;
    for (int i = 0; i < numNodes; i++) {
        if (nodeArray[i]->family > maxFamily) {
            maxFamily = nodeArray[i]->family;
        }
    }
    for (int family = 1; family <= maxFamily; family++) {
        printFamily(nodeArray, numNodes, family);
    }
}

void dfs(t_node* startNode, int family) {
    if (startNode == NULL || startNode->family != 0) return; // Already visited or belongs to another family

    startNode->family = family; // Mark the node as visited and assign it to the current family

    // Recursive DFS on mom and dad nodes
    dfs(startNode->mom, family);
    dfs(startNode->dad, family);
}

void separateFamilies(t_node* nodeArray[], int numNodes) {
    int currentFamily = 1;

    for (int i = 0; i < numNodes; i++) {
        if (nodeArray[i]->family == 0) {
            // Perform DFS from unvisited nodes to identify the family
            dfs(nodeArray[i], currentFamily);

            // Check if the parent nodes belong to the same family
            if (nodeArray[i]->mom != NULL && nodeArray[i]->mom->family != currentFamily) {
                // Assign the family number of the parent node to the current node
                nodeArray[i]->family = nodeArray[i]->mom->family;

                // Recursively update the family numbers of the parent nodes
                separateFamilies(nodeArray, numNodes);
            } else if (nodeArray[i]->dad != NULL && nodeArray[i]->dad->family != currentFamily) {
                // Assign the family number of the parent node to the current node
                nodeArray[i]->family = nodeArray[i]->dad->family;

                // Recursively update the family numbers of the parent nodes
                separateFamilies(nodeArray, numNodes);
            }

            currentFamily++;
        }
    }
}


void updateGenerations(t_node* node) {
    if (node == NULL) return;

    if (node->mom != NULL) {
        node->mom->generation = node->generation + 1;
        updateGenerations(node->mom);
    }
    if (node->dad != NULL) {
        node->dad->generation = node->generation + 1;
        updateGenerations(node->dad);
    }
}


t_node* findNodeByName(t_node* nodeArray[], int numNodes, const char* name) {
    for (int i = 0; i < numNodes; i++) {
        if (strcmp(nodeArray[i]->name, name) == 0) {
            return nodeArray[i];
            
        }
    }
    return NULL; // Node not found
}

void addFather(t_node* currentNode, t_node* fatherNode) {
    if (currentNode->dad != NULL) {
        fprintf(stdout, "Kļūda: 2 tēvi nav atļauti.\n");
        exit(1);
    }
    if (fatherNode->dad == currentNode || fatherNode->mom == currentNode) {
        fprintf(stdout, "Kļūda: cikliskas attiecības.\n");
        exit(1);
    }

    currentNode->dad = fatherNode;
}

void addMother(t_node* currentNode, t_node* motherNode) {
    if (currentNode->mom != NULL) {
        fprintf(stdout, "Kļūda: 2 mātes nav atļautas.\n");
        exit(1);
    }
    if (motherNode->dad == currentNode || motherNode->mom == currentNode) {
        fprintf(stdout, "Kļūda: cikliskas attiecības.\n");
        exit(1);
    }

   currentNode->mom = motherNode;
}

int main() {
    char line[MAX_LINE_LENGTH];
    char* name= NULL;
    char* father = NULL;
    char* mother = NULL;


    t_node* nodeArray[10000]; // Array to store pointers to created nodes
    int numNodes = 0;

    while (fgets(line, MAX_LINE_LENGTH, stdin) != NULL) {
    if (strstr(line, "VARDS")) {
        name = strstr(line, "VARDS") + 6;
        name[strcspn(name, "\n")] = 0; // Remove newline character
        // Check if the node already exists
        t_node* person = findNodeByName(nodeArray, numNodes, name);
        if (person == NULL) {
            // Create a node for the person
            person = createNode(name);
            nodeArray[numNodes++] = person;
        }
    } else if (strstr(line, "TEVS")) {
        father = strstr(line, "TEVS") + 5;
        father[strcspn(father, "\n")] = 0;
    
        // Check if the father node already exists
        t_node* fatherNode = findNodeByName(nodeArray, numNodes, father);
        if (fatherNode == NULL) {
            // Create a node for the father
            fatherNode = createNode(father);
            nodeArray[numNodes++] = fatherNode;
        }
    } else if (strstr(line, "MATE")) {
        mother = strstr(line, "MATE") + 5;
        mother[strcspn(mother, "\n")] = 0;

        // Check if the mother node already exists
        t_node* motherNode = findNodeByName(nodeArray, numNodes, mother);
        if (motherNode == NULL) {
            // Create a node for the mother
            motherNode = createNode(mother);
            nodeArray[numNodes++] = motherNode;
        }
    }
}


rewind(stdin);
t_node* currentNode; 

    while (fgets(line, MAX_LINE_LENGTH, stdin) != NULL) {
    if (strstr(line, "VARDS")) {
        name = strstr(line, "VARDS") + 6;
        name[strcspn(name, "\n")] = 0; // Remove newline character

        // Find the current node by name
            currentNode = findNodeByName(nodeArray, numNodes, name);
        if (currentNode == NULL) {
            return 1;
        }
    } else if (strstr(line, "TEVS")) {
        father = strstr(line, "TEVS") + 5;
        father[strcspn(father, "\n")] = 0;
        
        // Find the father node by name
        t_node* fatherNode = findNodeByName(nodeArray, numNodes, father);
        if (fatherNode == NULL) {
            return 1;
        }

        // Add father to the current node
        addFather(currentNode, fatherNode);
    } else if (strstr(line, "MATE")) {
        mother = strstr(line, "MATE") + 5;
        mother[strcspn(mother, "\n")] = 0;

        // Find the mother node by name
        t_node* motherNode = findNodeByName(nodeArray, numNodes, mother);
        if (motherNode == NULL) {
            return 1;
        }

        // Add mother to the current node
        addMother(currentNode, motherNode);
    }
}

 for (int i = 0; i < numNodes; i++) {
        if (nodeArray[i]->generation == 0) {
            updateGenerations(nodeArray[i]);
        }
    }
 

separateFamilies(nodeArray, numNodes);
printNodes(nodeArray, numNodes);   

   freeMemory(nodeArray, numNodes);

    return 0;
}
