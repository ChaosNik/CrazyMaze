#include "Game.hpp"

#define CLEAR_TERMINAL "\x1b[2J"



Game& Game::getInstance()
{
  static Game instance_;
  return instance_;
}

int Game::startGame(size_t* player_count, std::vector<Player*>* players)
{
  std::cout << "Welcome to the Wild OOP Labyrinth!!!\n";
  std::string input;
  
  // get user input for player count
  while (true)
  {
    std::cout << "Player Count (2-4): ";
    std::getline(std::cin, input);
    if (isInteger(input))
    {
      *player_count = stoi(input);
      if (*player_count == 2 || *player_count == 3 || *player_count == 4)
      {
        break;
      }
    }
    std::cout << "Wrong Input only a Number from 2 to 4 is allowed!\n";
  }
  // Init the players
  std::string player_tag [] = {"R", "Y", "G", "B"};
  for (size_t i = 0; i < *player_count; i++)
  {
    Player* temp = new Player{player_tag[i]};
    if (temp == nullptr)
    {
      return OUT_OF_MEM;
    }
    (*players).push_back(temp);
  }
  return ALL_GOOD;
}

void Game::handOutCard(std::vector<Treasure*> treasures, size_t player_count, Oop::Random& rand,
                      std::vector<Player*> players)
{
  // Split treasures deck into smaller decks evenly and in random order
  std::vector<std::vector<Treasure*>> treasures_by_player;
  for (size_t _ = 0; _ < player_count; _++)
  {
    treasures_by_player.push_back(std::vector<Treasure*>());
  }

  while(treasures.size())
  {
    for (size_t i = 0; i < player_count; ++i)
    {
      size_t random_index = rand.getRandomCard(treasures.size()) - 1;
      treasures_by_player[i].push_back(treasures.at(random_index));
      treasures.erase(treasures.begin() + random_index);
    }
  }
  // Assign each splitted deck to each player
  for (size_t i = 0; i < player_count; ++i)
  {
    players.at(i)->setCoveredStack(treasures_by_player.at(i));
  }
}

int Game::fillMaps()
{
  std::ifstream in("maps.txt");
  if (!in.is_open())
  {
    std::cout << "Error! File \"maps.txt\" is missing\n";
    return ERR_FILE_NOT_FOUND;
  }
  for (int t = 0; t < TOTAL_TILE_TYPES; t++)
  {
    for (int r = 0; r < TOTAL_ROTATION_TYPES; r++)
    {
      std::string temp;
      for (int row = 0; row < TILE_HEIGHT; row++)
      {
        in >> temp;
        for (int col = 0; col < TILE_WIDTH; col++)
        {
          if (temp[col] == '1')
          {
            maps_[t][r][row][col] = "X";  // character "█"         
          }
          else
          {
            maps_[t][r][row][col] = " ";
          }
        }
      }
    }
  }
  in.close();
  return ALL_GOOD;
}

