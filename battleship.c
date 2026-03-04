#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

enum ShipTypes {
    CARRIER,
    BATTLESHIP,
    CRUISER,
    SUBMARINE,
    DESTROYER,
    DESTROYED,
    NONE
};

enum HitStates {
    HIT,
    MISS, 
    SPOT_HASNTBEEN_TRIED
};

struct Boards {
    enum ShipTypes **shipGrid;
    enum HitStates **hitGrid;
    enum ShipTypes **shipGridSinglePlayer;
    enum HitStates **hitGridSinglePlayer;
    enum ShipTypes **shipGridOpponent;
};

void displayBoard(enum ShipTypes **shipGrid) {
    int rows = 10;
    int columns = 10;
    //Displays the ship board.
    printf("                     SHIP BOARD\n");
    printf("    0    1    2    3    4    5    6    7    8    9  \n");
    for(int i = 0; i < rows; i++) {
        //Prints out the letter for the row.
        char letter = 'A' + i;
        printf("%c  ", letter);
        for (int j = 0; j < columns; j++) {
            //Checks which enum is stored at the grid location and prints it out.
            switch(shipGrid[i][j]) {
                case NONE:
                     printf("  ");
                    break;
                case CARRIER: 
                    printf("CV");
                    break;
                case BATTLESHIP:
                    printf("BB");
                    break;
                case CRUISER:
                    printf("CA");
                    break;
                case SUBMARINE:
                    printf("SS");
                    break;
                case DESTROYER:
                    printf("DD");
                    break;
                case DESTROYED:
                    printf("XX");
                    break;
            }
            printf(" | ");
        }
        printf("\n");
    }
}

//Helper method to reduce long conditional statements for letter validation.
bool isValidLetter(char location[], int i) {
    if((location[i] >= 'a' && location[i] <= 'j') || (location[i] >= 'A' && location[i] <= 'J')) {
        return true;
    }
    return false;
}

//Helper method to reduce long conditional statements for number validation.
bool isValidNumber(char location[], int i) {
    if(location[i] >= '0' && location[i] <= '9') {
        return true;
    }
    return false;
}

void placeShips(enum ShipTypes **shipGrid, enum HitStates **hitGrid) {
    printf("Place your ships. The format for this is: AE4 (a carrier from A4-E4) or J37 (a carrier from J3-J7).\n");
    displayBoard(shipGrid);
    //Main loop, will run once for every ship size.
    for(int i = 5; i > 0; i--) {
        bool isValidInput = false;
        while(!isValidInput) {
            printf("Please enter a location for a ship of %d squares: ", i);
            char location[4];
            fgets(location, 4, stdin);
            int character;
            int extraInputFlag = 0;
            //Flags any extra inputs so inputs like "AE55" are invalidated, and clears the input buffer.
            while ((character = getchar()) != '\n' && character != EOF) { 
                extraInputFlag = 1;
            }
            //Main block for all input validation.
            if(isValidLetter(location, 0) && extraInputFlag == 0) {
                //Switch block to determine which ship is being placed.
                enum ShipTypes type;
                switch(i) {
                case 5:
                    type = CARRIER;
                    break;
                case 4:
                    type = BATTLESHIP;
                    break;
                case 3:
                    type = CRUISER;
                    break;
                case 2:
                    type = SUBMARINE;
                    break;
                case 1: 
                    type = DESTROYER;
                    break;
                }
                //Block that deals with vertical ships.
                if(isValidLetter(location, 1)) {
                    if(isValidNumber(location, 2)) { 
                        char startRowCharacter = toupper(location[0]);
                        char endRowCharacter = toupper(location[1]);
                        int column = location[2] - '0';
                        int startRow = startRowCharacter - 'A';
                        int endRow = endRowCharacter - 'A';
                        //Takes absolute value incase bounds are reversed.
                        int length = abs(endRow-startRow) + 1;
                        if(length == i) {
                            //Reverses bounds if needed.
                            if(startRow > endRow) {
                                int temp = startRow;
                                startRow = endRow;
                                endRow = temp;
                            }
                            bool isGridOccupied = false;
                            for(int j = startRow; j<= endRow; j++) {
                                if(shipGrid[j][column] != NONE) {
                                    isGridOccupied = true;
                                    printf("Ship found at location, please try again.\n");
                                    break;
                                }
                            }
                            //Populates the grid squares.
                            if(!isGridOccupied) {
                                isValidInput = true;
                                for(int j = startRow; j <= endRow; j++) {
                                    shipGrid[j][column] = type;
                                }
                            }
                        }
                        else {
                            printf("Ship is %d in size but should be %d.\n", length, i);
                        }
                     }
                }
                //Block that deals with horizontal ships.
                else if(isValidNumber(location, 1)) {
                    if(isValidNumber(location, 2)) {
                        int row = toupper(location[0]) - 'A';
                        int startingColumn = location[1] - '0';
                        int endingColumn = location[2] - '0';
                        //Reverses the bounds if needed.
                        if(startingColumn > endingColumn) {
                            int temp = startingColumn;
                            startingColumn = endingColumn;
                            endingColumn = temp;
                        }
                        int length = endingColumn - startingColumn + 1;
                        if(length == i) {
                            bool isGridOccupied = false;
                            for(int j = startingColumn; j <= endingColumn; j++) {
                                if(shipGrid[row][j] != NONE) {
                                    isGridOccupied = true;
                                    printf("Ship found at location, please try again.\n");
                                    break;
                                }
                            }
                            //Populates the grid squares. 
                            if(!isGridOccupied) {
                                isValidInput = true;
                                for(int j = startingColumn; j <= endingColumn; j++) {
                                    shipGrid[row][j] = type;
                                }
                            }
                        }
                        else {
                            printf("Ship is %d in size but should be %d.\n", length, i);
                        }
                    }
                }
            }
            else {
                printf("Input is not valid, please try again.\n");
            }
        }
        displayBoard(shipGrid);
    }
    printf("Ship placement complete.\n");
}

