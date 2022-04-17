//
// Assignment 1 19T3 COMP1511: CS Paint
// paint.c
//
// Author: Sarah Lay (z5161194@unsw.edu.au)
// Date: 25/10/2019
// 
// Overview: 
// CS Paint is a program that allows users to draw images to the terminal
// using a series of commands. Users are able to: draw lines, fill squares, 
// copy and paste, change shade and draw hollow or filled in ellipses.
//
// References:
// https://cgi.cse.unsw.edu.au/~cs1511/19T3/assignments/ass1/index.html
//

#include <stdio.h>
#include <math.h>

#define N_ROWS 20
#define N_COLS 36
#define MIN_ROWS -1
#define MIN_COLS -1

// colours
#define BLACK 0
#define WHITE 4

// commands
#define DRAW_ELLIPSE 0
#define DRAW_LINE 1
#define FILL_SQUARE 2
#define CHANGE_SHADE 3
#define COPY_PASTE 4
#define MIN_COLOUR -1
#define MAX_COLOUR 5

// directions
#define NORTH 0
#define NORTH_EAST 45
#define EAST 90
#define SOUTH_EAST 135
#define SOUTH 180
#define SOUTH_WEST 225
#define WEST 270
#define NORTH_WEST 315
#define LIMIT 360

void displayCanvas( int canvas[N_ROWS][N_COLS]);
void clearCanvas(   int canvas[N_ROWS][N_COLS]);
double distance(    int row1, int col1, int row2, int col2);

// Function prototypes
int change_shade(   int colour);
int within_bounds(  int start_row, int start_col, int length);
int target_within_bounds (
                    int target_row, int target_col, int length);
int correct_dir (   int direction, int length);
int correct_length (int length);
int draw_line(      int start_row, int start_col, int length, int direction, 
                    int canvas[N_ROWS][N_COLS], int colour);
int fill_square(    int start_row, int start_col, int length, int direction, 
                    int canvas[N_ROWS][N_COLS], int colour);
void draw_line_or_square (
                    int draw_type, int canvas[N_ROWS][N_COLS], int colour);
void copy_and_paste(int canvas[N_ROWS][N_COLS]);
void copy_paste(    int start_row, int start_col, int length, int direction,
                    int target_row, int target_col, 
                    int canvas[N_ROWS][N_COLS]);
void draw_ellipse(  int canvas[N_ROWS][N_COLS], int colour);
void fill_ellipse(  int focus_1_row, int focus_1_col, int focus_2_row, 
                    int focus_2_col, double length,
                    int canvas[N_ROWS][N_COLS], int colour);
int ellipse_edge(   int storage[N_ROWS][N_COLS], int colour, 
                    int row, int col);
void hollow_ellipse(int focus_1_row, int focus_1_col, int focus_2_row, 
                    int focus_2_col, double length,  
                    int canvas[N_ROWS][N_COLS], int colour);

int main(void) {
    int canvas[N_ROWS][N_COLS];

    clearCanvas(canvas);
    
    int draw_type;
    int colour = BLACK;
    
    while (scanf("%d", &draw_type) == 1) { 
 
        if (draw_type == CHANGE_SHADE) {
            colour = change_shade(colour);
        } else if (draw_type == DRAW_ELLIPSE) {
            draw_ellipse(canvas, colour);
        } else if (draw_type == COPY_PASTE) {
            copy_and_paste(canvas);
        } else {
            draw_line_or_square(draw_type, canvas, colour); 
        }
    }

    displayCanvas(canvas);

    return 0;
}

// Displays the canvas, by printing the integer value stored in
// each element of the 2-dimensional canvas array.
void displayCanvas(int canvas[N_ROWS][N_COLS]) {
    int row = 0;
    while (row < N_ROWS) {
        int col = 0;
        while (col < N_COLS) {
            printf("%d ", canvas[row][col]);
            col++;
        }
        row++;
        printf("\n");
    }
}

// Sets the entire canvas to be blank, by setting each element in the
// 2-dimensional canvas array to be WHITE (which is #defined at the top
// of the file).
void clearCanvas(int canvas[N_ROWS][N_COLS]) {
    int row = 0;
    while (row < N_ROWS) {
        int col = 0;
        while (col < N_COLS) {
            canvas[row][col] = WHITE;
            col++;
        }
        row++;
    }
}