int Game::setupFixedTile(std::vector<Player*> players, size_t p_count, std::vector<Treasure*> treasures)
{
  field_[0][0] = new StartTile(TileType::L, Rotation::DEG270, "R");
  field_[0][2] = new TreasureTile(TileType::T, Rotation::DEG0);
  field_[0][4] = new TreasureTile(TileType::T, Rotation::DEG0);
  field_[0][6] = new StartTile(TileType::L, Rotation::DEG180, "Y");

  field_[2][0] = new TreasureTile(TileType::T, Rotation::DEG90);
  field_[2][2] = new TreasureTile(TileType::T, Rotation::DEG90);
  field_[2][4] = new TreasureTile(TileType::T, Rotation::DEG0);
  field_[2][6] = new TreasureTile(TileType::T, Rotation::DEG270);

  field_[4][0] = new TreasureTile(TileType::T, Rotation::DEG90);
  field_[4][2] = new TreasureTile(TileType::T, Rotation::DEG180);
  field_[4][4] = new TreasureTile(TileType::T, Rotation::DEG270);
  field_[4][6] = new TreasureTile(TileType::T, Rotation::DEG270);

  field_[6][0] = new StartTile(TileType::L, Rotation::DEG0, "G");
  field_[6][2] = new TreasureTile(TileType::T, Rotation::DEG180);
  field_[6][4] = new TreasureTile(TileType::T, Rotation::DEG180);
  field_[6][6] = new StartTile(TileType::L, Rotation::DEG90, "B");

  //Check whether reservation in Heap successful
  for (size_t row = 0; row < FIELD_SIZE; row += 2)
  {
    for (size_t col = 0; col < FIELD_SIZE; col += 2)
    {
      if (field_[row][col] == nullptr)
      {
        return OUT_OF_MEM;
      }
    }
  }

  // add player to StartTile and Treasures to TreasureTiles
  size_t p_ind = 0;  // index for players vector
  size_t t_ind = 0;  // index for treasures vector
  for (size_t row = 0; row < FIELD_SIZE; row += 2)
  {
    for (size_t col = 0; col < FIELD_SIZE; col += 2)
    {
      // If it's coner Tiles (StartTile)
      if (((row + col) % 6 == 0) && ((row * col) % 6 == 0))
      {
        if (p_ind < p_count)  // Only add the players that exist.
        {
          ((StartTile*) field_[row][col])->putPlayer(players.at(p_ind));
          p_ind++;
        }
      }
      // If it's Treasure Tiles
      else
      {
        ((TreasureTile*) field_[row][col])->setTreasure(treasures.at(t_ind));
        t_ind++;
      }
    }
  }
  return ALL_GOOD;
}

int Game::setupMovingTile(Oop::Random& rand, std::vector<Treasure*> treasures, Tile** free_card)
{
  // create list of cards that are still available to be filled onto the field
  std::vector<Tile*> cards_left = std::vector<Tile*>();
  for (int _ = 0; _ < 11; _++)
  {
    cards_left.push_back(new NormalTile(TileType::L, Rotation::DEG0));
  }
  for (int _ = 0; _ < 11; _++)
  {
    cards_left.push_back(new NormalTile(TileType::I, Rotation::DEG0));
  }
  for (int i = 0; i < 12; i++)
  {
    // the index for treasures vector starts from 12 because the first 12 treasures are already in fixed Tiles
    size_t t_ind = i + 12;
    cards_left.push_back(new TreasureTile(TileType::T, Rotation::DEG0, treasures.at(t_ind)));
  }
  // check if reservation was successful
  for (auto card : cards_left)
  {
    if (card == nullptr)
    {
      return OUT_OF_MEM;
    }
  }
  
  // Fill empty tiles of the field with the available cards
  for (size_t row = 0; row < FIELD_SIZE; row++)
  {
    for (size_t col = 0; col < FIELD_SIZE; col++)
    {
      if (field_[row][col] != nullptr)
      {
        continue;
      }
      
      size_t rand_ind = rand.getRandomCard(cards_left.size()) - 1;
      field_[row][col] = cards_left.at(rand_ind);
      field_[row][col]->rotateLeft(rand.getRandomOrientation());
      cards_left.erase(cards_left.begin() + rand_ind);
    }
  }
  // After field fully filled, there's 1 leftover card
  *free_card = cards_left[0];
  return ALL_GOOD;
}

