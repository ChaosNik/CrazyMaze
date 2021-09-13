#include "StartTile.hpp"



StartTile::StartTile (TileType type, Rotation rotation)
  : Tile{type, rotation}, player_color_{""}
  {}

StartTile::StartTile (TileType type, Rotation rotation, std::string color)
  : Tile{type, rotation}, player_color_{color}
  {}

std::string StartTile::getTileString()
{
  std::string str_return {""};
  for (size_t row = 0; row < TILE_HEIGHT; row++)
  {
    str_return += StartTile::getLine(row) + "\n";
  }
  return str_return;
}

std::string StartTile::getLine(size_t row)
{
  // get the line from map
  std::string result = Tile::getLine(row);
  
  // Add the player color tag if the player exists.
  if (row == 2)
  {
    std::string tag = "(" + player_color_ + ")";
    result.replace(3, tag.length(), tag);
  }
  return result;
}
