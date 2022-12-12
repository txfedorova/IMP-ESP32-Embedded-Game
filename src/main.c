#include "game.h"

/// @brief main function of the program
void app_main() {
	// initialize display
	init_display();
	// initialize joystick
	init_joystick();
	// start the game
	start_game();
}