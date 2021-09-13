#include "NormalTile.hpp"


NormalTile::NormalTile(TileType type, Rotation rotation)
  : Tile{ type, rotation}
  {};



std::string NormalTile::getTileString()
{
  std::string str_return {""};
  for (size_t row = 0; row < TILE_HEIGHT; row++)
  {
    str_return += Tile::getLine(row) + "\n";
  }
  return str_return;
};