int Game::setupField(std::vector<Player*> players, size_t p_count, std::vector<Treasure*>* treasures, 
                      Oop::Random& rand, Tile** free_card)
{
  // create deck of treasure cards
  size_t id_counter = 1;
  std::string treasure_name_list [] = {"Totenkopf", "Schwert", "Goldsack", "Schlüsselbund", "Sombrero", "Ritterhelm",
                                "Buch", "Krone", "Schatztruhe", "Kerzenleuchte", "Schatzkarte", "Goldring",
                                "Eule", "Hofnarr", "Eidechse", "Käfer", "Flaschengeist", "Kobold", "Schlange",
                                "Geist", "Fledermaus", "Spinne", "Ratte", "Motte"};
  for (std::string treasure : treasure_name_list)
  {
    Treasure* temp = new Treasure(treasure, id_counter++);
    if (temp == nullptr)
    {
      return OUT_OF_MEM;
    }
    (*treasures).push_back(temp);
  }
	
  // First setup fixed Tiles
  size_t err = setupFixedTile(players, p_count, *treasures);
  if (err == OUT_OF_MEM)
  {
    return err;
  }
  //Then fill the rest of the field with random tiles
  err = setupMovingTile(rand, *treasures, free_card);
  if (err == OUT_OF_MEM)
  {
    return err;
  }
  return ALL_GOOD;
}

Game::~Game()
{
  for (size_t row = 0; row < FIELD_SIZE; row++)
  {
    for (size_t col = 0; col < FIELD_SIZE; col++)
    {
      delete field_[row][col];
      field_[row][col] = nullptr;      
    }
  }  
}

void Game::printField(size_t player_count, std::vector<Player*> players)
{
  using std::string;
  using std::cout;
  using std::endl;

  // Total number of treasures to find (for each player)
  string total_covered = std::to_string(24 / player_count);  // 24 for 24 total treasures

  // find score of player 1 and 2
  string p1_found = std::to_string(players.at(0)->getNrFoundTreasure());
  string p1_score = "Treasure: " + p1_found + "/" + total_covered;
  string p2_found = std::to_string(players.at(1)->getNrFoundTreasure());
  string p2_score = "Treasure: " + p2_found + "/" + total_covered;

  string empty_score_field = "                 ";
  string p1_score_full = empty_score_field;
  p1_score_full.replace(0, p1_score.length(), p1_score);

  // Print Header
  cout << "Player Red(R)    |                 |                 |    Player Yellow(Y)" << endl;
  cout << p1_score_full << "V                 V                 V    " << p2_score << endl;
  cout << "        1        2        3        4        5        6        7    " << std::endl;

  // Print Field
  for (size_t field_row = 0; field_row < FIELD_SIZE; field_row++)
  {
    for (size_t tile_row = 0; tile_row < TILE_HEIGHT; tile_row++)
    {
      // Beginning of lines that start with numbers
      if (tile_row % 5 == 2)
      {
        if (field_row % 2 == 0)
        {
          cout << "   ";
        }
        else
        {
          cout << "-->";
        }
        cout << field_row + 1;
      }
      // Beginning of other lines
      else
      {
        cout << "    ";
      }
      // print 1 full line of the field
      for (size_t field_col = 0; field_col < FIELD_SIZE; field_col++)
      {
        string line = field_[field_row][field_col]->getLine(tile_row);
        for (size_t c = 0; c < line.size(); c++)
        {
          if (line.at(c) == 'X')
          {
            cout << "█";
          }
          else
          {
            cout << line.at(c);
          }
        }
      }
      // End of each line of field
      if ((tile_row % 5 == 2) && (field_row % 2 == 1))
      {
        cout << "<--";
      }
      cout << endl;
    }
  }

  // Print Footer
  if (player_count == 2)
  {
    cout << "                 Ʌ                 Ʌ                 Ʌ" << endl;
    cout << "                 |                 |                 |" << endl;
  }
  else if (player_count > 2)
  {
    string p3_found = std::to_string(players.at(2)->getNrFoundTreasure());
    string p3_score = "Treasure: " + p3_found + "/" + total_covered;
    string p3_score_full = empty_score_field;
    p3_score_full.replace(0, p3_score.length(), p3_score);
    if (player_count == 3)
    {
      cout << "Player Green(G)  Ʌ                 Ʌ                 Ʌ" << endl;
      cout << p3_score_full << "|                 |                 |" << endl;
    }
    else if (player_count == 4)
    {
      string p4_found = std::to_string(players.at(3)->getNrFoundTreasure());
      string p4_score = "Treasure: " + p3_found + "/" + total_covered;
      cout << "Player Green(G)  Ʌ                 Ʌ                 Ʌ    Player Blue(B)" << endl;
      cout << p3_score_full << "|                 |                 |    " << p4_score << endl;
    }
  }
}
   

