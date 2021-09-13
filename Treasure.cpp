#include "Treasure.hpp"

Treasure::Treasure (string name, size_t treasure_id)
	: name_{ name }, treasure_id_{ treasure_id }, found_{false}
  {};


bool Treasure::operator==(const Treasure& other) const
{
    return treasure_id_ == other.treasure_id_;
}

string Treasure::getTreasureIDString()
{
  string result {""};
  if (treasure_id_ < 10)
  {
    result += "0";
  }
  result += std::to_string(treasure_id_);
  return result;
}