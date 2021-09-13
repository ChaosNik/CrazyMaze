#include "Player.hpp"



Player::Player(std::string tag)
  : nr_found_treasures_{0}, tag_{tag}
  {
    if (tag == "R")
    {
      cor_.x_ = 0;
      cor_.y_ = 0;
    }
    else if (tag == "Y")
    {
      cor_.x_ = 6;
      cor_.y_ = 0;
    }
    else if (tag == "G")
    {
      cor_.x_ = 0;
      cor_.y_ = 6;
    }
    else if (tag == "B")
    {
      cor_.x_ = 6;
      cor_.y_ = 6;
    }
  }

bool Player::operator==(const Player& other) const
{
  if (tag_ == other.tag_)
  {
    return true;
  }
  return false;
}

std::string Player::getTagFull() const
{
  if (tag_ == "R")
  {
    return "RED";
  }
  if (tag_ == "Y")
  {
    return "YELLOW";
  }
  if (tag_ == "B")
  {
    return "BLUE";
  }
  if (tag_ == "G")
  {
    return "GREEN";
  }
  return "";
}
