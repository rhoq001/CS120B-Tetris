#Tetris ###By Raqtan Hoq and Allen Nguyen

Full code for two player tetris game on LED Matrices.

Two Player Battle Tetris with three different powerups and a counter to determine score.
This is implemented around the use of LED Matrices and designing our own circuits. 

-There is a multiplexer going through each row of the LED Matrices. 
-This multiplexer defines each row's LED light configuration based on the Tetris game logic.
-These LED Matrices are controlled by microcontrollers with circuits connecting them on a circuit board. 
-The game is controlled by two analog sticks (one for each player) that are configured to send signals through the circuit board to the LED Matrices.
-The Tetris game include basic Tetris game logic like movement and rotation of pieces as well as switching pieces out. 
-The powerups below are sent through the two synchronized micro-controllers controlling each LED Matrix.
-Game music is implemented by mapping note sound frequencies onto an array for a connected speaker on the board.

Powerup 1: Clearing four rows consecutively sends blocks to the other player, making the matrix cluttered.

Powerup 2: Clearing four rows clears the player's entire screen to ensure survivability.

Powerup 3: Clearing seven rows consecutively at once disables the enemy player's controls.

The game ends once one player reaches the top of the stack. The surviving player wins.
