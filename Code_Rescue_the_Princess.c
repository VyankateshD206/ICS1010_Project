#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Coord {
    int x, y, z;
} Coord;

typedef struct Node {
    Coord position;
    int steps;
    int parentIndex; // Index of the parent node in the stack
} Node;

typedef struct Stack {
    Node* array;
    int top;
    unsigned capacity;
} Stack;

// Function declarations
int is_valid(int x, int y, int z);
Coord setup(char c[100]);
int calculate_distance(Coord p1, Coord p2);
int win_check(int k, Coord* Monsters);
bool can_move(Coord from, Coord to);
void print_moves(Coord* print_array, int n);
bool dfs(Coord* Monsters, int k);

// Global variables
int n, m, o; // Board Size
Coord Princess, Mario;

int main() {
    printf("Enter board size: ");
    scanf("%d %d %d", &n, &m, &o); // Getting Board

    Princess = setup("Princess"); // Getting Coordinates of Princess
    Mario = setup("Mario");       // Getting Coordinates of Mario

    int k;
    printf("Enter number of monsters: ");
    scanf("%d", &k);

    Coord* Monsters = (Coord*)malloc(k * sizeof(Coord));

    for (int i = 0; i < k; i++) {
        Monsters[i] = setup("Monster");
    }

    system("cls");

    if (win_check(k, Monsters) == 0) {
        printf("The GAME cannot be WON!!!\n");
    } else {
        // Starts to make a strategy
        if (dfs(Monsters, k)) {
            printf("VICTORY!!!\n");
        } else {
            printf("The GAME cannot be WON!!!\n");
        }
    }

    free(Monsters);
    return 0;
}

int is_valid(int x, int y, int z) {
    return (x >= 0 && x < n && y >= 0 && y < m && z >= 0 && z < o);
}

Coord setup(char c[100]) {
    int x, y, z;
    printf("Enter Coordinates for %s: ", c);

    while (1) {
        scanf("%d %d %d", &x, &y, &z);

        if (is_valid(x, y, z))
            break; // Checking for coordinate validity
        else {
            printf("The coordinates entered are wrong!!!\n");
            printf("Please Enter Valid Coordinates for %s: ", c);
        }
    }

    Coord charac = { x, y, z };
    return charac;
}

int calculate_distance(Coord p1, Coord p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y) + abs(p1.z - p2.z);
}

int win_check(int k, Coord* Monsters) {
    int princessToMario = calculate_distance(Princess, Mario);

    for (int i = 0; i < k; i++) {
        int princessToMonster = calculate_distance(Princess, Monsters[i]);
        int marioToMonster = calculate_distance(Mario, Monsters[i]);

        if (princessToMario == 0)
            return 1; // Game is already won but steps need to be
        else if ((princessToMonster <= marioToMonster && princessToMonster < princessToMario) || princessToMonster == 0 || marioToMonster == 0) {
            return 0; // Definitely lose conditions.
        }
    }
    return 1; // Game should continue.
}

bool can_move(Coord from, Coord to) {
    return calculate_distance(from, to) == 1;
}

void print_moves(Coord* print_array, int n) {
    
    printf("(%d, %d, %d)", print_array[0].x,print_array[0].y, print_array[0].z);

    for (int i=1; i<=n; i++)
    {
        printf("->(%d, %d, %d)", print_array[i].x,print_array[i].y, print_array[i].z);
    }
    printf("\n");
}

Stack createStack(unsigned capacity) {
    Stack stack;
    stack.capacity = capacity;
    stack.top = -1;
    stack.array = (Node*)malloc(capacity * sizeof(Node));
    return stack;
}

void push(Stack* stack, Node item) {
    if (stack->top == stack->capacity - 1) {
        // Stack is full
        return;
    }
    stack->array[++stack->top] = item;
}

