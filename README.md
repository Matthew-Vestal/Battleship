# Battleship (C)

## A fully functional battleship game on a 10x10 grid with both CPU and multiplayer compatability.

This project was created for ICSI333 - System fundamentals.

This is a classic battleship implementation in C. Linux is required to run the code as it uses linux exclusive headers for the TCP sockets and client/server management. Some of the features include:

* Command-line arguments to choose play type (1 is singleplayer, 2 is server, and 3 is client)
* Input validation and grid display of ships as well as hits and misses
* Functioning game loop for both singleplayer and client/server play
* Dynamic memory allocation for ship grids, and use of enums and structs
