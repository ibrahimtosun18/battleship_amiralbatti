#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <time.h>

#define PORT 8080
#define GRID_SIZE 10
#define BUFFER_SIZE 1024
#define MAX_GUESSES 30

typedef enum { HORIZONTAL, VERTICAL } Orientation;

typedef struct {
    int x;
    int y;
    int hits;  // To track hits on the ship
    int size;
    char *name;  // Name of the ship or commander
    Orientation orientation;
    bool isSunk;
} Ship;

char grid[GRID_SIZE][GRID_SIZE];
Ship ships[3];  // Declare ships globally to track them easily
int totalShips = 3;  // Total number of ships in the game
void error(const char *msg);
void initializeGrid();
void displayGrid(bool showShips);
bool canPlaceShip(int x, int y, int size, Orientation orientation);
void placeShip(int x, int y, int size, Orientation orientation);
void setupShips();
bool allShipsSunk();
bool processGuess(int x, int y, int *guessCount, char *response);
bool isPartOfShip(int x, int y, Ship ship); // Declaration added here to ensure visibility
int main();

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void initializeGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '~';  // Water
        }
    }
}

void displayGrid(bool showShips) {
    printf("  ");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%2d", i);
    }
    printf("\n");

    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < GRID_SIZE; j++) {
            char displayChar = grid[i][j];
            if (!showShips && displayChar == 'S') {
                displayChar = '~';  // Hide ships
            }
            printf("%c ", displayChar);
        }
        printf("\n");
    }
}

bool canPlaceShip(int x, int y, int size, Orientation orientation) {
    if (orientation == HORIZONTAL) {
        for (int i = 0; i < size; i++) {
            if (y + i >= GRID_SIZE || grid[x][y + i] != '~') return false;
        }
    } else {
        for (int i = 0; i < size; i++) {
            if (x + i >= GRID_SIZE || grid[x + i][y] != '~') return false;
        }
    }
    return true;
}

void placeShip(int x, int y, int size, Orientation orientation) {
    for (int i = 0; i < size; i++) {
        if (orientation == HORIZONTAL) {
            grid[x][y + i] = 'S';
        } else {
            grid[x + i][y] = 'S';
        }
    }
}

void setupShips() {
    srand(time(NULL));  // Seed the random number generator
    ships[0] = (Ship){rand() % GRID_SIZE, rand() % GRID_SIZE, 0, 5, "USS Missouri", HORIZONTAL, false};
    ships[1] = (Ship){rand() % GRID_SIZE, rand() % GRID_SIZE, 0, 4, "HMS Ocean", VERTICAL, false};
    ships[2] = (Ship){rand() % GRID_SIZE, rand() % GRID_SIZE, 0, 3, "KMS Bismarck", HORIZONTAL, false};

    for (int i = 0; i < 3; i++) {
        while (!canPlaceShip(ships[i].x, ships[i].y, ships[i].size, ships[i].orientation)) {
            ships[i].x = rand() % GRID_SIZE;
            ships[i].y = rand() % GRID_SIZE;
            ships[i].orientation = rand() % 2 ? HORIZONTAL : VERTICAL;
        }
        placeShip(ships[i].x, ships[i].y, ships[i].size, ships[i].orientation);
    }
}
bool allShipsSunk() {
    for (int i = 0; i < totalShips; i++) { // Update loop condition
        if (!ships[i].isSunk) {
            return false;
        }
    }
    return true;
}

bool processGuess(int x, int y, int *guessCount, char *response) {
    if (grid[x][y] == 'S') {
        grid[x][y] = 'H'; // Hit
        for (int i = 0; i < 3; i++) {
            if (isPartOfShip(x, y, ships[i]))
{
                ships[i].hits++;
                if (ships[i].hits == ships[i].size) {
                    ships[i].isSunk = true;
                    if (response != NULL) {
                        sprintf(response, "%s has been sunk!", ships[i].name);
                    }
                    return true;
                }
            }
        }
        if (response != NULL) {
            strcpy(response, "Hit!");
        }
    } else if (grid[x][y] == '~') {
        grid[x][y] = 'M'; // Miss
        if (response != NULL) {
            strcpy(response, "Miss!");
        }
    }
    (*guessCount)++;
    return false;
}

bool isPartOfShip(int x, int y, Ship ship) {
    if (ship.orientation == HORIZONTAL) {
        return x == ship.x && y >= ship.y && y < ship.y + ship.size;
    } else {
        return y == ship.y && x >= ship.x && x < ship.x + ship.size;
    }
}
int main() {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int guessCount = 0; // Initialize guess count

    initializeGrid();
    setupShips();
    displayGrid(false);  // Initially display grid without showing ships

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    portno = PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    printf("Connected to client\n");

    // Game loop for receiving guesses
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) error("ERROR reading from socket");

        int x, y;
        sscanf(buffer, "%d %d", &x, &y); // Parse guess coordinates from client
        char response[BUFFER_SIZE];
        processGuess(x, y, &guessCount, response);
        displayGrid(false); // Display the updated grid
        // After updating the game state, send the response back to the client
        n = write(newsockfd, response, strlen(response));
        if (n < 0) error("ERROR writing to socket");

        // Check if maximum number of guesses has been reached
        if (guessCount >= MAX_GUESSES || allShipsSunk()) {
            break;
        }
    }

    // Game over message
    if (guessCount >= MAX_GUESSES) {
        printf("Surrender! Maybe try better next time loser haha\n");
    } else {
        printf("We surrender to our new commander!\n");
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}