Node pop(Stack* stack,Coord* print_array) {
    if (stack->top == -1) {
        // Stack is empty
        Node emptyNode = {{0, 0, 0}, 0, -1}; // Parent index -1 indicates no parent
        return emptyNode;
    }
    int steps=stack->array[stack->top].steps;
    print_array[steps].x = stack->array[stack->top].position.x;
    print_array[steps].y = stack->array[stack->top].position.y;
    print_array[steps].z = stack->array[stack->top].position.z;
    return stack->array[stack->top--];
}

bool isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

void freeStack(Stack* stack) {
    free(stack->array);
}

bool dfs(Coord* Monsters, int k) {
    Stack stack = createStack(n * m * o);

    // Mark all cells as not visited
    bool*** visited = (bool***)malloc(n * sizeof(bool**));
    for (int i = 0; i < n; i++) {
        visited[i] = (bool**)malloc(m * sizeof(bool*));
        for (int j = 0; j < m; j++) {
            visited[i][j] = (bool*)malloc(o * sizeof(bool));
            for (int a = 0; a < o; a++) {
                visited[i][j][a] = false;
            }
        }
    }

    // Mark Mario's position as visited
    visited[Mario.x][Mario.y][Mario.z] = true;

    // Push Mario's position along with the path onto the stack
    Node start = {Mario, 0, -1}; // -1 indicates no parent
    push(&stack, start);


    // Arrays to represent possible moves in the 3D space
    int dx[] = {1, -1, 0, 0, 0, 0};
    int dy[] = {0, 0, 1, -1, 0, 0};
    int dz[] = {0, 0, 0, 0, 1, -1};

    Coord print_array[n*m*o-1];

    while (!isStackEmpty(&stack)) {
        // Pop a position from the stack
        Node current = pop(&stack,print_array);
        Coord currentPosition = current.position;

        // Check if Mario has reached the princess
        if (currentPosition.x == Princess.x && currentPosition.y == Princess.y && currentPosition.z == Princess.z) {
            // Output the steps taken
            printf("Steps to victory: %d\n", current.steps);

            // Print the moves
            printf("Moves:\n");
            print_moves(print_array, current.steps);

            freeStack(&stack);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    free(visited[i][j]);
                }
                free(visited[i]);
            }
            free(visited);
            return true;
        }

        // Try all possible moves and shots
        for (int i = 0; i < 6; i++) {
            Coord nextPosition = {currentPosition.x + dx[i], currentPosition.y + dy[i], currentPosition.z + dz[i]};

            // Check if the move is valid
            if (is_valid(nextPosition.x, nextPosition.y, nextPosition.z) && !visited[nextPosition.x][nextPosition.y][nextPosition.z] &&
                can_move(currentPosition, nextPosition) && (calculate_distance(Princess, currentPosition) > calculate_distance(Princess, nextPosition))) {
                visited[nextPosition.x][nextPosition.y][nextPosition.z] = true;

                // Push the next position along with the path onto the stack
                Node nextNode = {nextPosition, current.steps + 1, stack.top};
                push(&stack, nextNode);
            }
        }

        // Try moving monsters
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < 6; j++) {
                Coord nextMonsterPosition = {Monsters[i].x + dx[j], Monsters[i].y + dy[j], Monsters[i].z + dz[j]};

                // Check if the monster move is valid
                if (is_valid(nextMonsterPosition.x, nextMonsterPosition.y, nextMonsterPosition.z) &&
                    !visited[nextMonsterPosition.x][nextMonsterPosition.y][nextMonsterPosition.z] &&
                    can_move(Monsters[i], nextMonsterPosition)) {
                    // Update the monster position
                    Monsters[i] = nextMonsterPosition;
                    visited[nextMonsterPosition.x][nextMonsterPosition.y][nextMonsterPosition.z] = true;
                }
            }
        }
    }

    // If DFS doesn't find a solution
    printf("No solution found.\n");
    freeStack(&stack);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            free(visited[i][j]);
        }
        free(visited[i]);
    }
    free(visited);

    return false;
}