bool Game::isMovePossible(int row_amount, int col_amount, Player active_p)
{
  int p_row = (int) active_p.getLocation().y_;
  int p_col = (int) active_p.getLocation().x_;

  int row_new = p_row + row_amount;
  int col_new = p_col + col_amount;

  // Check if new location is out of the Field
  if (row_new < 0 || row_new > 6 || col_new < 0 || col_new > 6)
  {
    return false;
  }
  // Check if go in a certain direction is blocked by any wall or not
  // Check go down
  if (row_amount > 0)
  {
    for (int i = 0; i < row_amount; i++)
    {
      if (!field_[p_row + i][p_col]->isFreeBottom() || 
          !field_[p_row + i +1][p_col]->isFreeTop())
      {
        return false;
      }
    }
  }      
  // Check go up
  else if (row_amount < 0)
  {
    for (int i = 0; i > row_amount; i--)
    {
      if (!field_[p_row + i][p_col]->isFreeTop() || 
          !field_[p_row + i - 1][p_col]->isFreeBottom())
      {
        return false;
      }
    }
  }
  // Check go right
  if (col_amount > 0)
  {
    for (int i = 0; i < col_amount; i++)
    {
      if (!field_[p_row][p_col + i]->isFreeRight() || 
          !field_[p_row][p_col + i + 1]->isFreeLeft())
      {
        return false;
      }
    }
  }
  // Check go left
  else if (col_amount < 0)
  {
    for (int i = 0; i > col_amount; i--)
    {
      if (!field_[p_row][p_col + i]->isFreeLeft() || 
          !field_[p_row][p_col + i - 1]->isFreeRight())
      {
        return false;
      }
    }
  }
  // If no wall is found then path is clear
  return true;
}


int Game::executeCommand(Player* active_p, std::vector <string> commands, Game& game, Tile** free_card, 
										bool* gamefield_on, size_t p_count, std::vector<Player*> players, bool* show_treasure)
{
	if (commands.at(0) == "showtreasure" || commands.at(0) == "st")
	{
		*show_treasure = true;
    game.printField(p_count, players);
		game.showTreasureCMD(active_p);
	}
	else if (commands.at(0) == "hidetreasure" || commands.at(0) == "ht")
	{
		*show_treasure = false;
		game.hideTreasureCMD();
		game.printField(p_count, players);
	}
	else if (commands.at(0) == "showfreetile" || commands.at(0) == "sft")
	{
		game.showFreeTileCMD(*free_card);
	}
	
	string go_cmds [] = {"w", "a", "s", "d", ARROW_DOWN, ARROW_LEFT, ARROW_UP, ARROW_RIGHT, "go"};
	for (string temp : go_cmds)
	{
		if (commands.at(0) == temp)
		{
			game.goCMD(commands, active_p);
			break;
		}
	}
	if (commands.at(0) == "quit" || commands.at(0) == "exit")
	{
		return EXIT_GAME;
	}
	else if (commands.at(0) == "insert" || commands.at(0) == "i")
	{
		game.insertCMD(commands, free_card);
		game.printField(p_count, players);
		if (*show_treasure)
		{
			game.showTreasureCMD(active_p);
		}
	}
	else if (commands.at(0) == "rotate")
	{
		game.rotateCMD(commands, *free_card);
	}
	else if (commands.at(0) == "gamefield" || commands.at(0) == "g")
	{
		game.gamefieldCMD(commands, gamefield_on, game, p_count, players);
	}
	else if (commands.at(0) == "finish" || commands.at(0) == "f")
	{
		int err = game.finishCMD(active_p);
    if (err == NEXT_LOOP)
    {
      game.printField(p_count, players);
    }
		if (err == EXIT_GAME || err == NEXT_LOOP)
		{
			return err;
		}
	}

  // Commands that may require to print out the field and hidden treasure
	std::string cmds [] = {"w", "a", "s", "d", ARROW_DOWN, ARROW_LEFT, ARROW_UP, ARROW_RIGHT, "go", 
												 "finish", "f"};
	for (std::string cmd : cmds)
	{
		if (commands.at(0) == cmd)
		{
			if (*gamefield_on)
			{
				game.printField(p_count, players);
			}
			if (*show_treasure)
			{
				game.showTreasureCMD(active_p);
			}
		}
	}

	return ALL_GOOD;
}