void SetupSinglePlayer(enum ShipTypes ***shipGrid, enum HitStates ***hitGrid) {
    int rows = 10;
    int columns = 10;
    *shipGrid = malloc(rows * sizeof(enum ShipTypes*));
    if (*shipGrid == NULL) {
        printf("Memory allocation failed for shipGrid.\n");
        return;
    }
    for (int i = 0; i < rows; i++) {
        (*shipGrid)[i] = malloc(columns * sizeof(enum ShipTypes));
        if ((*shipGrid)[i] == NULL) {
            printf("Memory allocation failed for shipGrid columns.\n");
            for (int k = 0; k < i; k++) {
                free((*shipGrid)[k]);
            }
            free(*shipGrid);
            return;
        }
        for (int j = 0; j < columns; j++) {
            (*shipGrid)[i][j] = NONE;
        }
    }
    *hitGrid = malloc(rows * sizeof(enum HitStates *));
    if (*hitGrid == NULL) {
        printf("Memory allocation failed for hitGrid.\n");
        for (int i = 0; i < rows; i++) 
        {
            free((*shipGrid)[i]);
        }
        free(*shipGrid);
        return;
    }
    for (int i = 0; i < rows; i++) {
        (*hitGrid)[i] = malloc(columns * sizeof(enum HitStates));
        if ((*hitGrid)[i] == NULL) {
            printf("Memory allocation failed for hitGrid columns.\n");
            for (int l = 0; l < rows; l++) {
                free((*shipGrid)[l]);
            }
            for (int k = 0; k < i; k++) 
            {
                free((*hitGrid)[k]);
            }
            free(*shipGrid);
            free(*hitGrid);
            return;
        }
        for (int j = 0; j < columns; j++) {
            (*hitGrid)[i][j] = SPOT_HASNTBEEN_TRIED;
        }
    }

    // Loop for randomly placing ships.
    enum ShipTypes type;
    for (int i = 5; i > 0; i--) {
        switch (i) {
            case 5: type = CARRIER; break;
            case 4: type = BATTLESHIP; break;
            case 3: type = CRUISER; break;
            case 2: type = SUBMARINE; break;
            case 1: type = DESTROYER; break;
        }
        bool placed = false;
        while (!placed) {
            int orientation = rand() % 2; //0 = horizontal, 1 = vertical
            //horizontal
            if (orientation == 0) { 
                int row = rand() % 10;
                //Guarantees that the size will be correct.
                int startingColumn = rand() % (10 - i + 1);
                bool canPlace = true;
                for (int j = startingColumn; j < startingColumn + i; j++) {
                    if ((*shipGrid)[row][j] != NONE) {
                        canPlace = false;
                        break;
                    }
                }
                if (canPlace) {
                    for (int j = startingColumn; j < startingColumn + i; j++) {
                        (*shipGrid)[row][j] = type;
                    }
                    placed = true;
                }
            } 
            //vertical
            else { 
                int column = rand() % 10;
                //Guarantees that size will be correct.
                int startingRow = rand() % (10 - i + 1);
                bool canPlace = true;
                for (int j = startingRow; j < startingRow + i; j++) {
                    if ((*shipGrid)[j][column] != NONE) {
                        canPlace = false;
                        break;
                    }
                }
                if (canPlace) {
                    for (int j = startingRow; j < startingRow + i; j++) {
                        (*shipGrid)[j][column] = type;
                    }
                    placed = true;
                }
            }
        }
    }
}

