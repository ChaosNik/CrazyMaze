#ifndef STARTTILE_HPP
#define STARTTILE_HPP

#include "Tile.hpp"
#include "Player.hpp"
#include <string>


class StartTile : public Tile
{
  private:
    std::string player_color_;
  
  public:
    StartTile (TileType type, Rotation rotation);
    StartTile (TileType type, Rotation rotation, std::string color);

    // Getter and Setter
    std::string getPlayerColor() { return player_color_; };
    void setPlayerColor(std::string color) { player_color_ = color; };


    //---------------------------------------------------------------
    /// Get the whole string that represent the current Tile
    //
    std::string getTileString() override;

    //---------------------------------------------------------------
    /// Get a specific line of the string that represent the current Tile
    //
    std::string getLine(size_t row) override;

};


#endif