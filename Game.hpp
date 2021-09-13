#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <string>

#include "Tile.hpp"
#include "StartTile.hpp"
#include "NormalTile.hpp"
#include "TreasureTile.hpp"
#include "Random.hpp"
#include "Player.hpp"
#include "Treasure.hpp"


#define ALL_GOOD -1
#define NOT_GOOD -2		// When something unexpected happens. (Which means this error should never happen)
#define NEXT_LOOP -3   // signal for a new loop
#define ERR_FILE_NOT_FOUND -4
#define EXIT_GAME -5

#define OUT_OF_MEM 1

#define ARROW_UP "\x1b[A"
#define ARROW_DOWN "\x1b[B"
#define ARROW_LEFT "\x1b[D"
#define ARROW_RIGHT "\x1b[C"

#define FIELD_SIZE 7



bool isInteger(std::string line);

class Game
{
  private:
    Game() {};
    Game(const Game& copy) = delete;
    Game& operator= (const Game& other) = delete;

    inline static std::string maps_[TOTAL_TILE_TYPES][TOTAL_ROTATION_TYPES][TILE_HEIGHT][TILE_WIDTH] = {};
    Tile* field_[FIELD_SIZE][FIELD_SIZE];
    friend Tile;

  public:
    static Game& getInstance();
    // Destructor deletes every cards on the field ("free_card" not included)
    ~Game();

    // Methods
    //---------------------------------------------------------------
    /// Print the Welcome mss, Get number of player from user input, initialize player instances
    /// @param player_count Pointer to number of total players
    /// @param players Pointer to vector that contains pointer to all Player instances
    /// @return 1 if out of memory, -1 if ALL GOOD
    //
    int startGame(size_t* player_count, std::vector<Player*>* players);

    //---------------------------------------------------------------
    /// Hand the treasure deck out to every players evenly and randomly
    /// @param player_count Number of total players
    /// @param treasures Pointer to deck of treasures
    /// @param rand The instance of class Random
    /// @param players Vector that contains pointer to all Player instances
    //
    void handOutCard(std::vector<Treasure*> treasures, size_t player_count, Oop::Random& rand, 
                    std::vector<Player*> players);

    //---------------------------------------------------------------
    /// Create a map of all possible representation of a Tile. The map stores all 6 Tile types in all 4 possible Rotation,
    /// so 24 total representations, which are stored in file "maps.txt". The map is a 4D array, 1. argument is type
    /// of the tile, 2. is the rotation of that tile and 3. and 4. is a 2D array of Ascii Letters that draw out the tile
    /// The 2D Ascii characters cosists of "█" and " ".
    /// @return -1 if file failed to be opened, 0 if file opened successfully
    //
    int fillMaps();

    //---------------------------------------------------------------
    /// Print the whole game interface
    /// @param player_count numbers of active players
    /// @param players vector of pointers of all players
    void printField(size_t player_count, std::vector<Player*> players);

    //---------------------------------------------------------------
    /// Create the deck of Treasures and fill the whole field with StartTile, NormalTile and TreasureTile
    /// @param players vector of pointers of all players
    /// @param p_count numbers of active players
    /// @param treasures Pointer to vector of all treasures
    /// @param rand The instance of class Random
    /// @param free_card Pointer of the variable that holds the address of the extra card that is not on the field
    /// @return 1 if Out of memory, -1 if all good
    int setupField(std::vector<Player*> players, size_t p_count, std::vector<Treasure*>* treasures, 
                    Oop::Random& rand, Tile** free_card);

  // Helper Methods for setting up the game
  private:
    //---------------------------------------------------------------
    /// Fill fixed tiles with TreasureTile and StartTile in fixed order and rotation. Then put players on their 
    /// StartTile and fill all TreasureTiles with treasures.
    /// @param players vector of pointers of all players
    /// @param p_count numbers of active players
    /// @param treasures Vector of all treasures
    /// @return -1 if all good, 1 if out of memory
    //
    int setupFixedTile(std::vector<Player*> players, size_t p_count, std::vector<Treasure*> treasures);

