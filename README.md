# Battleship Game

Battleship is a classic two-player guessing game where each player tries to sink the other player's ships by guessing their coordinates on a grid.

## Features

- Single-player mode against a computer opponent.
- Multiplayer mode where two players can play against each other.
- Customizable grid size and number of ships.
- User-friendly interface for placing ships and making guesses.

## Installation

### Requirements

- C Compiler (e.g., GCC)
- Unix-like operating system (Linux, macOS)

### Steps

1. Clone the repository:

    ```bash
    git clone git@github.com:ibrahimtosun18/battleship_amiralbatti.git
    ```

2. Navigate to the project directory:

    ```bash
    cd battleship
    ```

3. Compile the source code:

    ```bash
    gcc -o battleship_server battleship_server.c
    ```

## Usage

### Starting the Server

1. Run the server:

    ```bash
    ./battleship_server
    ```

2. The server will start listening for connections on port 8080.

### Playing the Game

1. Connect to the server with
   ```bash
   ./battleship_client.c 127.0.0.1
   ```

3. The game will begin, and you will take turns making guesses by entering coordinates (e.g., "3 4").

4. The server will respond with feedback on your guess (hit or miss) and update the grid accordingly.

5. Keep guessing until you sink all the ships or reach the maximum number of guesses.

6. The game will end, and the server will display the game over message.

## Customization

You can customize the game by modifying the following parameters in the source code:

- Grid size (`GRID_SIZE`)
- Maximum number of guesses (`MAX_GUESSES`)
- Number and size of ships

## Credits

This Battleship game is developed by İbrahim TOSUN.

## License

This project is licensed under the [MIT License](LICENSE).
