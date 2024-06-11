#define ARROWKEYS_UP 'i'
#define ARROWKEYS_DOWN 'k'
#define ARROWKEYS_LEFT 'j'
#define ARROWKEYS_RIGHT 'l'

//Instant Input Functionality Comes Here

#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>

// Function to disable buffered input
void disableBufferedInput() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON;
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Function to enable buffered input
void enableBufferedInput() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON;
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
#endif

//Notes
/*

Missing:
- Error Checking for malloc and other problematic places
- Player movement and rotation
- Different Types of Objects and Player Reaction to these
- Player block and marker placing

*/



//Inclusions
#include <stdio.h>
#include <stdlib.h> //Required for memory allocation

//enums
enum Direction { //Player Directions
    RIGHT,
    DOWN,
    LEFT,
    UP
};

enum MarkerColor {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    BLACK,
    PINK,
    PURPLE
};

enum PlayerAction { //Player Actions
    STEP,
    STEP_BACK,
    ROTATE_RIGHT,
    ROTATE_LEFT,
    PLACE_MARKER,
    PLACE_BRICK,
    BREAK_MARKER,
    BREAK_BRICK,
    PLACE_WALL,
    BREAK_WALL,
    DO_NOTHING,
    QUIT
};

//structures
typedef struct Field { //Field "type"
    int Height;
    int Width;

    char** ObjectMap;
    char PlayerStanding;
    
    int PlayerCoord_x;
    int PlayerCoord_y;
    enum Direction PlayerDirection;
} Field;

typedef struct PlayerActionsHandler {
    enum PlayerAction action;
    enum MarkerColor color_if_needed;
} PlayerActionsHandler;

//Function declarations
void Field_Init(Field* this, int Height, int Width);
void Field_SetEmpty(Field* this); //Fills all positions with 0s
void Field_PrintOnlyField(Field* this); //Leaves out player location and direction calculation when printing
void Field_Print(Field* this); //Prints the whole
void Field_Redraw(Field* this); //Instead of printing the field, it updates the currently drawn field. NOT IMPLEMENTED
void Field_Free(Field* this); //Frees memory of Field malloc

PlayerActionsHandler Field_GetLiveInput(Field* this); //GETS LIVE USER INPUT; SYSTEM INDEPENDENT;
void Field_PerformPlayerAction(Field* this, PlayerActionsHandler playeractions); //CONTINUE HERE --> ADD MARKERS, WALLS, AND SHOW INFORMATION ON CURRENT BLOCK

void do_character_check(PlayerActionsHandler* pah, char c);
int movement_boundary_check(Field* this, char c);



// Main function start



int main(int argc, char** argv) {
    
    Field myField;
    int width = 0;
    int height = 0;

    if (argc == 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        Field_Init(&myField, height, width);
    } else {
        Field_Init(&myField, 9, 16);
    }

    Field_SetEmpty(&myField);
    Field_Print(&myField);

    PlayerActionsHandler actions;

    while (1==1) {

        actions = Field_GetLiveInput(&myField);
        if (actions.action == QUIT) {
            break;
        }
        Field_PerformPlayerAction(&myField, actions);

        Field_Redraw(&myField);

    }

    Field_Free(&myField);

    /////////
    return 0;
}



// Main function end



//Function definitions
void Field_Init(Field* this, int Height, int Width) {
    this->Height = Height;
    this->Width = Width;

    this->ObjectMap = malloc(Height * sizeof(char*));

    for (int x = 0; x < Height; x++) {
        this->ObjectMap[x] = malloc(Width * sizeof(char));
    }

    this->PlayerCoord_x = 0;
    this->PlayerCoord_y = 0;
    this->PlayerDirection = RIGHT;
    this->PlayerStanding = '0';
}

void Field_SetEmpty(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        for (int x = 0; x < this->Width; x++) {
            this->ObjectMap[i][x] = '0';
        }
    }
}

void Field_PrintOnlyField(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        for (int x = 0; x < this->Width; x++) {
            printf("%c ", this->ObjectMap[i][x]);
        }
        printf("\n");
    }
}

void Field_Print(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        for (int x = 0; x < this->Width; x++) {
            if (this->PlayerCoord_y == i && this->PlayerCoord_x == x) {
                switch (this->PlayerDirection) {
                    case UP:
                        printf("%c ", '^');
                        break;
                    case DOWN:
                        printf("%c ", 'v');
                        break;
                    case LEFT:
                        printf("%c ", '<');
                        break;
                    case RIGHT:
                        printf("%c ", '>');
                        break;
                }

                this->PlayerStanding = this->ObjectMap[i][x];
            } else {
                printf("%c ", this->ObjectMap[i][x]);
            }
        }
        printf("\n");
    }
}

