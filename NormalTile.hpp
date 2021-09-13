#ifndef NORMALTILE_HPP
#define NORMALTILE_HPP


#include "Tile.hpp"
#include <string>


class NormalTile : public Tile
{
  public:
    NormalTile(TileType type, Rotation rotation);


    //---------------------------------------------------------------
    /// Get the whole string that represent the current Tile
    //
    std::string getTileString() override;

};

#endif