void SetupTwoPlayer(enum ShipTypes ***shipGrid) {
    //Allocates the grid that will store the opponents grid
    int rows = 10;
    int columns = 10;
    *shipGrid = malloc(rows * sizeof(enum ShipTypes*));
    if (*shipGrid == NULL) {
        printf("Memory allocation failed for shipGrid.\n");
        return;
    }
    for (int i = 0; i < rows; i++) {
        (*shipGrid)[i] = malloc(columns * sizeof(enum ShipTypes));
        if ((*shipGrid)[i] == NULL) {
            printf("Memory allocation failed for shipGrid columns.\n");
            for (int k = 0; k < i; k++) {
                free((*shipGrid)[k]);
            }
            free(*shipGrid);
            return;
        }
        for (int j = 0; j < columns; j++) {
            (*shipGrid)[i][j] = NONE;
        }
    }
}
   
void initialization(enum ShipTypes ***shipGrid, enum HitStates ***hitGrid, enum ShipTypes ***shipGridSinglePlayer, enum HitStates ***hitGridSinglePlayer, enum ShipTypes ***shipGridOpponent, bool singlePlayer) {
    if(singlePlayer) {
        SetupSinglePlayer(shipGridSinglePlayer, hitGridSinglePlayer);
    } else {
        SetupTwoPlayer(shipGridOpponent);
    }
    int rows = 10;
    int columns = 10;
    //Allocates memory for the rows of ship grid.
    *shipGrid = malloc(rows * sizeof(enum ShipTypes*));
    //Malloc failure checking for shipGrid, frees shipGrid.
    if(*shipGrid == NULL) {
        printf("Memory allocation failed for shipGrid.\n");
        return;
    }
    for(int i = 0; i < rows; i++) {
        //Allocates memory for the columns of ship grid.
        (*shipGrid)[i] = malloc(columns * sizeof(enum ShipTypes));
        //Malloc failure checking for shipGrid columns, frees shipGrid.
        if((*shipGrid)[i] == NULL) {
            printf("Memory allocation failed for shipGrid columns.\n");
            for(int k = 0; k < i; k++) {
                free((*shipGrid)[k]); 
            }
            free(*shipGrid);
            return; 
        }
        for(int j = 0; j < columns; j++) {
            //Populates ship grid with NONE.
            (*shipGrid)[i][j] = NONE;
        }
    }
    //Allocates memory for the rows of hit grid.
    *hitGrid = malloc(rows * sizeof(enum HitStates *));
    //Malloc failure checking for hitGrid, frees both shipGrid and hitGrid.
     if(*hitGrid == NULL) {
        printf("Memory allocation failed for hitGrid.\n");
        for(int i = 0; i < rows; i++) {
            free((*shipGrid)[i]);
        }
        free(*shipGrid);
        return;
    }
    for(int i = 0; i < rows; i++) {
        //Allocates memory for the columns of hit grid.
        (*hitGrid)[i] = malloc(columns * sizeof(enum HitStates));
        //Malloc failure checking for hitGrid columns, frees both shipGrid and hitGrid. 
        if((*hitGrid)[i] == NULL) {
            printf("Memory allocation failed for hitGrid columns.\n");
            for(int l = 0; l < rows; l++) {
            free((*shipGrid)[l]);
            }
            for(int k = 0; k < i; k++) {
                free((*hitGrid)[k]);
            }
            free(*hitGrid);
            return;
        }
        for(int j = 0; j < columns; j++) {
            //Populates hit grid with hasn't been tried.
            (*hitGrid)[i][j] = SPOT_HASNTBEEN_TRIED;
        }
    }
    placeShips(*shipGrid, *hitGrid);
}

