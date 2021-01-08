//BRANZEU MIHNEA 313CA
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

#define MAX_PATH_LENGTH 100

pixel_t **alloc_matrix(int height, int width)
{
	//Alocating memory for the matrix
	pixel_t **matrix = (pixel_t **)calloc(height, sizeof(pixel_t *));
	if (!matrix) {
		printf("Couldn't store photo\n");
		return 0;
	}
	for (int i = 0; i < height; i++) {
		matrix[i] = (pixel_t *)calloc(width, sizeof(pixel_t));
		if (!matrix[i]) {
			printf("Couldn't store photo\n");
			return 0;
		}
	}
	return matrix;
}

void destroy_photo(photo_t *photo)
{
	//Free memory for the matrix
	for (int i = 0; i < photo->height; i++)
		free(photo->matrix[i]);
	free(photo->matrix);
}

void swap(int *a, int *b)
{
	//Swaps two integer values
	int x = *a;
	*a = *b;
	*b = x;
}

int powof10(int x)
{
	//Gets 10 ^ nrdigits -1
	//Used for printing a number starting from its first digit
	int pow = 1;
	while (x > 0) {
		x /= 10;
		pow *= 10;
	}
	return pow / 10;
}

void set_photo(photo_t *photo)
{
	//Set the initial values of a photo
	photo->type = -1;
	photo->width = -1;
	photo->height = -1;
	photo->maxvalue = -1;
	photo->is_grayscale = 0;
}

void check_loaded(photo_t *photo)
{
	if (photo->type != -1) {
		///Free the memory
		destroy_photo(photo);
		set_photo(photo);
	}
}

void load_logic(char command[], photo_t *photo)
{
	//Does the logic for the load command 
	//Adjusting the command string 
	command[strlen(command) - 1] = '\0';
	//Check if another photo was loaded before
	if (photo->type != -1) {
		//Free memory
		destroy_photo(photo);
		set_photo(photo);
	}
	//Launching the command
	load(command + 5, photo);
}

void select_all_logic(photo_t *photo)
{
	//Does the logic for the select all command
	//Check if a photo has been loaded
	if (photo->type != -1) {
		select_all(photo);
		printf("Selected ALL\n");
	} else {
	printf("No image loaded\n");
	}	
}

int get_selected_value(char command[], int *k, int *is_numerical_input, int *invalid_selectall)
{
	//Gets the value of a selected coordinate
	int value = 0;
	while(*k < (int)strlen(command) - 1 && command[*k] != ' ') {
		if ((command[*k] - '0' < 0 || command[*k] - '0' > 9) && (command[*k] != '-')) {
			*is_numerical_input = 0;
		}
		value = value * 10 + (command[*k] - '0');
		(*k)++;
		if (*k == (int)strlen(command) - 1) {
			*invalid_selectall = 1;
			break;
		}
	}
	return value;
}

void select_logic(char command[], photo_t *photo, int *invalid_selectall)
{
	//Does the logic for the select command
	//Parse the parametres of the command
	int x1 = 0, y1= 0, x2 = 0, y2 = 0;
	int is_numerical_input = 1;
	int k = strlen("SELECT ");
	if (strlen(command) < 9) {
		*invalid_selectall = 1;
	} else {
		//Get x1
		x1 = get_selected_value(command, &k, &is_numerical_input, invalid_selectall);
		//Get y1
		k++;
		y1 = get_selected_value(command, &k, &is_numerical_input, invalid_selectall);
		k++;
		//Get x2
		x2 = get_selected_value(command, &k, &is_numerical_input, invalid_selectall); 
		k++;
		//Get y2
		while(k < (int)strlen(command) &&  command[k] != '\n') {
			if ((command[k] - '0' < 0 || command[k] - '0' > 9) && (command[k] != '-')) {
				is_numerical_input = 0;
			}
			y2 = y2 * 10 + (command[k] - '0');
			k++;
		}
	}
	if (*invalid_selectall == 1 || k < (int)strlen(command) - 1 || strlen(command) < 9 || is_numerical_input == 0) {
			printf("Invalid command\n");
	} else {
		if (0 <= y1 && y1 <= photo->height && 0 <= y2 && y2 <= photo->height && 0 <= x1 && x1 <= photo->width && 0 <= x2 && x2 <= photo->width && x1 != x2 && y1 != y2 && is_numerical_input == 1) {
			//Valid input
			if (x1 > x2)
				swap(&x1, &x2);
			if (y1 > y2)
				swap(&y1, &y2);
			printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
			//Check if the whole image was selected
			if (x1 == 0 && x2 == photo->width && y1 == 0 && y2 == photo->height) {
				photo->is_selectedall = 1; 
			} else {
				photo->is_selectedall = 0;
			}
			x2--;
			y2--;
			photo->x1 = x1;
			photo->x2 = x2;
			photo->y1 = y1;
			photo->y2 = y2;
		} else {
			printf("Invalid set of coordinates\n");
		}
	}
}

