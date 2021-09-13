#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include "Treasure.hpp"

class Tile;

struct Coordinate
{
  size_t x_;
  size_t y_;
};

class Player
{
  private:
    std::vector<Treasure*> covered_stack_;
    size_t nr_found_treasures_;
    std::string tag_;
    Coordinate cor_;  // location of player on the Field

  public:
    Player(std::string tag);
    ~Player() = default;

    // Setter and Getter
    void setCoveredStack(std::vector<Treasure*> covered_stack) { covered_stack_ = covered_stack; };
    void setNrFoundTreasure(size_t nr_found_treasures) { nr_found_treasures_ = nr_found_treasures; };
    void setTag(std::string tag) { tag_ = tag; };
    void setLocation (Coordinate cor) { cor_ = cor; }
    std::vector<Treasure*> getCoveredStack() const { return covered_stack_; };
    size_t getNrFoundTreasure() const { return nr_found_treasures_; };
    std::string getTag() const { return tag_; };
    std::string getTagFull() const;
    Coordinate getLocation() const { return cor_; }

    // Methods
    //---------------------------------------------------------------
    /// Compare the tag between 2 players
    /// @return True if equal, else False
    //
    bool operator==(const Player& other) const;
};

#endif