void TeardownSinglePlayer(enum ShipTypes **shipGridSinglePlayer, enum HitStates **hitGridSinglePlayer, int rows) {
    for(int i = 0; i < rows; i++) {
        free(shipGridSinglePlayer[i]);
        free(hitGridSinglePlayer[i]);
    }
    free(shipGridSinglePlayer);
    free(hitGridSinglePlayer);
}

void TeardownTwoPlayer(enum ShipTypes **shipGrid, int rows) {
    for(int i = 0; i < rows; i++) {
        free(shipGrid[i]);
    }
    free(shipGrid);
}

void teardown(enum ShipTypes **shipGrid, enum HitStates **hitGrid, enum ShipTypes **shipGridSinglePlayer, enum HitStates **hitGridSinglePlayer, enum ShipTypes **shipGridOpponent, int rows, bool singlePlayer) {
    //Frees up the memory. 
    for(int i = 0; i < rows; i++) {
        free(shipGrid[i]);
        free(hitGrid[i]);
    }
    free(shipGrid);
    free(hitGrid);
    if(singlePlayer) {
        TeardownSinglePlayer(shipGridSinglePlayer, hitGridSinglePlayer, rows);
    } else {
        TeardownTwoPlayer(shipGridOpponent, rows);
    }
}

//Validates input and handles quit (Q).
void acceptInput(char* letterAndNumber, int sizeOfString, bool* quitFlag) {
    bool isValidInput = false;
    while (!isValidInput) {
        printf("Select a letter from A-J and a number from 0-9, or press Q to quit: ");
        fgets(letterAndNumber, sizeOfString, stdin);
        if((letterAndNumber[0] == 'q' || letterAndNumber[0] == 'Q') && letterAndNumber[1] == '\n') {
            *quitFlag = true;
            return;
        }
        int character;
        int extraInputFlag = 0;
        //Flags any extra inputs so inputs like "A15" are invalidated, and clears the input buffer.
        while ((character = getchar()) != '\n' && character != EOF) { 
            extraInputFlag = 1;
        }
        if(isValidLetter(letterAndNumber, 0)) {
            if(isValidNumber(letterAndNumber, 1)) {
                isValidInput = true;
            }
        }
        else {
            printf("Input is not valid, please select a letter from A-J and a number from 0-9. \n");
        }
    }
}

int MakeSinglePlayerShot(char *letterAndNumber, enum ShipTypes **shipGridSinglePlayer, enum HitStates **hitGrid) {
    int row = toupper(letterAndNumber[0]) - 'A';
    int column = letterAndNumber[1] - '0';
    if(hitGrid[row][column] != SPOT_HASNTBEEN_TRIED) {
        printf("This spot has already been shot.\n");
        //Used so that I can make sure the user has a valid hit before the computer goes.
        return -1;
    }
    else if(shipGridSinglePlayer[row][column] != NONE){
        return 1;
    }
    else {
        return 0;
    }
}

int MakeTwoPlayerShot(char *letterAndNumber, enum ShipTypes **shipGrid, enum HitStates **hitGrid) {
    int row = toupper(letterAndNumber[0]) - 'A';
    int col = letterAndNumber[1] - '0';
    if(hitGrid[row][col] != SPOT_HASNTBEEN_TRIED) 
    {
        return -1;
    }
    else if (shipGrid[row][col] != NONE) {
        return 1;
    }
    else {
        return 0;
    }
}

