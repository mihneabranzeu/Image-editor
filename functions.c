//BRANZEU MIHNEA 313CA
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

#define MAX_PATH_LENGTH 100

pixel_t **alloc_matrix(int height, int width)
{
    //Alocating memory for the matrix
    pixel_t **matrix = (pixel_t **)malloc(height * sizeof(pixel_t *));
    if (matrix == NULL) {
        printf("Couldn't store photo\n");
        return 0;
    }
    for (int i = 0; i < height; i++) {
        matrix[i] = (pixel_t *)malloc(width * sizeof(pixel_t));
        if (matrix[i] == NULL) {
            printf("Couldn't store photo\n");
            return 0;
        }
    }
    return matrix;
}

void swap(int *a, int *b) {
    //Swaps two integer values
    int x = *a;
    *a = *b;
    *b = x;
}

int powof10(int x){
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


//FUNCTIA LOAD:
//1. DESCHIDE FISIERUL DACA SE POATE
//2. AFLA TIPUL IMAGINII
//3. ALOCA STRUCTURA DE CARE AVEM NEVOIE 
void load(char filename[], photo_t *photo)
{
    //Open the file
    FILE *in;
    in = fopen(filename, "rb");
    if (in == NULL) {
        printf("Failed to load %s\n", filename);
        return;
    }
    char buff;
    fread(&buff, sizeof(char), 1, in);
    //Check for comments
    while (buff == '#') {
        while(buff != '\n') {
            fread(&buff, sizeof(char), 1, in);
        }
        fread(&buff, sizeof(char), 1, in);
    } 
    //Read the type of photo
    fread(&buff, sizeof(char), 1, in);
    photo->type = buff - '0';
    
    //Get to the next line
    fread(&buff, sizeof(char), 1, in);

    fread(&buff, sizeof(char), 1, in);
    //Check for comments
    while (buff == '#') {
        while (buff != '\n') {
            fread(&buff, sizeof(char), 1, in);
        }
        fread(&buff, sizeof(char), 1, in); //Read new buffer char
    }

    //Dimensions
    //Width
    photo->width = 0;
    while (buff != ' ') {
        photo->width = photo->width * 10 + (buff - '0');
        fread(&buff, sizeof(char), 1, in);
    }
    //Get to the first digit of height
    fread(&buff, sizeof(char), 1, in);
    //Height
    photo->height = 0;
    while (buff != '\n') {
        photo->height = photo->height * 10 + (buff - '0');
        fread(&buff, sizeof(char), 1, in);
    }

    //Select all image
    photo->x1 = 0;
    photo->y1 = 0;
    photo->y2 = photo->height - 1;
    photo->x2 = photo->width - 1;
    photo->is_selectedall = 1;
    
    fread(&buff, sizeof(char), 1, in);
    //Check for comments
    while (buff == '#') {
        while (buff != '\n') {
            fread(&buff, sizeof(char), 1, in);
        }
        fread(&buff, sizeof(char), 1, in);
    }
    if (photo->type != 1 && photo->type != 4) {
        //Read the maxvalue
        photo->maxvalue = 0;
        while (buff != '\n') {
            photo->maxvalue = photo->maxvalue * 10 + (buff - '0');
            fread(&buff, sizeof(char), 1, in);
        }
    }


    fread(&buff, sizeof(char), 1, in);
    //Check for comments
    while (buff == '#') {
        while (buff != '\n')
            fread(&buff, sizeof(char), 1, in);
        fread(&buff, sizeof(char), 1, in);
    }

    fseek(in, -2, SEEK_CUR);
    
    //Memorizing the current position
    int pos = ftell(in);

    //Reading the matrix
    //Allocating memory
    photo->matrix = alloc_matrix(photo->height, photo->width);

    if (photo->type > 3) { //Working on reading the binary formats
        //Go back to the beginning of the matrix
        //fseek(in, -2, SEEK_CUR);
        switch (photo->type)
        {
            case 4:
                //Reading the type 4 file
                //Reading the matrix
                for (int i = 0; i < photo->height; i++)
                    for (int j = 0; j < photo->width; j++)
                        fread(&photo->matrix[i][j].black, sizeof(unsigned char), 1, in);
                /*for(int i = 0; i < photo->height; i++) {
                    for (int j = 0; j < photo->width; j++)
                        printf("%d ", photo->matrix[i][j].black);
                    printf("\n");
                }*/
                break;
            
            case 5:
                //Reading the Grayscale image(type 5)
                //Reading the matrix
                for (int i = 0; i < photo->height; i++) {
                    for (int j = 0; j < photo->width; j++) {
                        fread(&photo->matrix[i][j].gray, sizeof(unsigned char), 1, in);
                    }
                }
                //Confirming the successful load
                printf("Loaded %s\n", filename);
                break;

            case 6:
                //Reading the RGB image(type 6)
                //Reading the matrix
                for (int i = 0; i < photo->height; i++)
                    for (int j = 0; j < photo->width; j++) {
                        fread(&photo->matrix[i][j].red, sizeof(unsigned char), 1, in);
                        fread(&photo->matrix[i][j].green, sizeof(unsigned char), 1, in);
                        fread(&photo->matrix[i][j].blue, sizeof(unsigned char), 1, in);
                    }
                //Confirming the successful load
                printf("Loaded %s\n", filename);
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

        switch (photo->type)
        {
        case 1:
            //Reading the black and white format
            for (int i = 0; i < photo->height; i++)
                for (int j = 0; j < photo->width; j++)
                    fscanf(in, "%d", &photo->matrix[i][j].black);
            //Confirming the successful load
            printf("Loaded %s\n", filename);
            break;
        
        case 2:
            //Reading the Grayscale format
            for (int i = 0; i < photo->height; i++)
                for (int j = 0; j < photo->width; j++)
                    fscanf(in, "%d", &photo->matrix[i][j].gray);
            //Confirming the successful load
            printf("Loaded %s\n", filename);
            break;

        case 3:
            //Reading the RGB format
            for (int i = 0; i < photo->height; i++)
                for (int j = 0; j < photo->width; j++) {
                    fscanf(in, "%d", &photo->matrix[i][j].red);
                    fscanf(in, "%d", &photo->matrix[i][j].green);
                    fscanf(in, "%d", &photo->matrix[i][j].blue);
                }
            //Confirming the successful load
            printf("Loaded %s\n", filename);
            break;

        default:
            break;
        }
    }
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
    switch (photo->type)
    {
    case 1:
        if (is_ascii == 1)
            new_type = 1;
        else new_type = 4;
        break;
    
    case 2:
        if (is_ascii == 1)
            new_type = 2;
        else new_type = 5;
        break;
    
    case 3:
        if (is_ascii == 1)
            new_type = 3;
        else new_type = 6;
        break;
        
    case 4:
        if (is_ascii == 1)
            new_type = 1;
        else new_type = 4;
        break;

    case 5:
        if (is_ascii == 1)
            new_type = 2;
        else new_type = 5;
        break;
    
    case 6:
        if (is_ascii == 1)
            new_type = 3;
        else new_type = 6;
        break;
    
    default:
        break;
    }
    buff = (char)(new_type + '0');
    fwrite(&buff, sizeof(char), 1, out);
    buff = '\n';
    fwrite(&buff, sizeof(char), 1, out);
    
    //Printing the dimensions
    int p10 = powof10(photo->width);
    while (p10 > 0) {
        buff = (photo->width / p10) % 10 + '0';
        fwrite(&buff, sizeof(char), 1, out);
        p10 /= 10;
    }
    buff = ' ';
    fwrite(&buff, sizeof(char), 1, out);
    
    p10 = powof10(photo->height);
    while (p10 > 0) {
        buff = (photo->height / p10) % 10 + '0';
        fwrite(&buff, sizeof(char), 1, out);
        p10 /= 10;
    }
    buff = '\n';
    fwrite(&buff, sizeof(char), 1, out);
    
    //Printing the maxvalue (if neccessary)
    if (new_type != 1 && new_type != 4) {
        p10 = powof10(photo->maxvalue);
        while (p10 > 0) {
            buff = (photo->maxvalue / p10) % 10 + '0';
            fwrite(&buff, sizeof(char), 1, out);
            p10 /= 10;
        }
        buff = '\n';
        fwrite(&buff, sizeof(unsigned char), 1, out);
    }

    //Printing the matrix
    if (new_type > 3) { //Working on the binary formats
        switch (new_type)
        {
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
                    fwrite(&buff, sizeof(unsigned char), 1 ,out);
                    buff = (unsigned char)photo->matrix[i][j].green;
                    fwrite(&buff, sizeof(unsigned char), 1 ,out);
                    buff = (unsigned char)photo->matrix[i][j].blue;
                    fwrite(&buff, sizeof(unsigned char), 1 ,out);
                }
            }
            break;
        
        default:
            break;
        }
    } else {
        fclose(out);
        out = fopen(filename, "a");
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
            newval = 1.0 * (photo->matrix[i][j].red + photo->matrix[i][j].green + photo->matrix[i][j].blue) / 3;
            photo->matrix[i][j].red = myround(newval);
            photo->matrix[i][j].green = myround(newval);
            photo->matrix[i][j].blue = myround(newval);
        }
    }
    printf("Grayscale filter applied\n");
    //Check if the whole image was selected
    if (photo->is_selectedall == 1) {
        photo->is_grayscale = 1; //helps for the sepia filter
    }
    
}

