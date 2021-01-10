//BRANZEU MIHNEA 313CA
#include <stdio.h>
#include <string.h>
#include "functions.h"

#define MAX_LENGTH_COMMAND 100

int main(void)
{
	//Get the commands
	int is_running = 1;
	photo_t photo;
	//Set the photo values
	set_photo(&photo);
	char command[MAX_LENGTH_COMMAND];
	while (is_running == 1) {
		fgets(command, MAX_LENGTH_COMMAND, stdin);
		int is_valid_command = 0;
		int invalid_selectall = 0;
		//Check if command is LOAD aici
		if (strstr(command, "LOAD")) {
			if (strlen(command) > strlen("LOAD") + 2) {
				is_valid_command = 1;
				load_logic(command, &photo);
			} else {
				printf("Invalid command\n");
			}
		}
		//Check if the command is SELECT ALL
		if (strstr(command, "SELECT ALL")) {
			is_valid_command = 1;
			select_all_logic(&photo);

		} else { // Check if the command is SELECT
			if (strstr(command, "SELECT")) {
				is_valid_command = 1;
				//Check if a photo has been loaded
				if (photo.type != -1)
					select_logic(command, &photo, &invalid_selectall);
				else
					printf("No image loaded\n");
			}
		}
		//Check if the command is SAVE
		if (strstr(command, "SAVE")) {
			is_valid_command = 1;
			save_logic(command, &photo);
		}
		//Check if the command is GRAYSCALE
		if (strcmp(command, "GRAYSCALE\n") == 0) {
			is_valid_command = 1;
			grayscale_logic(&photo);
		}
		//Check if the command is SEPIA
		if (strcmp(command, "SEPIA\n") == 0) {
			is_valid_command = 1;
			sepia_logic(&photo);
		}

		//Check if the command is CROP
		if (strcmp(command, "CROP\n") == 0) {
			is_valid_command = 1;
			crop_logic(&photo);
		}
		//Check if the command is EXIT
		if (strstr(command, "EXIT")) {
			is_valid_command = 1;
			exit_logic(&photo);
			//Stop the loop
			is_running = 0;
		}
		//Check if the command is ROTATE
		if (strstr(command, "ROTATE")) {
			is_valid_command = 1;
			rotate_logic(command, &photo);
		}
		//Check if the given command is invalid
		if (is_valid_command == 0)
			printf("Invalid command\n");
	}
	return 0;
}