void sendShot(int sockfd, char* letterAndNumber) {
    //Sends shot to opponent
    if(send(sockfd, letterAndNumber, 2, 0) <= 0) {
        perror("Send failed");
        exit(1);
    }
}

void receiveShot(int sockfd, char* letterAndNumber) {
    //Receives shot from opponent
    int n = recv(sockfd, letterAndNumber, 2, 0);
    if(n <= 0) {
        perror("Receive failed");
        exit(1);
    }
    letterAndNumber[2] = '\0';
}

bool validateShot(char* letterAndNumber, struct Boards *boards) {
    //Makes sure the spot hasn't already been shot so every move is valid
    int row = toupper(letterAndNumber[0]) - 'A';
    int col  = letterAndNumber[1] - '0';
    if(boards->hitGrid[row][col] != SPOT_HASNTBEEN_TRIED) {
        printf("This shot has already been shot, please try again.\n");
        return true;
    }
    return false;
}

void sendBoard(int sockfd, enum ShipTypes **shipGrid) {
    //Copying the grid temporarily to a fixed array so it can be sent easily
    enum ShipTypes temp[10][10];
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            temp[i][j] = shipGrid[i][j];
        }
    }
    size_t bytes = sizeof(temp);
    if(send(sockfd, temp, bytes, 0) != bytes) {
        perror("SendBoard");
        exit(1);
    }
}

void receiveBoard(int sockfd, enum ShipTypes **shipGrid) {
    enum ShipTypes temp[10][10];
    size_t bytes = sizeof(temp);
    if(recv(sockfd, temp, bytes, 0) != bytes) {
        perror("receiveBoard");
        exit(1);
    }
    //Converting the temp array back to the standard grid
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            shipGrid[i][j] = temp[i][j];
        }
    }
}

bool GetSinglePlayerShot(enum ShipTypes **shipGrid, enum HitStates **hitGridSinglePlayer, int *row, int *column) {
    *row = rand() % 10;
    *column = rand() % 10;
    while(hitGridSinglePlayer[*row][*column] != SPOT_HASNTBEEN_TRIED) {
        *row = rand() % 10;
        *column = rand() % 10;
    }
    if(shipGrid[*row][*column] != NONE) {
        return true;
    }
    else {
        return false;
    }
}

void SinglePlayerResponse(char *letterAndNumber, int *row, int *column, int hitOrMiss, bool hitOrMissSinglePlayer, struct Boards *boards) {
    int playerRow = toupper(letterAndNumber[0]) - 'A';
    int playerColumn = letterAndNumber[1] - '0';
    //Updates the computer grid.
    if(hitOrMiss == 1) {
        printf("Thats a hit!\n");
        boards->shipGridSinglePlayer[playerRow][playerColumn] = DESTROYED;
        boards->hitGrid[playerRow][playerColumn] = HIT;
    } 
    else if(hitOrMiss == 0) {
        printf("Thats a miss!\n");
        boards->hitGrid[playerRow][playerColumn] = MISS;
    }
    //Updates the player grid.
    if(hitOrMissSinglePlayer) {
        printf("One of your ships have been hit!\n");
        boards->shipGrid[*row][*column] = DESTROYED;
        boards->hitGridSinglePlayer[*row][*column] = HIT;
    }
    else {
        printf("Your ships have not been hit!\n");
        boards->hitGridSinglePlayer[*row][*column] = MISS;
    }
}

void TwoPlayerResponse(char *myMove, char *oppMove, struct Boards *boards) {
    int row = toupper(myMove[0]) - 'A';
    int col = myMove[1] - '0';
    if(oppMove[0] != '\0') { 
        printf("Opponents shot: %c%d\n", toupper(oppMove[0]), oppMove[1]-'0');
    }
    //Handles hit sent to the opponent
    if (boards->shipGridOpponent[row][col] != NONE) {
         printf("Hit!\n");
         boards->hitGrid[row][col] = HIT;
         boards->shipGridOpponent[row][col] = DESTROYED;
     } 
     else {
         printf("Miss!\n");
        boards->hitGrid[row][col] = MISS;
    } 
    //Handles hit sent by the opponent
    if(oppMove[0] != '\0') {
        row = toupper(oppMove[0]) - 'A';
        col = oppMove[1] - '0';
        if(boards->shipGrid[row][col] != NONE) {
            printf("One of your ships has been hit!\n");
            boards->shipGrid[row][col] = DESTROYED;
        } else {
            printf("Opponent missed.\n");
        }
    }
}