// Calculates the distance between two points (row1, col1) and (row2, col2).
double distance(int row1, int col1, int row2, int col2) {
    int row_dist = row2 - row1;
    int col_dist = col2 - col1;
    return sqrt((row_dist * row_dist) + (col_dist * col_dist));
}

// Scans for input for a shade change and changes the shade 
// if the shade is black, grey, light, or white.
int change_shade(int colour) {
    int colour_add;
    scanf("%d", &colour_add);
    if (
        colour + colour_add >= BLACK && colour + colour_add <= WHITE
    ) {
        colour = colour + colour_add;
    }
    return colour;
}

// Checks that input is within the bounds of the canvas and returns
// TRUE or FALSE accordingly
int within_bounds(int start_row, int start_col, int length) {
    if (
        length > 0 && (start_col >= MIN_COLS) && 
        (start_col < N_COLS) && (start_row < N_ROWS) && 
        (start_row >= MIN_ROWS)
    ) {
        return 1;
    } else {
        return 0;
    }
}

// Checks if the pasted pixels of copy_paste does not go outside 
// the canvas and returns TRUE or FALSE accordingly
int target_within_bounds (int target_row, int target_col, int length) {
    if (
        target_row + length < N_ROWS && 
        target_col + length < N_COLS
    ) {
        return 1;
    } else {
        return 0;
    }      
}

// Checks if the length input is negative or if a direction input 
// is more than 360 degrees and returns a modified direction if true
int correct_dir (int direction, int length) {
    // negative lengths
    if (length < 0) {
        direction += SOUTH;
    } 

    while (direction >= LIMIT) {
        direction = direction - LIMIT;
    }

    return direction;
}

// Checks if the length input is negative and returns a 
// modified length if true
int correct_length (int length) {
    // negative lengths
    if (length < 0) {
        length = -1 * length;
    }
    return length;
}

// Draws a straight line on the canvas given the 
// starting pixel, length, direction and colour
int draw_line(
    int start_row, int start_col, 
    int length, int direction, 
    int canvas[N_ROWS][N_COLS], int colour
) {

    // corrects direction if direction input is more than 360
    direction = correct_dir(direction, length);

    int row = start_row;
    int col = start_col;

    if (direction == EAST && (start_col + length <= N_COLS)) {
        while (col < (start_col + length)) {
            canvas[row][col] = colour;
            col++;
        }
    } else if (direction == WEST && (start_col - length >= MIN_COLS)) {     
        while (col > (start_col - length)) {
            canvas[row][col] = colour;
            col--;
        }
    } else if (direction == NORTH && (start_row - length >= MIN_ROWS)) {
        while (row > (start_row - length)) {
            canvas[row][col] = colour;
            row--;
        }
    } else if (direction == SOUTH && (start_row + length <= N_ROWS)) {
        while (row < (start_row + length)) {
            canvas[row][col] = colour;
            row++;
        }
    } else if (direction == NORTH_EAST && (start_row - length >= MIN_ROWS)) {
        while (row > (start_row - length)) {
            canvas[row][col] = colour;
            col++;
            row--;
        }
    } else if (direction == SOUTH_EAST && (start_row + length <= N_ROWS)) {
        while (row < (start_row + length)) {
            canvas[row][col] = colour;
            col++;
            row++;
        }
    } else if (direction == SOUTH_WEST && (start_col - length >= MIN_COLS)) {
        while (col > (start_col - length)) {
            canvas[row][col] = colour;
            col--;
            row++;
        }
    } else if (direction == NORTH_WEST && (start_row - length >= -1)) {
        while (col > (start_col - length)) {
            canvas[row][col] = colour;
            col--;
            row--;
        }
    }

    return 0;
}