FILE *open_file(char filename[])
{
	//Opens the input file
	FILE *in;
	in = fopen(filename, "rb");
	if (!in)
		printf("Failed to load %s\n", filename);
	return in;
}

void check_comments(FILE *in, char *buff)
{
	//Checks for comments in input file
	fread(buff, sizeof(char), 1, in);
	//Check for comments
	while (*buff == '#') {
		while (*buff != '\n')
			fread(buff, sizeof(char), 1, in);
		fread(buff, sizeof(char), 1, in);
	}
}

int parse_value(FILE *in, char *buff)
{
	//Reads a string and parses it, returning a nummerical value
	int value = 0;
	while (*buff != ' ' && *buff != '\n') {
		value = value * 10 + (*buff - '0');
		fread(buff, sizeof(char), 1, in);
	}
	return value;
}

void alloc_and_read_matrix(char filename[], FILE *in, photo_t *photo, int pos)
{
	//Builds the photo matrix
	photo->matrix = alloc_matrix(photo->height, photo->width);
	if (photo->type > 3) { //Working on reading the binary formats
		switch (photo->type) {
			case 4:
				//Reading the type 4 file
				//Reading the matrix
				for (int i = 0; i < photo->height; i++)
					for (int j = 0; j < photo->width; j++)
						fread(&photo->matrix[i][j].black,
							  sizeof(unsigned char), 1, in);

				break;
			case 5:
				//Reading the Grayscale image(type 5)
				//Reading the matrix
				for (int i = 0; i < photo->height; i++) {
					for (int j = 0; j < photo->width; j++) {
						fread(&photo->matrix[i][j].gray,
							  sizeof(unsigned char), 1, in);
					}
				}
				break;
			case 6:
				//Reading the RGB image(type 6)
				//Reading the matrix
				for (int i = 0; i < photo->height; i++)
					for (int j = 0; j < photo->width; j++) {
						fread(&photo->matrix[i][j].red,
							  sizeof(unsigned char), 1, in);
						fread(&photo->matrix[i][j].green,
							  sizeof(unsigned char), 1, in);
						fread(&photo->matrix[i][j].blue,
							  sizeof(unsigned char), 1, in);
					}
				break;
			default:
				break;
		}
	} else { //Working on the ascii formats
		//Opening the file in a prefered format
		fclose(in);
		in = fopen(filename, "r");
		//Going to the start of the matrix
		fseek(in, pos, SEEK_SET);
		switch (photo->type) {
		case 1:
			//Reading the black and white format
			for (int i = 0; i < photo->height; i++)
				for (int j = 0; j < photo->width; j++)
					fscanf(in, "%d", &photo->matrix[i][j].black);
			break;
		case 2:
			//Reading the Grayscale format
			for (int i = 0; i < photo->height; i++)
				for (int j = 0; j < photo->width; j++)
					fscanf(in, "%d", &photo->matrix[i][j].gray);
			break;
		case 3:
			//Reading the RGB format
			for (int i = 0; i < photo->height; i++)
				for (int j = 0; j < photo->width; j++) {
					fscanf(in, "%d", &photo->matrix[i][j].red);
					fscanf(in, "%d", &photo->matrix[i][j].green);
					fscanf(in, "%d", &photo->matrix[i][j].blue);
				}
			break;
		default:
			break;
		}
	}
	//Confirming the successful load
	printf("Loaded %s\n", filename);
}