void Field_Free(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        free(this->ObjectMap[i]);
        this->ObjectMap[i] = NULL;
    }

    free(this->ObjectMap);
    this->ObjectMap = NULL;
}

PlayerActionsHandler Field_GetLiveInput(Field* this) {
    
    PlayerActionsHandler playeractions;
    playeractions.color_if_needed = BLACK;
    playeractions.action = DO_NOTHING;

    char c;
    
    #ifdef _WIN32

    while (1) {
        c = _getch();
        if (c != 27 && c != 91) {
            do_character_check(&playeractions, c);
            break;
        }
    }
    #else

    disableBufferedInput();
    while (1) {
        c = getchar();
        if (c != 27 && c != 91) {
            do_character_check(&playeractions, c);
            break;
        }
    }

    enableBufferedInput();

    return playeractions;

    #endif
}

int movement_boundary_check(Field* this, char fob) {
    switch (this->PlayerDirection) {
        case RIGHT:
            if (this->PlayerCoord_x + 1 < this->Width && fob == 'f') {return 1;}
            if (this->PlayerCoord_x - 1 >= 0 && fob == 'b') {return 1;}
            break;
        case LEFT:
            if (this->PlayerCoord_x + -1 >= 0 && fob == 'f') {return 1;}
            if (this->PlayerCoord_x + 1 < this->Width && fob == 'b') {return 1;}
            break;
        case DOWN:
            if (this->PlayerCoord_y + 1 < this->Height && fob == 'f') {return 1;}
            if (this->PlayerCoord_y - 1 >= 0 && fob == 'b') {return 1;}
            break;
        case UP:
            if (this->PlayerCoord_y + 1 < this->Height && fob == 'b') {return 1;}
            if (this->PlayerCoord_y - 1 >= 0 && fob == 'f') {return 1;}
            break;
    }

    return 0;
}

void Field_PerformPlayerAction(Field* this, PlayerActionsHandler playeractions) {
    switch (playeractions.action) {
        case STEP:
            if (movement_boundary_check(this, 'f') == 1) {
                switch (this->PlayerDirection) {
                    case UP:
                        this->PlayerCoord_y--;
                        break;
                    case DOWN:
                        this->PlayerCoord_y++;
                        break;
                    case RIGHT:
                        this->PlayerCoord_x++;
                        break;
                    case LEFT:
                        this->PlayerCoord_x--;
                        break;
                }
            }
            break;
        case STEP_BACK:
            if (movement_boundary_check(this, 'b') == 1) {
                switch (this->PlayerDirection) {
                    case UP:
                        this->PlayerCoord_y++;
                        break;
                    case DOWN:
                        this->PlayerCoord_y--;
                        break;
                    case RIGHT:
                        this->PlayerCoord_x--;
                        break;
                    case LEFT:
                        this->PlayerCoord_x++;
                        break;
                }
            }
            break;
        case ROTATE_RIGHT:
            switch (this->PlayerDirection) {
                case RIGHT:
                    this->PlayerDirection = DOWN;
                    break;
                case DOWN:
                    this->PlayerDirection = LEFT;
                    break;
                case LEFT:
                    this->PlayerDirection = UP;
                    break;
                case UP:
                    this->PlayerDirection = RIGHT;
                    break;
            }
            break;
        case ROTATE_LEFT:
            switch (this->PlayerDirection) {
                case RIGHT:
                    this->PlayerDirection = UP;
                    break;
                case UP:
                    this->PlayerDirection = LEFT;
                    break;
                case LEFT:
                    this->PlayerDirection = DOWN;
                    break;
                case DOWN:
                    this->PlayerDirection = RIGHT;
                    break;
            }
            break;
        default:
            break;
    }
}

void do_character_check(PlayerActionsHandler* pah, char c) {
    switch (c) {
        case ARROWKEYS_UP:
            pah->action = STEP;
            break;
        case ARROWKEYS_DOWN:
            pah->action = STEP_BACK;
            break;
        case ARROWKEYS_RIGHT:
            pah->action = ROTATE_RIGHT;
            break;
        case ARROWKEYS_LEFT:
            pah->action = ROTATE_LEFT;
            break;
        case 'q':
            pah->action = QUIT;
            break;
        default:
            pah->action = DO_NOTHING;
            break;
    }
}

void Field_Redraw(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        printf("\033[A");
    }
    for (int x = 0; x < this->Width-1; x++) {
        printf("\033[D");
    }
    Field_Print(this);
}