void Game::showTreasureCMD(Player* active_p)
{
  std::vector <Treasure*> stack = active_p->getCoveredStack();
  if (stack.size() == 0)
  {
    std::cout << "All Treasures found, return to your startfield to win!\n";
  }
  else
  {
    Treasure* treasure = stack.back();
    std::cout << "Current Treasure: " << treasure->getName() << " Nr.: " << 
                  treasure->getTreasureIDString() << std::endl;
  }
}

void Game::hideTreasureCMD()
{
  std::cout << CLEAR_TERMINAL;
}

void Game::showFreeTileCMD(Tile* free_card)
{
  std::cout << "Free tile:\n";
  string line = free_card->getTileString();
  for (size_t i = 0; i < line.length(); ++i)
  {
    if (line.at(i) == 'X')
    {
      std::cout << "█";
    }
    else
    {
      std::cout << line.at(i);
    }
  }
}

void Game::goCMD(std::vector <string> commands, Player* active_p)
{
  size_t amount = 1;  // default move range is 1 Tile at a time
  if (commands.at(0) == "go" && commands.size() == 3)
  {
    amount = stoi(commands.at(2));
  }
  size_t old_col = active_p->getLocation().x_;
  size_t old_row = active_p->getLocation().y_;
  size_t new_col = 0;
  size_t new_row = 0;

  // Going left
  if (commands.at(0) == "a" || commands.at(0) == ARROW_LEFT || 
      (commands.at(0) == "go" && commands.at(1) == "left"))
  {
    new_col = old_col - amount;
    new_row = old_row;
  }
  // Going right
  else if (commands.at(0) == "d" || commands.at(0) == ARROW_RIGHT || 
      (commands.at(0) == "go" && commands.at(1) == "right"))
  {
    new_col = old_col + amount;
    new_row = old_row;
  }
  // Going up
  else if (commands.at(0) == "w" || commands.at(0) == ARROW_UP || 
      (commands.at(0) == "go" && commands.at(1) == "up"))
  {
    new_col = old_col;
    new_row = old_row - amount;
  }
  // Going down
  else if (commands.at(0) == "s" || commands.at(0) == ARROW_DOWN || 
      (commands.at(0) == "go" && commands.at(1) == "down"))
  {
    new_col = old_col;
    new_row = old_row + amount;
  }

  Coordinate new_coor = {new_col, new_row};
  field_[old_row][old_col]->popPlayer(active_p);
  field_[new_row][new_col]->putPlayer(active_p);
  active_p->setLocation(new_coor);
}

void Game::rotateCMD(std::vector <string> commands, Tile* free_card)
{
  if (commands.at(1) == "l" || commands.at(1) == "left")
  {
    free_card->rotateLeft(1);
  }
  else
  {
    free_card->rotateLeft(3);
  }
}