    //---------------------------------------------------------------
    /// Fill the remaining empty Tiles on the field with NormalTile and TreasureTile in random order and rotation.
    /// The 1 leftover card will be saved in Var "free_card".
    /// @param players vector of pointers of all players
    /// @param p_count numbers of active players
    /// @param treasures Vector of all treasures
    /// @return -1 if all good, 1 if out of memory
    //
    int setupMovingTile(Oop::Random& rand, std::vector<Treasure*> treasures, Tile** free_card);

  public:
    //---------------------------------------------------------------
    /// Check whether the given "go" cmd is possible or not (check if path is blocked by any wall)
    /// @param col_amount the amount to move left/right (<0 is go left, >0 is go right)
    /// @param rol_amount the amount to move up/down (<0 is go up, >0 is go down)
    /// @param active_p the active player that called the "go" command
    //
    bool isMovePossible(int row_amount, int col_amount, Player active_p);


  // Commands Execution
    //---------------------------------------------------------------
    /// Execute 1 command that given by user, and print out the field/hidden treasure 
    /// under certain circumstances.
    /// @param commands The vector of strings that stores the user command
    /// @param free_card The pointer to pointer of free card
    /// @param game The instance of the class Game
    /// @param active_p The active player
    /// @param gamefield_on Boolean that tells whether to print the field out or not
    /// @param p_count The number of players
    /// @param players The vector that stores address of all player's instances
    /// @return -5 if game ends, -3 if its next player's turn, -1 if command executed.
    //
    int executeCommand(Player* active_p, std::vector <string> commands, Game& game, Tile** free_card, 
										bool* gamefield_on, size_t p_count, std::vector<Player*> players, bool* show_treasure);
  
  // Helper Method for Execute Command
  private:
    //---------------------------------------------------------------
    /// Show the next treasure player has to find
    /// @param active_p The active player.
    //
    void showTreasureCMD(Player* active_p);

    //---------------------------------------------------------------
    /// Clear the terminal so next player wont see the hidden treasure of the current player
    /// @param active_p The active player.
    //
    void hideTreasureCMD();

    //---------------------------------------------------------------
    /// print out the free tile
    /// @param free_card the free tile.
    //
    void showFreeTileCMD(Tile* free_card);

    //---------------------------------------------------------------
    /// Put player to the s location and update the player location on field
    /// @param active_p The active player.
    /// @param commands The vector that stores the command from player
    //
    void goCMD(std::vector <string> commands, Player* active_p);

    //---------------------------------------------------------------
    /// rotate the free tile
    /// @param free_card The free tile.
    /// @param commands The vector that stores the command from player
    //
    void rotateCMD(std::vector <string> commands, Tile* free_card);

    //---------------------------------------------------------------
    /// Insert the free card to a position on the field by shifting the whole row/col.
    ///  The leftover card will now be the free card. After insertion update the location of all
    ///  players on the field.
    /// @param free_card The pointer to the free tile.
    /// @param commands The vector that stores the command from player
    //
    void insertCMD(std::vector <string> commands, Tile** free_card);

    //---------------------------------------------------------------
    /// Print out the game field OR set the boolean gamefield to "on" or "off"
    /// @param gamefield_on Pointer to the boolean that decides whether to print the field or not.
    /// @param commands The vector that stores the command from player
    //
    void gamefieldCMD(std::vector <string> commands, bool* gamefield_on, Game& game, size_t p_count
                      , std::vector<Player*> players);

    //---------------------------------------------------------------
    /// Ends current player's turn. If player is on correct treasure tile then collect that treasure. 
    /// If player has found all treasure and on his Start position then he has won and the game finished.
    /// @param active_p The active player
    /// @return -5 if player has won, -3 if it is next player's turn.
    //
    int finishCMD(Player* active_p);

};

#endif