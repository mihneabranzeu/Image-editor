
typedef struct
{
	//The structure of a pixel
	int red;
	int green;
	int blue;
	int black;
	int gray;
}pixel_t;

typedef struct 
{
	int type;
	int maxvalue;
	int height;
	int width;
	//Selected portion
	int x1;
	int y1;
	int x2;
	int y2;
	int is_selectedall; //used for CROP and SEPIA
	int is_grayscale; //used for deciding whether SEPIA can be applied 
	pixel_t **matrix;
} photo_t;

void load(char filename[], photo_t *photo);
pixel_t** alloc_matrix(int height, int width);
void set_photo(photo_t *photo);
void select_all(photo_t *photo);
void swap(int *a, int *b);
void save(photo_t *photo, char filename[], int is_ascii);
int powof10(int x);
void destroy_photo(photo_t *photo);
void apply_grayscale(photo_t *photo);
void apply_sepia(photo_t *photo);
void rotate_all(photo_t *photo, int nrrot);
void rotate_selection(photo_t *photo, int nrrot);
void crop(photo_t *photo);
int myround(double x);
void check_loaded(photo_t *photo);
void check_comments(FILE *in, char *buff);
int parse_value(FILE *in, char *buff);
void alloc_and_read_matrix(char filename[], FILE *in, photo_t *photo, int pos);
void print_dimension(FILE *out, int value);
void print_matrix(FILE *out, char filename[], int new_type, photo_t *photo);
void load_logic(char command[], photo_t *photo);
void select_all_logic(photo_t *photo);
void select_logic(char command[], photo_t *photo, int *invalid_selectall);
int get_selected_value(char command[], int *k, int *is_numerical_input, int *invalid_selectall);
void save_logic(char command[], photo_t *photo);