// Draws a filled in square on the canvas given the
// starting pixel, length, direction and colour
int fill_square(
    int start_row, int start_col, 
    int length, int direction, 
    int canvas[N_ROWS][N_COLS], int colour
) {

    // corrects direction if direction input is more than 360
    direction = correct_dir(direction, length);

    int row = start_row;
    int col = start_col;
    
    if (direction % 90 == 0) {
        draw_line(
            start_row, start_col, length, 
            direction, canvas, colour
        );    
    } else if (
        direction == NORTH_EAST && 
        (start_row - length >= MIN_ROWS)
    ) {
        while (row > (start_row - length)) {
            while (col < (start_col + length)) {
                canvas[row][col] = colour;
                col++;
            }
            row--;
            col = start_col;
        }
    } else if (
        direction == SOUTH_EAST && 
        (start_row + length <= N_ROWS)
    ) {
        while (row < (start_row + length)) {
            while (col < (start_col + length)) {
                canvas[row][col] = colour;
                col++;
            }
            col = start_col;
            row++;
        }
    } else if (
        direction == NORTH_WEST && 
        (start_row - length >= MIN_ROWS)
    ) {
        while (row > (start_row - length)) {
            while (col > (start_col - length)) {
                canvas[row][col] = colour;
                col--;
            }
            row--;
            col = start_col;
        }
    } else if (
        direction == SOUTH_WEST && 
        (start_row + length < N_ROWS)
    ) {
        while (row < (start_row + length)) {
            while (col > (start_col - length)) {
                canvas[row][col] = colour;
                col--;
            }
            row ++;
            col = start_col;
        }
    }
    
    return 0;
}

// Scans for input in order to execute the draw line or draw square 
// function after checking the bounds
void draw_line_or_square (
    int draw_type, int canvas[N_ROWS][N_COLS], 
    int colour 
) {
    
    int start_row, start_col, length, direction;
    scanf(  
        "%d %d %d %d", 
        &start_row, &start_col, &length, &direction
    );
    
    direction = correct_dir(direction, length);
    length = correct_length(length);

    if (within_bounds(start_row, start_col, length)) {
        if (draw_type == DRAW_LINE) {
            draw_line(  
                start_row, start_col, length, 
                direction, canvas, colour
            ); 
        } else if (draw_type == FILL_SQUARE) {
            fill_square(
                start_row, start_col, length, 
                direction, canvas, colour
            ); 
        }
    }
}

// Copies a section from the starting pixel of the campus 
// and pastes this section at the target pixel
void copy_paste(
    int start_row, int start_col, 
    int length, int direction,
    int target_row, int target_col, 
    int canvas[N_ROWS][N_COLS]
) {

    int x = 0;
    int y = 0;
    int i = 0;
    int j = 0;

    // Assigning directional coefficients
    if (direction == NORTH && (start_row - length > MIN_ROWS)) {
        x = -1;
    } else if (direction == NORTH_EAST && (start_row - length > MIN_ROWS)) {
        x = -1;
        y = 1;
    } else if (direction == EAST && (start_col + length <= N_COLS)) {
        y = 1;
    } else if (direction == SOUTH_EAST && (start_row + length <= N_ROWS)) {
        x = 1;
        y = 1;
    } else if (direction == SOUTH && (start_row + length <= N_ROWS)) {
        y = 1;
    } else if (direction == SOUTH_WEST && (start_col - length >= MIN_COLS)) {
        x = 1;
        y = -1;
    } else if (direction == WEST && (start_col - length >= MIN_COLS)) {
        y = -1;
    } else if (direction == NORTH_WEST && (start_row - length >= MIN_ROWS)) {
        x = -1;
        y = -1;
    }

    int storage[N_ROWS][N_COLS];
    if (direction % 90 == 0) {
        // copying a straight line and storing into a storage array
        while (i < length) {
            storage[target_row + x * i][target_col + y * i] = 
            canvas[start_row + x * i][start_col + y * i];
            i++;
        }
        // pasting the canvas with the shape stored in the storage array
        i = 0;
        while (i < length) {
            canvas[target_row + x * i][target_col + y * i] =
            storage[target_row + x * i][target_col + y * i];
            i++;
        }
    } else {
        // copying a diagonal square area and storing into a storage array
        while (i < length) {
            while (j < (target_col + length - target_col)) {
                storage[target_row + x * i][target_col + y * j] =
                canvas[start_row + x * i][start_col + y * j];
                j++;
            }
            j = 0;
            i++;
        }
        // pasting the canvas with the shape stored in the storage array
        i = 0;
        j = 0;
        while (i < length) {
            while (j < (target_col + length - target_col)) {
                canvas[target_row + x * i][target_col + y * j] =
                storage[target_row + x * i][target_col + y * j];
                j++;
            }
            j = 0;
            i++;        
        }
    }  
}

