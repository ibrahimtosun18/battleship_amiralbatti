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

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point start;
    Point end;
} Ship;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Initialize the game grid with water ('~')
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '~';
        }
    }
}

// Display the grid to the server console (for debugging)
void displayGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i);
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Simple function to place ships on the grid (needs to be improved for actual gameplay)
void placeShips(char grid[GRID_SIZE][GRID_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < 3; i++) { // Place 3 ships
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        grid[x][y] = 'S';
    }
}

// Check if a guess is a hit or a miss and update the grid
bool processGuess(char grid[GRID_SIZE][GRID_SIZE], int x, int y) {
    if (grid[x][y] == 'S') {
        grid[x][y] = 'H'; // Hit
        return true;
    }
    grid[x][y] = 'M'; // Miss
    return false;
}

int main() {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    char grid[GRID_SIZE][GRID_SIZE];
    initializeGrid(grid);
    placeShips(grid); // Place ships randomly at the start
    displayGrid(grid); // Show the grid in server's terminal for debugging

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
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) error("ERROR reading from socket");

        int x, y;
        sscanf(buffer, "%d %d", &x, &y); // Parse guess coordinates from client
        bool hit = processGuess(grid, x, y);
        sprintf(buffer, hit ? "Hit at %d,%d" : "Miss at %d,%d", x, y);
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) error("ERROR writing to socket");

        displayGrid(grid); // Update the grid in the server's console
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
