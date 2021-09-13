#include "Tile.hpp"
#include "Game.hpp"


Tile::Tile(TileType type, Rotation rotation)
  : type_{type}, rotation_{rotation}
  {}

void Tile::rotateLeft(size_t count)
{
  for (size_t _ = 0; _ < count; _++)
  {
    if (rotation_ == Rotation::DEG0)
    { 
      rotation_ = Rotation::DEG90; 
      continue; 
    }
    if (rotation_ == Rotation::DEG90) 
    { 
      rotation_ = Rotation::DEG180; 
      continue; 
    }
    if (rotation_ == Rotation::DEG180) 
    { 
      rotation_ = Rotation::DEG270; 
      continue; 
    }
    if (rotation_ == Rotation::DEG270) 
    { 
      rotation_ = Rotation::DEG0; 
      continue; 
    }
  }
}


int Tile::getRotationValue()
{
  if (rotation_ == Rotation::DEG0) { return 0; }
  if (rotation_ == Rotation::DEG90) { return 90; }
  if (rotation_ == Rotation::DEG180) { return 180; }
  if (rotation_ == Rotation::DEG270) { return 270; }
  return -1;
};
    
std::string Tile::getTileTypeString()
{
  if (type_ == TileType::T) { return "T"; }
  if (type_ == TileType::L) { return "L"; }
  if (type_ == TileType::I) { return "I"; }
  if (type_ == TileType::O) { return "O"; }
  if (type_ == TileType::U) { return "U"; }
  if (type_ == TileType::X) { return "X"; }
  return "";
};



std::string Tile::getLine(size_t row)
{
  // get the line from map
  std::string result = "";
  for (int col = 0; col < TILE_WIDTH; col++)
  {
    result += Game::maps_[(int)type_][(int)rotation_][row][col];
  }
  // If theres player on this tile then add player tag.
  if (row == 3 && (players_.size() > 0))
  {
    std::string tag {"P"};
    std::string color_order [] = {"R", "Y", "G", "B"};
    for (string p_color : color_order)
    {
      for (size_t i = 0; i < players_.size(); i++)
      {
        if (p_color == players_.at(i)->getTag())
        {
          tag += players_.at(i)->getTag();
        }
      }
    }
    result.replace(3 - (players_.size() == 4), tag.length(), tag);
  }
  return result;
}

void Tile::popPlayer(Player* player)
{
  for (size_t i = 0; i < players_.size(); i++)
  {
    if(players_.at(i) == player)
    {
      players_.erase(players_.begin() + i);
    }
  }
}
    
void Tile::putPlayer(Player* player)
{ 
  players_.push_back(player);
}


bool Tile::isFreeTop()
{
  return (Game::maps_[(int)type_][(int)rotation_][0][(TILE_WIDTH - 1) / 2] == " ");
}
bool Tile::isFreeBottom()
{
    return (Game::maps_[(int)type_][(int)rotation_][TILE_HEIGHT - 1][(TILE_WIDTH - 1) / 2] == " ");
}
bool Tile::isFreeLeft()
{
    return (Game::maps_[(int)type_][(int)rotation_][(TILE_HEIGHT - 1) / 2][0] == " ");
}
bool Tile::isFreeRight()
{
    return (Game::maps_[(int)type_][(int)rotation_][(TILE_HEIGHT - 1) / 2][TILE_WIDTH - 1] == " ");
}