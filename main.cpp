/* Crazy Labyrinth Game:
- 2-4 players, find all the treasures and return to start point to win
- Commands:
    - showtreasure/st: show the active player the treasure he needs to find
		- hidetreasure/ht: hide the treasure so other players cant know it.
		- showfreetile/sft: show the free card that can be used to push into the play field
		- w/"ARROW_UP"/go up: move the player on the field up 1 tile
		- a/"ARROW_LEFT"/go left: move to left
		- s / "ARROW_DOWN" / go down: move down
		- d / "ARROW_RIGHT" / go right: move right
		(NOTE: for commands "go", we can add the number of tiles to move (zb: go down 2))
		- quit / exit / "EOF": end the game
		- l/left: rotate the free card to left 90°
		- r/right: rotate the free card to right
		- insert t/top 2: insert free card to top row 2. column
		- insert b/bottom x: insert bottom row at x. column
		- insert l/left x: insert first column at x. row.
		- insert r/right x: insert last column at x. row.
		- gamefield/g (on/off): print the gamefield.
		- finish/f: end player turn (or end game if all cards are found)

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Random.hpp"
#include "Game.hpp"

#define ERR_EOF 0  	// End of File
#define ERR_SIZE 3		// Arguments are given by user despite command does not require one.
#define ERR_SIZE_2 4  // Command requires argument but too many/few were given.
#define ERR_INVALID 5  // Invalid 1st argument in user command
#define ERR_INVALID_2 6  // Invalid 2nd argument in user command
#define ERR_INVALID_3 7  // Not allowed insert command
#define ERR_INVALID_4 8  // Player cant go before inserting the free card
#define ERR_IMPOSSIBLE 9  // When player gives an impossible "go" command
#define ERR_POSITION 10		// When player insert card on the wrong position

using std::string;

//---------------------------------------------------------------
/// Check if a string only consisted of numbers
/// @param line The string to be checked
/// @return True if the string are only numbers, else False
//
bool isInteger(std::string line);

//---------------------------------------------------------------
/// Check if the first word of user given command is a known command
/// @param commands The vector of strings that stores the user command
/// @return -1 if ALL_GOOD, 5 if invalid command
//
int isCommandValid(std::vector <string> commands);

//---------------------------------------------------------------
/// Print out the Error message according to the error in command from user.
/// @param commands The vector of strings that stores the user command
/// @return -1 if ALL_GOOD, -3 if message printed and call for next loop
//
int errorHandling(int err, std::vector <string>* commands);

//---------------------------------------------------------------
/// Forbid "insert" cmd that undo the last "insert" from last player. 
/// Forbid player to "go" before "insert".
/// @param commands The vector of strings that stores the user command
/// @param p_can_go Can the player go or not
/// @param last_insert The last insert cmd from last player
/// @return -1 if ALL_GOOD, 7 if forbidden insert, 8 if forbidden go, 
//
int isCommandAllowed(std::vector <string> commands, bool p_can_go, bool p_can_insert, 
										std::vector <string> last_insert);

//---------------------------------------------------------------
/// Check if CMD has the right size and valid argument
/// @param commands The vector of strings that stores the user command
/// @return -1 if ALL_GOOD, 3/4 if wrong size, 6 if invalid, 10 if wrong insert position
//
int isArgumentValid (std::vector <string> commands);

//---------------------------------------------------------------
/// Check if "go" cmd is possible (blocked by wall or not, or out of field range). 
/// Check if "insert" cmd is at the right position or not
/// @param commands The vector of strings that stores the user command
/// @param p_can_go Can the player go or not
/// @param last_insert The last insert cmd from last player
/// @return -1 if ALL_GOOD, 7 if forbidden insert, 8 if forbidden go, 
//
int isCommandPossible (std::vector <string> commands, Player active_p, Game& game);

//---------------------------------------------------------------
/// Get CMD input from user, keep asking until a valid and possible cmd is given. 
/// Check for all possible error in input in order: Command is valid, Command is allowed, 
/// Arguments of cmd is valid, command is possible
/// @param commands The vector of strings that stores the user command
/// @param p_can_go Can the player go or not
/// @param game The instance of the class Game
/// @param active_p The active player that should give cmd.
/// @return -1 if ALL_GOOD, 0 if EOF 
//
int getCommandInput(Player active_p, std::vector <string>* commands, bool* p_can_go, Game& game
										, bool* p_can_insert);



// MAIN
int main(int argc, char* argv[])
{
	if (argc != 1)
	{
		argv = nullptr;
		std::cout << "Wrong arguments!\n";
		return 2;
	}
	
	// Start Game
	Game& game = Game::getInstance();
	size_t player_count = 0;
	std::vector<Player*> players;
	int err = game.startGame(&player_count, &players);
	if (err == OUT_OF_MEM)
	{
		return err;
	}
	

	// Belegen des Spielplan
	std::vector<Treasure*> treasures;
	Oop::Random& rand = Oop::Random::getInstance();
	Tile* free_card;
	err = game.setupField(players, player_count, &treasures, rand, &free_card);
	if (err == OUT_OF_MEM)
	{
		return err;
	}
	// Verteilen der Schatzkarten
	game.handOutCard(treasures, player_count, rand, players);

	// print the field
	game.fillMaps();
	game.printField(player_count, players);

	// the game begins
	bool game_on = true;
	int return_value;
	bool gamefield_on = true;
	while (game_on)
	{
		for (size_t i = 0; i < player_count; i++)
		{
			if (game_on == false)
			{
				break;
			}
			
			Player* active_p = players.at(i);
			bool p_can_go = false;  	// the player can go only when he has inserted the free card
			bool p_can_insert = true;  // Player can only insert once per round
			bool show_treasure = false;
			while (true)
			{
				// Ask for command and check for error in given command
				std::vector <string> commands;
				err = getCommandInput(*active_p, &commands, &p_can_go, game, &p_can_insert);
				if (err == ERR_EOF)
				{
					return_value = ERR_EOF;
					game_on = false;
					break;
				}
				// Execute the command
				err = game.executeCommand(active_p, commands, game, &free_card, &gamefield_on, player_count, 
																	players, &show_treasure);
				if (err == EXIT_GAME)  // A Player has won
				{
					return_value = 0;
					game_on = false;
					break;
				}
				else if (err == NEXT_LOOP)  // Next Player's turn
				{
					break;
				}
			}
		}
	}

	// Finish up
	delete free_card;
	for (Player* player : players)
	{
		delete player;
	}
	for (Treasure* treasure : treasures)
	{
		delete treasure;
	}
	return return_value;
}



// FUNCTION IMPLEMENTATION
bool isInteger(std::string line)
{
	if (line.length() == 0)
	{
		return false;
	}
	for (size_t i = 0; i < line.size(); ++i)
	{
		if (line.at(i) < '0' || line.at(i) > '9')
		{
			return false;
		}
	}
	return true;
}

int errorHandling(int err, std::vector <string>* commands)
{
  switch (err)
  {
    case ALL_GOOD:
      return ALL_GOOD;
    case ERR_SIZE:
      std::cout << "This command does not take any arguments!\n";
      break;
    case ERR_SIZE_2:
      std::cout << "Wrong number of arguments!\n";
      break;
    case ERR_INVALID:
      std::cout << "Invalid command: \"" << (*commands).at(0) << "\"" << std::endl;
      break;
    case ERR_INVALID_2:
      std::cout << "Invalid parameter: \"" << (*commands).at(1) << "\"" << std::endl;
      break;
		case ERR_INVALID_3:  // Cant "insert" that undo last "insert"
			std::cout << "\"" << (*commands).at(0) << " " << (*commands).at(1) << " " <<
									(*commands).at(2) << "\" is currently not allowed\n";
			break;
		case ERR_INVALID_4:  // Cant "go" before "insert" or cant insert more than once
			if ((*commands).at(0) == ARROW_UP) { (*commands).at(0) = "arrow up"; }
			else if ((*commands).at(0) == ARROW_DOWN) { (*commands).at(0) = "arrow down"; }
			else if ((*commands).at(0) == ARROW_LEFT) { (*commands).at(0) = "arrow left"; }
			else if ((*commands).at(0) == ARROW_RIGHT) { (*commands).at(0) = "arrow right"; }
			std::cout << "\"" << (*commands).at(0) << "\" is currently not allowed\n";
			break;
		case ERR_IMPOSSIBLE:
			std::cout << "Impossible move!\n";
			break;
		case ERR_POSITION:
			std::cout << "Invalid Position!\n";
			break;
  }
  (*commands).clear();
  return NEXT_LOOP;
}

int isCommandValid(std::vector <string> commands)
{
	// CMD with 1 argument
	string cmd_first_arg [] = {"showtreasure", "st", "hidetreasure", "ht", "showfreetile", "sft",
															"w", "a", "s", "d", ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, "go",
															"rotate", "insert", "i", "gamefield", "g", "finish", "f", "quit", "exit"};
	for (string temp : cmd_first_arg)
	{
		if (commands.at(0) == temp)
		{
			return ALL_GOOD;		
		}
	}
	
	// If reach here then a valid cmd has not been found, which means its an error
	return ERR_INVALID;
}
    
int isCommandAllowed(std::vector <string> commands, bool p_can_go, bool p_can_insert, 
										std::vector <string> last_insert)
{
	// forbid "un-do" insert
	if ((last_insert.size() == 3) && (last_insert.at(0) == commands.at(0)) && (last_insert.at(2) == commands.at(2)))
	{
		string direction1 [] = {"t", "l", "b", "r"};
		string direction2 [] = {"top", "left", "bottom", "right"};
		for (size_t i = 0, dir_arr_size = 4; i < dir_arr_size; i++)  // 4 for 4 directions
		{
			// check if the new insert command undo the action of last insert command
			if (((last_insert.at(1) == direction1[i]) || (last_insert.at(1) == direction2[i])) &&
				 ((commands.at(1) == direction1[(i + 2) % 4]) || (commands.at(1) == direction2[(i + 2) % 4])))
			{
				return ERR_INVALID_3;
			}
		}
		return ALL_GOOD;
	}
	
	// Forbid "go" if player has not inserted a card first
	string go_cmd [] = {"w", "a", "s", "d", ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, "go"};
	for (string temp : go_cmd)
	{
		if (commands.at(0) == temp)
		{
			if (p_can_go)
			{
				return ALL_GOOD;
			}
			return ERR_INVALID_4;
		}
	}
	// Player can only insert once per round
	if (commands.at(0) == "insert" || commands.at(0) == "i")
	{
		if (p_can_insert)
		{
			return ALL_GOOD;
		}
		return ERR_INVALID_4;
	}
	

	// If command is not forbidden by any of the above rules then command is allowed
	return ALL_GOOD;
}

int isArgumentValid (std::vector <string> commands)
{
	// CMD with 1 argument
	string cmd_single_arg [] = {"showtreasure", "st", "hidetreasure", "ht", "showfreetile", "sft",
															"w", "a", "s", "d", ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT,
														  "finish", "f", "quit", "exit"};
	for (string temp : cmd_single_arg)
	{
		if (commands.at(0) == temp)
		{
			if (commands.size() == 1)
			{
				return ALL_GOOD;
			}
			return ERR_SIZE;		
		}
	}

	// CMD with 2 arguments
	string cmd_arg1 [] = {"l", "left", "r", "right"};
	string cmd_arg2 [] = {"l", "left", "r", "right", "t", "top", "b", "bottom"};
	if (commands.at(0) == "rotate")
	{
		if (commands.size() != 2)
		{
			return ERR_SIZE_2;
		}
		for (string temp : cmd_arg1)
		{
			if (commands.at(1) == temp)
			{
				return ALL_GOOD;
			}
		}
		return ERR_INVALID_2;
	}
	
	else if (commands.at(0) == "insert" || commands.at(0) == "i")
	{
		if (commands.size() != 3)
		{
			return ERR_SIZE_2;
		}
		for (string temp : cmd_arg2)
		{
			if (commands.at(1) == temp)
			{
				if (isInteger(commands.at(2)))
				{
					return ALL_GOOD;
				}
				return ERR_POSITION;
			}
		}
		return ERR_INVALID_2;
	}
	
	else if (commands.at(0) == "gamefield" || commands.at(0) == "g")
	{
		if (commands.size() == 1)
		{
			return ALL_GOOD;
		}
		else if (commands.size() == 2)
		{
			if (commands.at(1) == "on" || commands.at(1) == "off")
			{
				return ALL_GOOD;
			}
			return ERR_INVALID_2;
		}
		return ERR_SIZE_2;
	}

	// CMD with 2 or more arguments
	else if (commands.at(0) == "go")
	{
		if (commands.size() == 2)
		{
			if (commands.at(1) == "up" || commands.at(1) == "down" ||
				  commands.at(1) == "left" || commands.at(1) == "right")
			{
				return ALL_GOOD;
			}
			return ERR_INVALID_2;
		}
		else if (commands.size() == 3)
		{
			if (commands.at(1) == "up" || commands.at(1) == "down" ||
				  commands.at(1) == "left" || commands.at(1) == "right")
			{
				if (isInteger(commands.at(2)))
				{
					return ALL_GOOD;
				}
				std::cout << "Thats not a number\n";
				return ERR_INVALID_2;
			}
			return ERR_INVALID_2;
		}
		return ERR_SIZE_2;
	}

	// if reach here then theres something wrong
	return NOT_GOOD;
}

int isCommandPossible (std::vector <string> commands, Player active_p, Game& game)
{
	// Impossible go move (cant go to another Tile if road is blocked)
	string go_cmd [] = {"w", "a", "s", "d", ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, "go"};
	for (string temp : go_cmd)
	{
		if (commands.at(0) == temp)
		{
			size_t amount = 1;  // default move range is 1 Tile at a time
			if (commands.at(0) == "go" && commands.size() == 3)
			{
				amount = stoi(commands.at(2));
			}
			size_t neg_amount = amount * (-1);

			// Check for going left
			if (commands.at(0) == "a" || commands.at(0) == ARROW_LEFT || 
					(commands.at(0) == "go" && commands.at(1) == "left"))
			{
				if(!(game.isMovePossible(0, neg_amount, active_p)))
				{
					return ERR_IMPOSSIBLE;
				}
				return ALL_GOOD;
			}
			// Check for going right
			else if (commands.at(0) == "d" || commands.at(0) == ARROW_RIGHT || 
					(commands.at(0) == "go" && commands.at(1) == "right"))
			{
				if(!game.isMovePossible(0, amount, active_p))
				{
					return ERR_IMPOSSIBLE;
				}
				return ALL_GOOD;
			}
			// Check for going up
			else if (commands.at(0) == "w" || commands.at(0) == ARROW_UP || 
					(commands.at(0) == "go" && commands.at(1) == "up"))
			{
				if(!game.isMovePossible(neg_amount, 0, active_p))
				{
					return ERR_IMPOSSIBLE;
				}
				return ALL_GOOD;
			}
			// Check for going down
			else if (commands.at(0) == "s" || commands.at(0) == ARROW_DOWN || 
					(commands.at(0) == "go" && commands.at(1) == "down"))
			{
				if(!game.isMovePossible(amount, 0, active_p))
				{
					return ERR_IMPOSSIBLE;
				}
				return ALL_GOOD;
			}
			return NOT_GOOD;  // This should not be reached
		}
	}

	// Invalid insert to the wrong position of field
	if ((commands.at(0) == "insert") || (commands.at(0) == "i"))
	{
		size_t insert = stoi(commands.at(2));
		if ((insert <= 0) || (insert > FIELD_SIZE))
		{
			return ERR_IMPOSSIBLE;
		}
		else if (insert % 2 != 0)
		{
			return ERR_POSITION;
		}
		return ALL_GOOD;
	}
	return ALL_GOOD;
}

int getCommandInput(Player active_p, std::vector <string>* commands, bool* p_can_go, Game& game, 
										bool* p_can_insert)
{
	static std::vector <string> last_insert;
	while (true)
	{
		if (std::cin.eof())		// If EOF
		{
			return ERR_EOF;
		}
		std::cout << active_p.getTagFull() << " > ";
		std::string line;
		getline(std::cin, line);
		
		if (line == "")		// If nothing is entered
		{
			continue;
		}
		// Put user input into string vector
		std::istringstream str_stream {line};
		string word;
		while (str_stream >> word)
		{
			(*commands).push_back(word);
		}

		// Check validity of Command's 1st word
		int err = isCommandValid(*commands);
		int err2 = errorHandling(err, commands);
		if (err2 == NEXT_LOOP)
		{
			continue;
		}
		
		// Check if Command is allowed
		err = isCommandAllowed(*commands, *p_can_go, *p_can_insert, last_insert);
		err2 = errorHandling(err, commands);
		if (err2 == NEXT_LOOP)
		{
			continue;
		}

		// Check if parameters of the command is valid
		err = isArgumentValid(*commands);
		err2 = errorHandling(err, commands);
		if (err2 == NEXT_LOOP)
		{
			continue;
		}

		// Check if the command is possible to be executed
		err = isCommandPossible(*commands, active_p, game);
		err2 = errorHandling(err, commands);
		if (err2 == NEXT_LOOP)
		{
			continue;
		}

		// If the command is "Insert" then player can start to move on the next command
		if ((*commands).at(0) == "i" || (*commands).at(0) == "insert")
		{
			last_insert = (*commands);
			*p_can_go = true;
			*p_can_insert = false;
		}
		return ALL_GOOD;
	}
}

