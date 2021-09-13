#ifndef TREASURE_HPP
#define TREASURE_HPP

#include <iostream>
#include <string>

using std::string;

class Treasure
{
  private:
    string name_;
    size_t treasure_id_;
    bool found_;
  
  public:
    Treasure (string name, size_t treasure_id);
    virtual ~Treasure() = default;

    
    string getName() const { return name_; };
    size_t getTreasureID() const { return treasure_id_; };
    bool getFound() const { return found_; };
    string getTreasureIDString();

    void setFound(bool found) { found_ = found; };

    // Method
    //---------------------------------------------------------------
    /// Compare ID of 2 treasures
    /// @return True if equal, else false
    //
    bool operator==(const Treasure& other) const;
};

#endif