// Scans in input for the copy_paste function and executes the 
// copy_paste function if the given pixels are within bounds of the canvas
void copy_and_paste(int canvas[N_ROWS][N_COLS]) {
    
    int start_row, start_col, length, direction, 
        target_row, target_col;
    scanf(  
        "%d %d %d %d %d %d", 
        &start_row, &start_col, &length, &direction,
        &target_row, &target_col
    );

    // checking that input is within the bounds of the canvas
    if (
        within_bounds(start_row, start_col, length) &&
        target_within_bounds(target_row, target_col, length)
    ) {        
        copy_paste(
            start_row, start_col, length, direction,
            target_row, target_col, canvas
        );
    }
}


// Colours in the shape of an ellipse on the canvas, given
// 2 focus pixels, a length, and a fill type which states if
// the ellipse will be filled in or hollow
void draw_ellipse( 
    int canvas[N_ROWS][N_COLS], int colour
) {

    int focus_1_row, focus_1_col, focus_2_row, focus_2_col, fill;
    double length;
    scanf(  
        "%d %d %d %d %lf %d", 
        &focus_1_row, &focus_1_col, &focus_2_row, 
        &focus_2_col, &length, &fill
    );

    if (fill == 0) {
        hollow_ellipse( 
            focus_1_row, focus_1_col, 
            focus_2_row, focus_2_col, 
            length, canvas, colour
        );
    } else {
        fill_ellipse(
            focus_1_row, focus_1_col, 
            focus_2_row, focus_2_col, 
            length, canvas, colour 
        );
    }
    
    
}

// Fills in an ellipse on the canvas, given
// 2 focus pixels, a length and a colour
void fill_ellipse(  
    int focus_1_row, int focus_1_col, 
    int focus_2_row, int focus_2_col, 
    double length,  int canvas[N_ROWS][N_COLS], 
    int colour
) {
   
    int row = 0;
    int col = 0;

    while (row < N_ROWS) {
        while  (col < N_COLS) { 
            if (
                distance(focus_1_row, focus_1_col, row, col) + 
                distance(row, col, focus_2_row, focus_2_col) <= 2 * length
            ) {
                canvas[row][col] = colour;
            }   
            col++;
        }
        row ++;
        col = 0;
    }
}

// Checks a pixel on the canvas to determine if that pixel belongs
// on the edge of an ellipse, returning TRUE or FALSE accordingly
int ellipse_edge(
    int storage[N_ROWS][N_COLS], int colour, 
    int row, int col
) {
    if (
        row > 0 && row < N_ROWS - 1 &&
        storage[row][col] == colour && 
        (storage[row + 1][col] != colour ||
        storage[row - 1][col] != colour)
    ) {
        return 1;
    } else if ( 
        col > 0 && col < N_COLS - 1 &&
        storage[row][col] == colour && 
        (storage[row][col - 1] != colour ||
        storage[row][col + 1] != colour)
    ) {
        return 1;
    } else {
        return 0;
    }
}

// Draws a hollow ellipse on the canvas, given
// 2 focus pixels, length and a colour
void hollow_ellipse(
    int focus_1_row, int focus_1_col, 
    int focus_2_row, int focus_2_col, 
    double length,  int canvas[N_ROWS][N_COLS], 
    int colour
) {

    int storage[N_ROWS][N_COLS];
    int row = 0;
    int col = 0;

    clearCanvas(storage);

    while (row < N_ROWS) {
        while  (col < N_COLS) { 
            if (
                distance(focus_1_row, focus_1_col, row, col) + 
                distance(row, col, focus_2_row, focus_2_col) <= 2 * length
            ) {
                storage[row][col] = colour;
            }   
            col++;
        }
        row ++;
        col = 0;
    }

    row = 0;
    col = 0;

    while (row < N_ROWS) {
        while  (col < N_COLS) {
            if (
                ellipse_edge(storage, colour, row, col)
            ) {               
                canvas[row][col] = storage[row][col];            
            }
            col++;
        }
        row++;
        col = 0;
    }    
}