void load(char filename[], photo_t *photo)
{
	//Loads the input file into memory
	//Check if another photo was loaded before
	check_loaded(photo);
	FILE *in;
	in = open_file(filename);
	if (!in)
		return;
	//Start the reading logic
	char buff;
	check_comments(in, &buff);
	//Read the type of photo
	fread(&buff, sizeof(char), 1, in);
	photo->type = buff - '0';
	//Get to the next line
	fread(&buff, sizeof(char), 1, in);
	check_comments(in, &buff);
	//Dimensions
	//Width
	photo->width = parse_value(in, &buff);
	//Get to the first digit of height
	fread(&buff, sizeof(char), 1, in);
	//Height
	photo->height = parse_value(in, &buff);
	//Select all image
	select_all(photo);
	check_comments(in, &buff);
	if (photo->type != 1 && photo->type != 4) {
		//Read the maxvalue
		photo->maxvalue = parse_value(in, &buff);
	}
	check_comments(in, &buff);
	fseek(in, -1, SEEK_CUR);
	//Memorizing the current position
	int pos = ftell(in);
	//Allocating memory and reading the matrix
	alloc_and_read_matrix(filename, in, photo, pos);
	//Close the input file
	fclose(in);
}

void select_all(photo_t *photo)
{
	//Selects the entire surface of a photo
	photo->x1 = 0;
	photo->y1 = 0;
	photo->y2 = photo->height - 1;
	photo->x2 = photo->width - 1;
	photo->is_selectedall = 1;
}

void print_dimension(FILE *out, int value)
{
	//Transforms a value into chars and writes them onto a file
	int p10 = powof10(value);
	char buff;
	while (p10 > 0) {
		buff = (value / p10) % 10 + '0';
		fwrite(&buff, sizeof(char), 1, out);
		p10 /= 10;
	}
}

void print_matrix(FILE *out, char filename[], int new_type, photo_t *photo)
{
	//Prints the photo matrix onto  a file
	char buff;
	if (new_type > 3) { //Working on the binary formats
		switch (new_type) {
		case 4:
			//Printing the black and white format
			for (int i = 0; i < photo->height; i++) {
				for (int j = 0; j < photo->width; j++) {
					buff = (unsigned char)photo->matrix[i][j].black;
					fwrite(&buff, sizeof(unsigned char), 1, out);
				}
			}
			break;

		case 5:
			//Printing the GRAYSCALE format
			for (int i = 0; i < photo->height; i++) {
				for (int j = 0; j < photo->width; j++) {
					buff = (unsigned char)photo->matrix[i][j].gray;
					fwrite(&buff, sizeof(unsigned char), 1, out);
				}
			}
			break;

		case 6:
			//Printing the RGB format
			for (int i = 0; i < photo->height; i++) {
				for (int j = 0; j < photo->width; j++) {
					buff = (unsigned char)photo->matrix[i][j].red;
					fwrite(&buff, sizeof(unsigned char), 1, out);
					buff = (unsigned char)photo->matrix[i][j].green;
					fwrite(&buff, sizeof(unsigned char), 1, out);
					buff = (unsigned char)photo->matrix[i][j].blue;
					fwrite(&buff, sizeof(unsigned char), 1, out);
				}
			}
			break;
		default:
			break;
		}
	} else {
		fclose(out);
		out = fopen(filename, "a");
		fseek(out, + 1, SEEK_CUR);
		switch (new_type)
		{
		case 1:
			//Printing the black and white format
			for (int i = 0; i < photo->height; i++) {
				for (int j = 0; j < photo->width; j++)
					fprintf(out, "%d ", photo->matrix[i][j].black);
				fprintf(out, "\n");
			}
			break;

		case 2:
			//Printing the Grayscale format
			for (int i = 0; i < photo->height; i++) {
				for (int j = 0; j < photo->width; j++)
					fprintf(out, "%d ", photo->matrix[i][j].gray);
				fprintf(out, "\n");
			}
			break;
		
		case 3:
			//Printing the RGB format
			for (int i = 0; i < photo->height; i++) {
				for (int j = 0; j < photo->width; j++)
					fprintf(out, "%d %d %d ", photo->matrix[i][j].red, photo->matrix[i][j].green, photo->matrix[i][j].blue);
				fprintf(out, "\n");
			}
			break;
		
		default:
			break;
		}
	}
}

