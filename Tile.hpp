#ifndef TILE_HPP
#define TILE_HPP 

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "Player.hpp"

#define TOTAL_TILE_TYPES 6
#define TOTAL_ROTATION_TYPES 4
#define TILE_HEIGHT 5
#define TILE_WIDTH 9

enum class TileType {T, L, I, O, U, X};
enum class Rotation {DEG0 = 0, DEG90 = 1, DEG180 = 2, DEG270 = 3};

class Tile
{
  private:
    TileType type_;
    Rotation rotation_;
    std::vector<Player*> players_;
    
  public:
    Tile(TileType type, Rotation rotation);
    virtual ~Tile() = default;

    // Setter und Getter
    TileType getType() { return type_; }
    Rotation getRotation() { return rotation_; }
    std::vector<Player*> getPlayers() { return players_; }
    int getRotationValue();
    std::string getTileTypeString();

    void setRotation(Rotation rotation) { rotation_ = rotation; }

    // Methods
    //---------------------------------------------------------------
    /// Add a player to the current tile
    /// @param player pointer to the player to be added
    //
    void putPlayer(Player* player);

    //---------------------------------------------------------------
    /// Remove a player from the current tile,
    /// @param player pointer to the player to be removed
    //
    void popPlayer(Player* player);

    //---------------------------------------------------------------
    /// Rotate the current tile to left (counter-clockwise) for "count" times
    /// @param count the number of times for current tile to rotate to left
    //
    void rotateLeft(size_t count);

    //---------------------------------------------------------------
    /// Get the whole string that represent the current Tile
    //
    virtual std::string getTileString() = 0;

    //---------------------------------------------------------------
    /// Get a specific line of the string that represent the current Tile
    //
    virtual std::string getLine(size_t row);

    //---------------------------------------------------------------
    /// Check if the current Tile is open to the top or not.
    //
    bool isFreeTop();

    //---------------------------------------------------------------
    /// Check if the current Tile is open to the bottom or not.
    //
    bool isFreeBottom();

    //---------------------------------------------------------------
    /// Check if the current Tile is open to the left or not.
    //
    bool isFreeLeft();

    //---------------------------------------------------------------
    /// Check if the current Tile is open to the right or not.
    //
    bool isFreeRight();


};

#endif