bool SinglePlayerDidWin(struct Boards *boards) {
    bool isShipPresent = false;
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            if(boards->shipGrid[i][j] != DESTROYED && boards->shipGrid[i][j] != NONE) {
                isShipPresent = true;
                break;
            }
        }
    }
    if(isShipPresent) {
        return false;
    }
    else {
        return true;
    }
}

bool TwoPlayerDidWin(struct Boards *boards) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (boards->shipGridOpponent[i][j] != NONE && boards->shipGridOpponent[i][j] != DESTROYED) {
                    return false;
            }
        }
    }
    return true;
}

//Determines if there is a hit for both the player and the opponent and updates the boards.
void updateState(char* letterAndNumber, struct Boards *boards, bool *someoneWon) {
    //Checks for player win.
    bool isShipPresentSinglePlayer = false;
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            if(boards->shipGridSinglePlayer[i][j] != DESTROYED && boards->shipGridSinglePlayer[i][j] != NONE) {
                isShipPresentSinglePlayer = true;
                break;
            }
        }
    }
    if(isShipPresentSinglePlayer == false) {
        printf("The player has won!\n");
        *someoneWon = true;
        return;
    }
    bool singlePlayerWon = SinglePlayerDidWin(boards);
    if(singlePlayerWon) {
        printf("The computer has won!\n");
        *someoneWon = true;
        return;
    }
    int hitOrMiss = MakeSinglePlayerShot(letterAndNumber, boards->shipGridSinglePlayer, boards->hitGrid);
    //Used to make sure the computer doesn't get a free move when the player shoots at an already hit spot.
    if(hitOrMiss != -1) {
        int row;
        int column;
        bool hitOrMissSinglePlayer = GetSinglePlayerShot(boards->shipGrid, boards->hitGridSinglePlayer, &row, &column);
        SinglePlayerResponse(letterAndNumber, &row, &column, hitOrMiss, hitOrMissSinglePlayer, boards);
    }
}

//Displays hits or misses and the player board.
void displayWorld(enum ShipTypes **shipGrid, enum HitStates **hitGrid) {
    displayBoard(shipGrid);
    int rows = 10;
    int columns = 10;
    //Displays the hit board.
    printf("                     HIT BOARD\n");
    printf("    0    1    2    3    4    5    6    7    8    9  \n");
    for(int i = 0; i < rows; i++) {
        //Prints out the letter for the row.
        char letter = 'A' + i;
        printf("%c  ", letter);
        for (int j = 0; j < columns; j++) {
            //Checks which enum is stored at the grid location and prints it out.
            switch(hitGrid[i][j]) {
                case HIT:
                     printf(" X");
                    break;
                case MISS: 
                    printf(" O");
                    break;
                case SPOT_HASNTBEEN_TRIED:
                    printf("  ");
                    break;
            }
            printf(" | ");
        }
        printf("\n");
    }
}

int serverSetup(char *port) {
     //Gets the address
        int s = -1;
        struct addrinfo hints, *servinfo, *p;
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_INET;
        int status = getaddrinfo(NULL, port, &hints, &servinfo);
        if(status != 0) {
            perror("getaddrinfo failed");
            exit(1);
        }
        for(p = servinfo; p != NULL; p = p->ai_next) {
            int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if(sock == -1) {
                continue;
            }
            if(bind(sock, p->ai_addr, p->ai_addrlen) == 0) {
                s = sock;
                break;
            }
            close(sock);
        }
        freeaddrinfo(servinfo);
        if(s == -1) {
            fprintf(stderr, "Failed to bind\n");
            exit(1);
        }
        int newSocket;
        int l = listen(s, 1);
        if(l < 0) {
            perror("Listen failed");
            exit(1);
        }
        struct sockaddr_storage client_addr;
        socklen_t addr_len = sizeof(client_addr);
        newSocket = accept(s, (struct sockaddr*)&client_addr, &addr_len);
        if(newSocket < 0) {
            perror("Accept failed");
            close(s);
            exit(1);
        }
        close(s);
        return newSocket;
}