void save(photo_t *photo, char filename[], int is_ascii)
{
	//Opening the file in binary format
	FILE *out;
	out  = fopen(filename, "wb");
	char buff; //Declaring the buffer that will do the writing
	//Printing the file type
	buff = 'P';
	fwrite(&buff, sizeof(char), 1, out);
	//Getting the new type of file
	int new_type;
	switch (is_ascii) {
	case 1:
		if (photo->type > 3)
			new_type = photo->type - 3;
		else
			new_type = photo->type;
		break;
	case 0:
		if (photo->type <= 3)
			new_type = photo->type + 3;
		else
			new_type = photo->type;
		break;
	default:
		break;
	}
	buff = (char)(new_type + '0');
	fwrite(&buff, sizeof(char), 1, out);
	buff = '\n';
	fwrite(&buff, sizeof(char), 1, out);
	//Printing the dimensions
	print_dimension(out, photo->width);
	buff = ' ';
	fwrite(&buff, sizeof(char), 1, out);
	print_dimension(out, photo->height);
	buff = '\n';
	fwrite(&buff, sizeof(char), 1, out);
	//Printing the maxvalue (if neccessary)
	if (new_type != 1 && new_type != 4) {
		print_dimension(out, photo->maxvalue);
		buff = '\n';
		fwrite(&buff, sizeof(char), 1, out);
	}
	//Printing the matrix
	print_matrix(out, filename, new_type, photo);
	printf("Saved %s\n", filename);
	fclose(out);
}

int myround(double x)
{
	//Rounds a double value to the closest int
	int floor = (int)x;
	int ceil = (int)x + 1;
	if (x - floor > ceil - x)
		return ceil;
	return floor;
}

void apply_grayscale(photo_t *photo)
{
	//Applies the grayscale filter to the selected portion of the photo
	for (int i = photo->y1; i <= photo->y2; i++) {
		for (int j = photo->x1 ; j <= photo->x2; j++) {
			double newval;
			newval = 1.0 * (photo->matrix[i][j].red + photo->matrix[i][j].green
							+ photo->matrix[i][j].blue) / 3;
			photo->matrix[i][j].red = myround(newval);
			photo->matrix[i][j].green = myround(newval);
			photo->matrix[i][j].blue = myround(newval);
		}
	}
	printf("Grayscale filter applied\n");
	//Check if the whole image was selected
	if (photo->is_selectedall == 1)
		photo->is_grayscale = 1; //helps for the sepia filter
}

int mymin(int a, int b)
{
	//Returns the smaller value out of two integers
	if (a < b)
		return a;
	return b;
}

void apply_sepia(photo_t *photo)
{
	//Applies the sepia filter to the selected portion of the photo
	for (int i = photo->y1; i <= photo->y2; i++) {
		for (int j = photo->x1; j <= photo->x2; j++) {
			double newval;
			int red = photo->matrix[i][j].red;
			int green = photo->matrix[i][j].green;
			int blue = photo->matrix[i][j].blue;
			//Compute the red value
			newval =  0.393 * red + 0.769 * green + 0.189 * blue;
			photo->matrix[i][j].red = mymin(myround(newval), photo->maxvalue);
			//Compute the green value
			newval = 0.349 * red + 0.686 * green + 0.168 * blue;
			photo->matrix[i][j].green = mymin(myround(newval), photo->maxvalue);
			//Compute the blue value
			newval = 0.272 * red + 0.534 * green + 0.131 * blue;
			photo->matrix[i][j].blue = mymin(myround(newval), photo->maxvalue);
		}
	}
	printf("Sepia filter applied\n");
}