void Game::insertCMD(std::vector <string> commands, Tile** free_card)
{
  if (commands.at(1) == "t" || commands.at(1) == "top")
  {
    size_t col = stoi(commands.at(2)) - 1;
    Tile* temp = field_[FIELD_SIZE - 1][col];
    for (size_t row = (FIELD_SIZE - 1); row > 0; row--)  // go from bottom row to top row
    {
      field_[row][col] = field_[row - 1][col];    // asign values from higher rows to lower rows
    }
    field_[0][col] = *free_card;
    *free_card = temp;
  }
  else if (commands.at(1) == "b" || commands.at(1) == "bottom")
  {
    size_t col = stoi(commands.at(2)) - 1;
    Tile* temp = field_[0][col];
    for (size_t row = 0; row < (FIELD_SIZE - 1); row++)   // go from top row to bottom row
    {
      field_[row][col] = field_[row + 1][col];    // asign values from lower rows to higher rows
    }
    field_[FIELD_SIZE - 1][col] = *free_card;
    *free_card = temp;
  }
  else if (commands.at(1) == "r" || commands.at(1) == "right")
  {
    size_t row = stoi(commands.at(2)) - 1;
    Tile* temp = field_[row][0];
    for (size_t col = 0; col < (FIELD_SIZE - 1); col++)   // go from left col to right col
    {
      field_[row][col] = field_[row][col + 1];    // asign values from right cols to left cols
    }
    field_[row][FIELD_SIZE - 1] = *free_card;
    *free_card = temp;
  }
  else if (commands.at(1) == "l" || commands.at(1) == "left")
  {
    size_t row = stoi(commands.at(2)) - 1;
    Tile* temp = field_[row][FIELD_SIZE - 1];
    for (size_t col = (FIELD_SIZE - 1); col > 0; col--)  // go from right col to left col
    {
      field_[row][col] = field_[row][col - 1];    // asign values from left cols to right cols
    }
    field_[row][0] = *free_card;
    *free_card = temp;
  }

  // update player position on Field
  for (size_t row = 0; row < FIELD_SIZE; row++)
  {
    for (size_t col = 0; col < FIELD_SIZE; col++)
    {
      std::vector <Player*> players = field_[row][col]->getPlayers();
      if (players.size() > 0)
      {
        for (Player* player : players)
        {
          player->setLocation(Coordinate {col, row});
        }
      }
    }
  }
}

void Game::gamefieldCMD(std::vector <string> commands, bool* gamefield_on, Game& game, size_t p_count
                        , std::vector<Player*> players)
{
  if (commands.size() == 1)
  {
    game.printField(p_count, players);
  }
  else if (commands.at(1) == "on")
  {
    *gamefield_on = true;
  }
  else
  {
    *gamefield_on = false;
  }
}

int Game::finishCMD(Player* active_p)
{
  // If player ends turn on the correct treasure tile then collect that treasure.
  size_t p_row = active_p->getLocation().y_;
  size_t p_col = active_p->getLocation().x_;
  TreasureTile* tile = dynamic_cast<TreasureTile*>(field_[p_row][p_col]);
  if (tile != nullptr)
  {
    if ((active_p->getCoveredStack().size() > 0) && !tile->getCollected())
    {
      std::vector<Treasure*> p_stack = active_p->getCoveredStack();
      if (tile->getTreasure() == p_stack.back())
      {
        p_stack.pop_back();

        tile->collectTreasure();
        active_p->setCoveredStack(p_stack);
        active_p->setNrFoundTreasure(active_p->getNrFoundTreasure() + 1);
      }
    }
  }

  // If player found all treasure and on the start tile, then game finishes.
  if (active_p->getCoveredStack().size() == 0)
  {
    StartTile* tile = dynamic_cast<StartTile*>(field_[p_row][p_col]);
    if (tile != nullptr)
    {
      if (tile->getPlayerColor() == active_p->getTag())
      {
        std::string tag;
        if (active_p->getTag() == "R") { tag = "RED"; }
        else if (active_p->getTag() == "Y") { tag = "YELLOW"; }
        else if (active_p->getTag() == "G") { tag = "GREEN"; }
        else if (active_p->getTag() == "B") { tag = "BLUE"; }
        std::cout << "The Player " << tag << " has won the game!\n";
        return EXIT_GAME;
      }
    }
  }

  return NEXT_LOOP;
}
