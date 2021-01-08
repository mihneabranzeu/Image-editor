build:main.c functions.c
	gcc main.c functions.c -o image_editor -Wall -Wextra
clean:image_editor
	rm -rf image_editor