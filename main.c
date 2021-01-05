//BRANZEU MIHNEA 313CA
#include <stdio.h>
#include <string.h>
#include "functions.h"

#define MAX_LENGTH_COMMAND 100



int main()
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
                //Adjusting the command string 
                command[strlen(command) - 1] = '\0';
                //Launching the command
                load(command + 5, &photo);
            } else printf("Invalid command\n");
        }
        //Check if the command is SELECT ALL
        if (strstr(command, "SELECT ALL") != NULL) {
            if (strlen(command) == 11) {
                is_valid_command = 1;
                //Check if a photo has been loaded
                if (photo.type != -1) {
                    select_all(&photo);
                    printf("Selected ALL\n");
                } else {
                printf("No image loaded.\n");
                }
            } else printf("Invalid command\n");
        } else { // Check if the command is SELECT
            if (strstr(command, "SELECT") != NULL) {
                is_valid_command = 1;
                //Check if a photo has been loaded
                if (photo.type != -1) {
                    //Parse the parametres of the command
                    int x1 = 0, y1= 0, x2 = 0, y2 = 0;
                    int k = strlen("SELECT ");
                    if (strlen(command) < 9) {
                        invalid_selectall = 1;
                    } else {
                        //Get x1
                        while(k < (int)strlen(command) - 1 && command[k] != ' ') {
                            x1 = x1 * 10 + (command[k] - '0');
                            k++;
                            if (k == (int)strlen(command) - 1) {
                                invalid_selectall = 1;
                                break;
                            }
                        }
                        //Get y1
                        k++;
                        while(k < (int)strlen(command) - 1 && command[k] != ' ') {
                            y1 = y1 * 10 + (command[k] - '0');
                            k++;
                            if (k == (int)strlen(command) - 1) {
                                invalid_selectall = 1;
                                break;
                            }
                        }
                        k++;
                        //Get x2
                        while(k < (int)strlen(command) - 1 && command[k] != ' ') {
                            x2 = x2 * 10 + (command[k] - '0');
                            k++;
                            if (k == (int)strlen(command) - 1) {
                                invalid_selectall = 1;
                                break;
                            }
                        }
                        k++;
                        //Get y2
                        while(k < (int)strlen(command) &&  command[k] != '\n') {
                            y2 = y2 * 10 + (command[k] - '0');
                            k++;
                        }
                    }
                    if (invalid_selectall == 1 || k < (int)strlen(command) - 1 || strlen(command) < 9) {
                            printf("Invalid command\n");
                    } else {
                        if (0 <= y1 && y1 <= photo.height && 0 <= y2 && y2 <= photo.height && 0 <= x1 && x1 <= photo.width && 0 <= x2 && x2 <= photo.width) {
                            printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
                            //Valid input
                            if (x1 > x2)
                                swap(&x1, &x2);
                            if (y1 > y2)
                                swap(&y1, &y2);
                            //Check if the whole image was selected
                            if (x1 == 0 && x2 == photo.height && y1 == 0 && y2 == photo.width) {
                                photo.is_selectedall = 1; 
                            } else {
                                photo.is_selectedall = 0;
                            }
                           /* if (y1 == photo.height)
                                y1--;
                            if (x1 == photo.width)
                                x1--;
                            if (y2 == photo.height)
                                y2--;
                            if (x2 == photo.width)
                                x2--;
                            */
                            x2--;
                            y2--;
                            photo.x1 = y1;
                            photo.x2 = y2;
                            photo.y1 = x1;
                            photo.y2 = x2;
                        } else {
                            printf("Invalid coordinates\n");
                        }
                    }
                } else printf("No image loaded\n");
            }
        }
        //Check if the command is SAVE
        if (strstr(command, "SAVE") != NULL) {
            is_valid_command = 1;
            if (photo.type != -1) {
                if (strstr(command, "ascii") != NULL) {
                    if (strlen(command) > 12) {
                        //Extracting the filename
                        int k = 0;
                        while ((command + 5)[k] != ' ') {
                            k++;
                        }
                        (command + 5)[k] = '\0';
                        save(&photo, command + 5, 1);
                    } else {
                        printf("Invalid command\n");
                    }
                } else {
                    if (strlen(command) > 6) {
                        //Extracting the filename
                        (command + 5)[strlen((command + 5)) - 1] = '\0';
                        save(&photo, command + 5, 0);
                    } else {
                        printf("Invalid command\n");
                    }
                }
            } else {
                printf("No image loaded\n");
            }
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
                if ((photo.type == 3 || photo.type == 6) && photo.is_grayscale == 0) {
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
            crop(&photo);
        }

        //Check if the command is EXIT
        if (strcmp(command, "EXIT\n") == 0) {
            is_valid_command = 1;
            //Free memory
            destroy_photo(&photo);
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
                    if (angle == 90 || angle == 180 || angle == 270) {
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