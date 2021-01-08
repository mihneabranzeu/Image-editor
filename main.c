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
	while(is_running == 1) {
		fgets(command, MAX_LENGTH_COMMAND, stdin);
		int is_valid_command = 0;
		int invalid_selectall = 0;
		//Check if command is LOAD
		if (strstr(command, "LOAD") != NULL) {
			if (strlen(command) > strlen("LOAD \n")) {
				is_valid_command = 1;
				load_logic(command, &photo);
			} else printf("Invalid command\n");
		}
		//Check if the command is SELECT ALL
		if (strstr(command, "SELECT ALL") != NULL) {
			is_valid_command = 1;
			select_all_logic(&photo);
			
		} else { // Check if the command is SELECT
			if (strstr(command, "SELECT") != NULL) {
				is_valid_command = 1;
				//Check if a photo has been loaded
				if (photo.type != -1) {
					select_logic(command, &photo, &invalid_selectall);
				} else printf("No image loaded\n");
			}
		}
		//Check if the command is SAVE
		if (strstr(command, "SAVE") != NULL) {
			is_valid_command = 1;
			save_logic(command, &photo);
		}
		//Check if the command is GRAYSCALE 
		if (strcmp(command, "GRAYSCALE\n") == 0) {
			is_valid_command = 1;
			if (photo.type != -1) {
				if (photo.type == 3 || photo.type == 6) {
					//Apply the grayscale filter
					apply_grayscale(&photo);
				} else {
					printf("Grayscale filter not available\n");
				}
			} else {
				printf("No image loaded\n");
			}
		}

		//Check if the command is SEPIA
		if (strcmp(command, "SEPIA\n") == 0) {
			is_valid_command = 1;
			if (photo.type != -1) {
				if (photo.type == 3 || photo.type == 6) {
					//Apply the sepia filter
					apply_sepia(&photo);
				} else {
					printf("Sepia filter not available\n");
				}
			} else {
				printf("No image loaded\n");
			}
		}

		//Check if the command is CROP
		if (strcmp(command, "CROP\n") == 0) {
			is_valid_command = 1;
			if (photo.type != -1) {
				crop(&photo);
			} else {
				printf("No image loaded\n");
			}
		}

		//Check if the command is EXIT
		if (strstr(command, "EXIT") != NULL) {
			is_valid_command = 1;
			if (photo.type != -1) {
				//Free memory
				destroy_photo(&photo);
			} else {
				printf("No image loaded\n");
			}
			//Stop the loop
			is_running = 0;
		}

		//Check if the command is ROTATE
		if (strstr(command, "ROTATE") != NULL) {
			is_valid_command = 1;
			if (photo.type != -1) {
				if (strlen(command) > strlen("ROTATE \n")) {
					//Extract the angle value
					int angle = 0;
					int start = 7;
					//Get the sgn value
					int sgn = 1;
					if (command[7] == '-') {
						sgn = -1;
						start = 8;
					}
					if (command[7] == '+') {
						start = 8;
					}
					//Compute the angle
					while (command[start] != '\n') {
						angle = angle * 10 + (command[start] - '0');
						start++;
					}
					//Verify if the input is correct
					if (angle == 0 || angle == 90 || angle == 180 || angle == 270 || angle == 360) {
						//Check which type of rotation should be done
						int nrrot;
						//Getting the number of 90 degrees rotations that are required
						if (sgn == 1) {
							nrrot = angle / 90;
						} else {
							nrrot = (360 - angle) / 90;
						}

						if (photo.is_selectedall == 1) {
							rotate_all(&photo, nrrot);
							printf("Rotated %d\n", angle * sgn);
						} else {
							//Check if the selected area is square shaped
							if ((photo.x2 - photo.x1) == (photo.y2 - photo.y1)) {
								rotate_selection(&photo, nrrot);
								printf("Rotated %d\n", angle * sgn);
							} else {
								printf("The selection must be square\n");
							}
						}
					} else {
						printf("Unsupported rotation angle\n");
					}
				} else {
					printf("Invalid command\n");
				}
			} else {
				printf("No image loaded\n");
			}
		}

		if (is_valid_command == 0) {
			printf("Invalid command\n");
		}
	}
	return 0;
}
