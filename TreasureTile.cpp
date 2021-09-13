
#include "TreasureTile.hpp"



TreasureTile::TreasureTile(TileType type, Rotation rotation, Treasure* treasure)
  : Tile{type, rotation}, treasure_{treasure}, collected_{false}
  {}

TreasureTile::TreasureTile(TileType type, Rotation rotation)
  : Tile{type, rotation}, treasure_{nullptr}, collected_{false}
  {}

void TreasureTile::collectTreasure()
{
  treasure_ = nullptr;
  collected_ = true;
}

std::string TreasureTile::getTileString()
{
  std::string str_return {""};
  for (size_t row = 0; row < TILE_HEIGHT; row++)
  {
    str_return += TreasureTile::getLine(row) + "\n";
  }
  return str_return;
}

std::string TreasureTile::getLine(size_t row)
{
  // get the line from map
  std::string result = Tile::getLine(row);
  
  // If the treasure is not collected yet then add treasure's tag
  if (row == 2 && !collected_)
  {
    std::string tag = "T" + treasure_->getTreasureIDString();
    result.replace(3, tag.length(), tag);
  }
  return result;
};