int mymin(int a, int b) {
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
    int new_height = photo->x2 - photo->x1 + 1;
    int new_width = photo->y2 - photo->y1 + 1;
    //Extracting the data in a copy
    pixel_t **copy;
    copy = alloc_matrix(new_height, new_width);
    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            copy[i][j] = photo->matrix[i + photo->x1][j + photo->y1];
        }
    }

    destroy_photo(photo);

    //Realloc the memory
    photo->height = new_height;
    photo->width = new_width;
    photo->matrix = alloc_matrix(new_height, new_width);
    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            photo->matrix[i][j] = copy[i][j];
        }
    }
    //Select all image
    select_all(photo);
    //Destroy the copy
    for (int i = 0; i < new_height; i++) {
        free(copy[i]);
    }
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
        for (int i = photo->x1; i <= photo->x2; i++)
            for (int j = photo->y1; j <= photo->y2; j++)
                copy[i - photo->x1][j - photo->y1] = photo->matrix[i][j];

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
        for (int i = photo->x1; i <= photo->x2; i++)
            for (int j = photo->y1; j <= photo->y2; j++)
                photo->matrix[i][j] = copy[i - photo->x1][j - photo->y1];

        //Free the memory used by the copy
        for (int i = 0; i < length; i++)
            free(copy[i]);
        free(copy);

        //Decrease the number of remaining rotations
        nrrot--;
    }
}

void destroy_photo(photo_t *photo)
{
    //Free memory for the matrix
    for (int i = 0; i < photo->height; i++)
        free(photo->matrix[i]);
    free(photo->matrix);
}

