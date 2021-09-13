#ifndef TREASURETILE_HPP
#define TREASURETILE_HPP


#include "Tile.hpp"
#include <string>


class TreasureTile : public Tile
{
  private:
    Treasure* treasure_;
    bool collected_;
  
  public:
    TreasureTile(TileType type, Rotation rotation, Treasure* treasure);
    TreasureTile(TileType type, Rotation rotation);

    // Setter and Getter
    void setTreasure (Treasure* treasure) { treasure_ = treasure; };
    void setCollected (bool collected) { collected_ = collected; };
    Treasure* getTreasure () { return treasure_; };
    bool getCollected () { return collected_; };

    // Methods
    //---------------------------------------------------------------
    /// Set the current treasure tile as "collected"
    //
    void collectTreasure();

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