void crop(photo_t *photo)
{
	//Getting the new dimensions
	int new_height = photo->y2 - photo->y1 + 1;
	int new_width = photo->x2 - photo->x1 + 1;
	//Extracting the data in a copy
	pixel_t **copy;
	copy = alloc_matrix(new_height, new_width);
	for (int i = 0; i < new_height; i++)
		for (int j = 0; j < new_width; j++)
			copy[i][j] = photo->matrix[i + photo->y1][j + photo->x1];

	destroy_photo(photo);

	//Realloc the memory
	photo->height = new_height;
	photo->width = new_width;
	photo->matrix = alloc_matrix(new_height, new_width);
	for (int i = 0; i < new_height; i++)
		for (int j = 0; j < new_width; j++)
			photo->matrix[i][j] = copy[i][j];
	//Select all image
	select_all(photo);
	//Destroy the copy
	for (int i = 0; i < new_height; i++)
		free(copy[i]);
	free(copy);
	printf("Image cropped\n");
}

void rotate_all(photo_t *photo, int nrrot)
{
	while (nrrot > 0) {
		//Create a middle structure to do the operation
		pixel_t **copy;
		copy = alloc_matrix(photo->width, photo->height);

		//Iterate through the matrix
		for (int j = 0; j < photo->width; j++)
			for (int i = photo->height - 1; i >= 0; i--)
				copy[j][photo->height - i - 1] = photo->matrix[i][j];

		//Set the memory in order for the new matrix
		destroy_photo(photo);

		//Modify the dimensions
		swap(&photo->height, &photo->width);
		photo->matrix = alloc_matrix(photo->height, photo->width);

		//Copy the elements in the matrix
		for (int i = 0; i < photo->height; i++)
			for (int j = 0; j < photo->width; j++)
				photo->matrix[i][j] = copy[i][j];

		//Free the memory used for the copy
		for (int i = 0; i < photo->height; i++)
			free(copy[i]);
		free(copy);

		//Decrease the number of remaining rotations
		nrrot--;
	}

	//Select the whole image
	select_all(photo);
}

void rotate_selection(photo_t *photo, int nrrot)
{
	while (nrrot > 0) {
		int length = photo->x2 - photo->x1 + 1;

		//Create a middle structure to do the operation
		pixel_t **copy;
		copy = alloc_matrix(length, length);

		//Copy the values
		for (int i = photo->y1; i <= photo->y2; i++)
			for (int j = photo->x1; j <= photo->x2; j++)
				copy[i - photo->y1][j - photo->x1] = photo->matrix[i][j];

		//Transpose the matrix
		for (int i = 0; i < length; i++)
			for (int j = 0; j < i; j++) {
				//Swap the values
				pixel_t aux = copy[i][j];
				copy[i][j] = copy[j][i];
				copy[j][i] = aux;
			}
		
		//Swap columns
		for (int i = 0; i < length; i++)
			for (int j = 0; j < length / 2; j++) {
				//Swap the values
				pixel_t aux = copy[i][j];
				copy[i][j] = copy[i][length - j - 1];
				copy[i][length - j - 1] = aux;
			}

		//Update the values in the matrix
		for (int i = photo->y1; i <= photo->y2; i++)
			for (int j = photo->x1; j <= photo->x2; j++)
				photo->matrix[i][j] = copy[i - photo->y1][j - photo->x1];

		//Free the memory used by the copy
		for (int i = 0; i < length; i++)
			free(copy[i]);
		free(copy);

		//Decrease the number of remaining rotations
		nrrot--;
	}
}