int clientSetup(char *serverIP, char *portChar) {
        int port = atoi(portChar);
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if(s < 0) {
            perror("Socket failed");
            exit(1);
        }
        //Sets the address so it can be connected
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if(inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0) {
            perror("Invalid address");
            exit(1);
        }
        int c = connect(s, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if(c < 0) {
            perror("Connect failed");
            close(s);
            exit(1);
        }
        return s;
}

int main(int argc, char *argv[]) {
    srand(time(0)); 
    int sockfd = -1;
    struct Boards boards;
    if(argc == 1) {
    initialization(&boards.shipGrid, &boards.hitGrid, &boards.shipGridSinglePlayer, &boards.hitGridSinglePlayer, NULL, true);
    char letterAndNumber[3];
    while(true) {
        bool quitFlag = false;
        acceptInput(letterAndNumber, sizeof(letterAndNumber), &quitFlag);
        if (quitFlag) {
            break;
        }
        updateState(letterAndNumber, &boards, &quitFlag);
        displayWorld(boards.shipGrid, boards.hitGrid);
        if(quitFlag) {
            break;
        }
    }
    teardown(boards.shipGrid, boards.hitGrid, boards.shipGridSinglePlayer, boards.hitGridSinglePlayer, NULL, 10, true);
    } 
    //Server mode
    else if (argc == 2) {
        sockfd = serverSetup(argv[1]);
        initialization(&boards.shipGrid, &boards.hitGrid, NULL, NULL, &boards.shipGridOpponent, false);
        sendBoard(sockfd, boards.shipGrid);
        receiveBoard(sockfd, boards.shipGridOpponent);
        bool gameOver = false;
        char myMove[3], oppMove[3];
        while(!gameOver) {
            bool invalidMove = true;
            //Validates shot before it gets sent
            while(invalidMove) {
                acceptInput(myMove, sizeof(myMove), &gameOver);
                invalidMove = validateShot(myMove, &boards);
                if(gameOver) {
                    break;
                }
            }      
            sendShot(sockfd, myMove);
            receiveShot(sockfd, oppMove);
            TwoPlayerResponse(myMove, oppMove, &boards);
            displayWorld(boards.shipGrid, boards.hitGrid);
            if(TwoPlayerDidWin(&boards)) {
                printf("You won!\n");
                break;
            }
        }
        teardown(boards.shipGrid, boards.hitGrid, NULL, NULL, boards.shipGridOpponent, 10, false);
        close(sockfd);
    } 
    //Client mode
    else if(argc == 3){
        sockfd = clientSetup(argv[1], argv[2]);
        initialization(&boards.shipGrid, &boards.hitGrid, NULL, NULL, &boards.shipGridOpponent, false);
        receiveBoard(sockfd, boards.shipGridOpponent);
        sendBoard(sockfd, boards.shipGrid);
        bool gameOver = false;
        char myMove[3], oppMove[3];
        while(!gameOver) {
            receiveShot(sockfd, oppMove);
            bool invalidMove = true; 
            //Validates shot before it gets sent
            while(invalidMove) {
                acceptInput(myMove, sizeof(myMove), &gameOver);
                invalidMove = validateShot(myMove, &boards);
                if(gameOver) {
                    break;
                }
            }
            sendShot(sockfd, myMove);
            TwoPlayerResponse(myMove, oppMove, &boards);
            displayWorld(boards.shipGrid, boards.hitGrid);
            if(TwoPlayerDidWin(&boards)) {
                printf("You won!\n");
                break;
            }
        }
        teardown(boards.shipGrid, boards.hitGrid, NULL, NULL, boards.shipGridOpponent, 10, false);
        close(sockfd);
    }
    return 0;
}