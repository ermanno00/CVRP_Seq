#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

typedef struct {
    int id;
    double demand;
    bool is_routed;
} Node;

typedef struct {
    Node current_node;
    struct RoutedNode* nextNode;
} RoutedNode;

typedef struct {
    double cost;
    double load;
    RoutedNode* routedNode;
} Vehicle;

typedef struct {
    Node* nodes;
    Vehicle* vehicles;
    double** distanceMatrix;
    int number_of_customer;
    int number_of_vehicles;
} Problem;


typedef struct {
    bool found;
    Node closest_node;
} ClosestNodeResult;


void init_problem(Problem* p, int noc, int demand_range, int nov, int capacity, int grid_range);
ClosestNodeResult find_closest(Vehicle v, Node last_node, Problem p);
bool all_routed(Problem p);
void greedy_solve(Problem * p);
double random_double(double min, double max);


// Function to print the elements of the linked list
void printList(RoutedNode * head) {
    RoutedNode * current = head;
    while (current != NULL) {
        printf("%d -> ", current->current_node.id);
        current = (RoutedNode *) current->nextNode;
    }
    printf("NULL\n");
}

int main() {

    const int noc = 20;
    const int demand_range = 10;
    const int nov = 3;
    const int capacity = 50;
    const int grid_range = 10;
    Problem p;

    init_problem(&p, noc, demand_range, nov, capacity, grid_range);

    printf("Greedy: \n");
    greedy_solve(&p);
    printf("\n");

    for (int i = 0; i < noc; i++) {
        for (int j = 0; j < noc; j++) {
            printf("%f ",p.distanceMatrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    for(int i =0; i<nov; i++){
        printf("veicolo %d con rotta: ",i);
        printList(p.vehicles[i].routedNode);
    }


    return 0;
}

// Function to create a new node with the given data
RoutedNode * createNode_seq(Node node) {
    RoutedNode * newNode = (RoutedNode*)malloc(sizeof(RoutedNode));
    newNode->current_node = node;
    newNode->nextNode = NULL;
    return newNode;
}

// Function to insert a new node at the end of the linked list
void insertAtEnd_seq(RoutedNode ** head, Node data) {
    RoutedNode* newNode = createNode_seq(data);

    if (*head == NULL) {
        *head = newNode;
    } else {
        RoutedNode* current = *head;
        while (current->nextNode != NULL) {
            current = (RoutedNode *) current->nextNode;
        }
        current->nextNode = (struct RoutedNode *) newNode;
    }
}

// Function to get the last element of the linked list
RoutedNode * getLastElement_seq(RoutedNode * head) {
    if (head == NULL) {
        return NULL; // Empty list
    }

    RoutedNode * current = head;
    while (current->nextNode != NULL) {
        current = (RoutedNode *) current->nextNode;
    }

    return current;
}




void init_problem(Problem* p, int noc, int demand_range, int nov, int capacity, int grid_range) {

    // Allocate memory for nodes, vehicles, and distanceMatrix as needed
    p->nodes = (Node*)malloc(noc * sizeof(Node));
    p->vehicles = (Vehicle*)malloc(nov * sizeof(Vehicle));
    p->distanceMatrix = (double**)malloc(noc * sizeof(double*));
    p->number_of_customer =noc;
    p->number_of_vehicles = nov;
    for (int i = 0; i < noc; ++i) {
        p->distanceMatrix[i] = (double*)malloc(noc * sizeof(double));
    }

    //init distance matrix with random value
    for (int i = 0; i < noc; ++i) {
        for (int j = 0; j < noc; ++j) {
            p->distanceMatrix[i][j] = random_double(0.0, grid_range);
        }
    }


    //init nodes with random demand
    for (int i = 0; i < noc; ++i) {
        p->nodes[i].id = i;
        p->nodes[i].demand = random_double(0.0, (double) demand_range);
        p->nodes[i].is_routed = false;
    }

    //depot has 0 demand and is routed
    p->nodes[0].demand =0;
    p->nodes[0].is_routed = true;

    //init vehicles with initial capacity
    //set first nodes which is depot
    for (int i = 0; i < nov; ++i) {
        p->vehicles[i].load = (double)capacity;
        p->vehicles[i].cost = 0.0;
        insertAtEnd_seq(&p->vehicles[i].routedNode, p->nodes[0]);
    }

}

// Function to generate a random double in the range [min, max]
double random_double_seq(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

bool all_routed_seq(Problem p) {

    for (int i = 0; i < p.number_of_customer; i++) {
        if (!p.nodes[i].is_routed) {
            return false;
        }
    }
    return true;
}




void greedy_solve_seq(Problem * p) {

    for (int i = 0; i < p->number_of_vehicles; i++) {
        while(!all_routed_seq(*p)) {
            RoutedNode *last_routed_node = getLastElement_seq(p->vehicles->routedNode);
            ClosestNodeResult result = find_closest(
                    p->vehicles[i],
                    last_routed_node->current_node,
                    *p
            );

            if (result.found) {
                p->vehicles[i].load -= result.closest_node.demand;
                p->vehicles[i].cost += p->distanceMatrix[last_routed_node->current_node.id][result.closest_node.id];

                // Update the linked list of routed nodes for the vehicle
                insertAtEnd_seq(&p->vehicles[i].routedNode, result.closest_node);
                p->nodes[result.closest_node.id].is_routed = true;
            } else {
                //adding depot
                p->vehicles[i].cost += p->distanceMatrix[last_routed_node->current_node.id][0];
                insertAtEnd_seq(&p->vehicles[i].routedNode, p->nodes[0]);
                break;
            }
        }
    }

    double cost = 0.0;
    for (int i = 0; i < p->number_of_vehicles; i++) {
        cost += p->vehicles[i].cost;
    }
    printf("Cost: %lf\n", cost);
    printf("Solution valid: %d\n", all_routed_seq(*p));
}


ClosestNodeResult find_closest_seq(Vehicle v, Node last_node, Problem p) {
    double cost = DBL_MAX;
    size_t id = 0;

    bool found = false;
    double * distances = p.distanceMatrix[last_node.id];


    for (size_t j = 0; j < p.number_of_customer; j++) {
        if (!p.nodes[j].is_routed && p.nodes[j].demand <= v.load && distances[j] < cost) {
            cost = distances[j];
            id = j;
            found = true;
        }
    }

    ClosestNodeResult result;
    result.found = found;
    if (found) {
        result.closest_node = p.nodes[id];
    }

    return result;
}

