#define c_RESET   "\x1b[0m"
#define c_RED     "\x1b[31m"
#define c_GREEN   "\x1b[32m"
#define c_YELLOW  "\x1b[33m"
#define c_BLUE    "\x1b[34m"
#define c_MAGENTA "\x1b[35m"
#define c_CYAN    "\x1b[36m"
#define c_WHITE   "\x1b[37m"

//Instant Input Functionality Comes Here

#ifdef _WIN32 //windows only

#include <conio.h>
#include <windows.h>

// Function to enable Ansi Escape Codes under Windows for command prompt colors

void enableAnsiEscapeCodes() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

#else //linux / unix only

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
- Fixing of unreadable & repeated code
- Add Player block placing (number system for multiple stacked blocks)
- Add Player Wall placement
- Rewrite of the arguments handler in main() --> also put the arguments handler in functions

*/



//Includes

#include <stdio.h>
#include <stdlib.h> //Required for memory allocation

//enums
typedef enum Mode { //Specifies program launch options
    PLAYERCONTROL_DEFAULT,
    PLAYERCONTROL_CUSTOM_SIZE,
    FILECONTROL_DEFAULT,
    FILECONTROL_CUSTOM_SIZE
} Mode;

typedef enum Direction { //Player Directions
    RIGHT,
    DOWN,
    LEFT,
    UP
} Direction;

typedef enum MarkerColor { //Enum for the colors of a marker obj
    RED,
    GREEN,
    YELLOW,
    BLUE,
    WHITE,
    MAGENTA,
    NO_COLOR
} MarkerColor;

typedef enum PlayerAction { //Player Actions
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
} PlayerAction;

//structures
typedef struct Field { //Field "type"; Everything Field-related; objs and player info
    int Height;
    int Width;

    char** ObjectMap;
    char PlayerStanding;
    
    int PlayerCoord_x;
    int PlayerCoord_y;
    Direction PlayerDirection;
} Field;

typedef struct PlayerActionsHandler { //Passes playeractions and marker color to cmd-interpreter
    PlayerAction action;
    MarkerColor color_if_needed;
} PlayerActionsHandler;

typedef struct Position { //Simple position value holder; has a is_valid flag for position validity testing
    int x;
    int y;
    int is_valid;
} Position;

typedef struct InputAssignment { //CONTINUE HERE; USELESS, SHOULD BE REPLACE BY FILE READING FOR INPUTS
    char forward;
    char backward;
    char turn_left;
    char turn_right;
    char place_marker_red;
    char place_marker_green;
    char place_marker_yellow;
    char place_marker_blue;
    char place_marker_white;
    char place_marker_magenta;
    char quit;
} InputAssignment;

//Function declarations

//Field Generation/Management
void FieldGen_init(Field* this, int Height, int Width); //Initializes a newly created Field (Should always be run before using the Field)
void FieldGen_SetEmpty(Field* this); //Fills all positions with '0's (char type)
void FieldGen_Free(Field* this); //Frees memory of Field memory allocation
Mode GetProgramMode(int argc, char** argv); //Returns a mode set by user arguments
void SetupProgramLaunchOptions(Field* myField, Mode myMode, char** argv); //Sets all necessary option-variables using the Mode obj


//Field Drawing
void FieldDraw_PrintOnlyField(Field* this); //Leaves out player location and direction calculation when printing
void FieldDraw_Print(Field* this); //Prints the screen as well as the player location (Use this almost always)
void FieldDraw_Redraw(Field* this); //Redraws the current Field in place of the old one. Depends on FieldDraw_Print()

//Player Action Management
PlayerActionsHandler PAM_GetLiveInput(Field* this); //GETS LIVE USER INPUT; SYSTEM INDEPENDENT;
void PAM_PerformPlayerAction(Field* this, PlayerActionsHandler playeractions); //CONTINUE HERE --> ADD WALLS AND BLOCKS

//Player Movement Checks
Position PMC_GetInfront(Field* this); //returns the position (in a new type format) infront of the player and returns if the position is valid or not
Position PMC_GetBehind(Field* this); //same as PMC_GetInfront but for the backwards direction duh
int PMC_MovementBoundaryCheck(Field* this, char c);

//User-Input Management
void UIM_CheckCharInputs(PlayerActionsHandler* pah, char c);

//Deprecated
void setup_inputs(InputAssignment* inputassignment);
void basic_inputs(InputAssignment* inputassignment);

//Global Variables Assignment
InputAssignment user_inputs;



// Main function start



int main(int argc, char** argv) {

    //Enables AnsiEscapeCodes if the _WIN32 API is found
    #ifdef _WIN32
    enableAnsiEscapeCodes();
    #endif
    
    Field myField;
    Mode myMode;
    //int width = 0;
    //int height = 0;

    //Sets the launch options to the user-specified values
    myMode = GetProgramMode(argc, argv);
    SetupProgramLaunchOptions(&myField, myMode, argv); //ACTUALLY CONTINUE HERE TBH

    /*switch (argc) {
        case 3:
            width = atoi(argv[1]);
            height = atoi(argv[2]);
            FieldGen_init(&myField, height, width);
            myMode = PLAYERCONTROL;
            basic_inputs(&user_inputs);
            break;
        case 4:
            width = atoi(argv[1]);
            height = atoi(argv[2]);
            FieldGen_init(&myField, height, width);

            // EXPAND HERE

            myMode = FILECONTROL;
            basic_inputs(&user_inputs);
            break;
        case 1:
            FieldGen_init(&myField, 9, 16);
            myMode = PLAYERCONTROL;
            basic_inputs(&user_inputs);
            break;
        case 2:
            FieldGen_init(&myField, 9, 16);
            myMode = PLAYERCONTROL;
            setup_inputs(&user_inputs);
            break;
        case 6:
            width = atoi(argv[1]);
            height = atoi(argv[2]);
            FieldGen_init(&myField, height, width);

            setup_inputs(&user_inputs);

            myMode = FILECONTROL;
            break;
        case 5:
            width = atoi(argv[1]);
            height = atoi(argv[2]);
            FieldGen_init(&myField, height, width);

            setup_inputs(&user_inputs);

            myMode = PLAYERCONTROL;
            break;
        default:
            printf("Invalid usage of program, type: robotkarolrewrite <width> <height> [optional file name for some_program.rkr]\n Loading Standard Field...\n");
            FieldGen_init(&myField, 9, 16);
            myMode = FILECONTROL;
            basic_inputs(&user_inputs);
            break;
    }
    */

    FieldGen_SetEmpty(&myField); //Temporary
    printf("\n");
    FieldDraw_Print(&myField); //Initial print of the screen

    PlayerActionsHandler actions;

    while (1==1) {

        actions = PAM_GetLiveInput(&myField);

        if (actions.action == QUIT) {
            break;
        }

        PAM_PerformPlayerAction(&myField, actions);
        FieldDraw_Redraw(&myField);
    }

    FieldGen_Free(&myField); //"Deletes" the Field myField obj
    /////////
    return 0;
}



// Main function end



//Function definitions
Mode GetProgramMode(int argc, char** argv) {
    Mode return_mode = PLAYERCONTROL_DEFAULT;
    switch (argc) {
        case 1:
            break;
        case 2:
            if (argv[1] != "-d") {printf("Error! \"2\" is not a valid argument!", argv[1]); break;}
            break;
        case 3:
            if (argv[1] != "-f") {printf("Error! \"3\" is not a valid argument!", argv[1]); break;}
            return_mode = FILECONTROL_DEFAULT; break;
        case 4:
            if (argv[1] != "-c\0") {printf("Error! \"4\" is not a valid argument!", argv[1]); break;}
            for (int x = 0; x != '\0' && x != '\r' && x != '\n'; x++) {if (argv[2][x] < 48 || argv[2][x] > 57) {printf("Error! \"%s\" is not a valid argument!", argv[2]); break;}}
            for (int x = 0; x != '\0' && x != '\r' && x != '\n'; x++) {if (argv[3][x] < 48 || argv[3][x] > 57) {printf("Error! \"%s\" is not a valid argument!", argv[3]); break;}}
            return_mode = PLAYERCONTROL_CUSTOM_SIZE; break;
        case 5:
            if (argv[1] != "-fc") {printf("Error! \"5\" is not a valid argument!", argv[1]); break;}
            for (int x = 0; x != '\0' && x != '\r' && x != '\n'; x++) {if (argv[3][x] < 48 || argv[3][x] > 57) {printf("Error! \"%s\" is not a valid argument!", argv[3]); break;}}
            for (int x = 0; x != '\0' && x != '\r' && x != '\n'; x++) {if (argv[4][x] < 48 || argv[4][x] > 57) {printf("Error! \"%s\" is not a valid argument!", argv[4]); break;}} //FIX THIS
            return_mode = PLAYERCONTROL_CUSTOM_SIZE; break;
    }

    return return_mode;
}

void SetupProgramLaunchOptions(Field* myField, Mode myMode, char** argv) {
    switch (myMode) {
        case PLAYERCONTROL_DEFAULT:
            FieldGen_init(myField, 9, 16);
            basic_inputs(&user_inputs);
            break;
        case PLAYERCONTROL_CUSTOM_SIZE:
            FieldGen_init(myField, atoi(argv[3]), atoi(argv[2]));
            basic_inputs(&user_inputs);
            break;
        case FILECONTROL_DEFAULT:
            //NOTHING CURRENTLY; SHOULD LOAD FILE EVENTUALLY

            FieldGen_init(myField, 9, 16);
            basic_inputs(&user_inputs);
            break;
        case FILECONTROL_CUSTOM_SIZE:
            //NOTHING CURRENTLY; SHOULD LOAD CUSTOM SIZED FIELD AND FILE EVENTUALLY

            FieldGen_init(myField, atoi(argv[4]), atoi(argv[3]));
            basic_inputs(&user_inputs);
            break;
    }
}

void setup_inputs(InputAssignment* inputassignment) {
    printf("Enter keybind for \"forward\": ");
    scanf(" %c", &inputassignment->forward);
    printf("Enter keybind for \"backward\": ");
    scanf(" %c", &inputassignment->backward);
    printf("Enter keybind for \"turn_left\": ");
    scanf(" %c", &inputassignment->turn_left);
    printf("Enter keybind for \"turn_right\": ");
    scanf(" %c", &inputassignment->turn_right);
    printf("Enter keybind for \"red\": ");
    scanf(" %c", &inputassignment->place_marker_red);
    printf("Enter keybind for \"green\": ");
    scanf(" %c", &inputassignment->place_marker_green);
    printf("Enter keybind for \"yellow\": ");
    scanf(" %c", &inputassignment->place_marker_yellow);
    printf("Enter keybind for \"blue\": ");
    scanf(" %c", &inputassignment->place_marker_blue);
    printf("Enter keybind for \"white\": ");
    scanf(" %c", &inputassignment->place_marker_white);
    printf("Enter keybind for \"magenta\": ");
    scanf(" %c", &inputassignment->place_marker_magenta);
    printf("Enter keybind for \"quit\": ");
    scanf(" %c", &inputassignment->quit);
}

void basic_inputs(InputAssignment* inputassignment) {
    inputassignment->forward = 'i';
    inputassignment->backward = 'k';
    inputassignment->turn_left = 'j';
    inputassignment->turn_right = 'l';
    inputassignment->place_marker_red = 'r';
    inputassignment->place_marker_green = 'g';
    inputassignment->place_marker_yellow = 'y';
    inputassignment->place_marker_blue = 'b';
    inputassignment->place_marker_white = 'w';
    inputassignment->place_marker_magenta = 'm';
    inputassignment->quit = 'q';
}

void FieldGen_init(Field* this, int Height, int Width) {
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

void FieldGen_SetEmpty(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        for (int x = 0; x < this->Width; x++) {
            this->ObjectMap[i][x] = '0';
        }
    }
}

void FieldDraw_PrintOnlyField(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        for (int x = 0; x < this->Width; x++) {
            printf("%c ", this->ObjectMap[i][x]);
        }
        printf("\n");
    }
}

void FieldDraw_Print(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        for (int x = 0; x < this->Width; x++) {
            if (this->PlayerCoord_y == i && this->PlayerCoord_x == x) {
                switch (this->PlayerDirection) {
                    case UP:
                        printf(c_CYAN "%c " c_RESET, '^');
                        break;
                    case DOWN:
                        printf(c_CYAN "%c " c_RESET, 'v');
                        break;
                    case LEFT:
                        printf(c_CYAN "%c " c_RESET, '<');
                        break;
                    case RIGHT:
                        printf(c_CYAN "%c " c_RESET, '>');
                        break;
                }

                this->PlayerStanding = this->ObjectMap[i][x];
            } else {
                switch (this->ObjectMap[i][x]) {
                    case 'R':
                        printf(c_RED "%c " c_RESET, 'R');
                        break;
                    case 'G':
                        printf(c_GREEN "%c " c_RESET, 'G');
                        break;
                    case 'Y':
                        printf(c_YELLOW "%c " c_RESET, 'Y');
                        break;
                    case 'B':
                        printf(c_BLUE "%c " c_RESET, 'B');
                        break;
                    case 'M':
                        printf(c_MAGENTA "%c " c_RESET, 'M');
                        break;
                    case 'W':
                        printf(c_WHITE "%c " c_RESET, 'W');
                        break;
                    default:
                        printf("%c ", this->ObjectMap[i][x]);
                }
                if (i == 0 && x == this->Width-1) {
                    printf("  Under Player: \'" c_CYAN "%c" c_RESET "\'", this->PlayerStanding);
                }
            }
        }
        printf("\n");
    }
}

void FieldGen_Free(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        free(this->ObjectMap[i]);
        this->ObjectMap[i] = NULL;
    }

    free(this->ObjectMap);
    this->ObjectMap = NULL;
}

PlayerActionsHandler PAM_GetLiveInput(Field* this) {
    
    PlayerActionsHandler playeractions;
    playeractions.color_if_needed = NO_COLOR;
    playeractions.action = DO_NOTHING;

    char c;
    
    #ifdef _WIN32

    while (1) {
        c = _getch();
        if (c != 27 && c != 91) {
            UIM_CheckCharInputs(&playeractions, c);
            break;
        }
    }

    return playeractions;
    
    #else

    disableBufferedInput();
    while (1) {
        c = getchar();
        if (c != 27 && c != 91) {
            UIM_CheckCharInputs(&playeractions, c);
            break;
        }
    }

    enableBufferedInput();

    return playeractions;

    #endif
}

int PMC_MovementBoundaryCheck(Field* this, char fob) {
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

void PAM_PerformPlayerAction(Field* this, PlayerActionsHandler playeractions) {
    switch (playeractions.action) {
        case STEP:
            if (PMC_MovementBoundaryCheck(this, 'f') == 1) {
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
            if (PMC_MovementBoundaryCheck(this, 'b') == 1) {
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
        case PLACE_MARKER:
            switch (playeractions.color_if_needed) {
                case RED:
                    this->ObjectMap[this->PlayerCoord_y][this->PlayerCoord_x] = 'R';
                    break;
                case GREEN:
                    this->ObjectMap[this->PlayerCoord_y][this->PlayerCoord_x] = 'G';
                    break;
                case YELLOW:
                    this->ObjectMap[this->PlayerCoord_y][this->PlayerCoord_x] = 'Y';
                    break;
                case BLUE:
                    this->ObjectMap[this->PlayerCoord_y][this->PlayerCoord_x] = 'B';
                    break;
                case WHITE:
                    this->ObjectMap[this->PlayerCoord_y][this->PlayerCoord_x] = 'W';
                    break;
                case MAGENTA:
                    this->ObjectMap[this->PlayerCoord_y][this->PlayerCoord_x] = 'M';
                    break;
            }
            break;
        default:
            break;
    }
}

void UIM_CheckCharInputs(PlayerActionsHandler* pah, char c) {
    /*switch (c) {
        case user_inputs.forward:
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
        case 'm':
            pah->action = PLACE_MARKER;
            pah->color_if_needed = MAGENTA;
            break;
        case 'w':
            pah->action = PLACE_MARKER;
            pah->color_if_needed = WHITE;
            break;
        case 'b':
            pah->action = PLACE_MARKER;
            pah->color_if_needed = BLUE;
            break;
        case 'y':
            pah->action = PLACE_MARKER;
            pah->color_if_needed = YELLOW;
            break;
        case 'g':
            pah->action = PLACE_MARKER;
            pah->color_if_needed = GREEN;
            break;
        case 'r':
            pah->action = PLACE_MARKER;
            pah->color_if_needed = RED;
            break;
        default:
            pah->action = DO_NOTHING;
            break;
    }*/

    if (c == user_inputs.forward) {
        pah->action = STEP;
    } else if (c == user_inputs.backward) {
        pah->action = STEP_BACK;
    } else if (c == user_inputs.turn_left) {
        pah->action = ROTATE_LEFT;
    } else if (c == user_inputs.turn_right) {
        pah->action = ROTATE_RIGHT;
    } else if (c == user_inputs.quit) {
        pah->action = QUIT;
    } else if (c == user_inputs.place_marker_red) {
        pah->action = PLACE_MARKER;
        pah->color_if_needed = RED;
    } else if (c == user_inputs.place_marker_green) {
        pah->action = PLACE_MARKER;
        pah->color_if_needed = GREEN;
    } else if (c == user_inputs.place_marker_yellow) {
        pah->action = PLACE_MARKER;
        pah->color_if_needed = YELLOW;
    } else if (c == user_inputs.place_marker_blue) {
        pah->action = PLACE_MARKER;
        pah->color_if_needed = BLUE;
    } else if (c == user_inputs.place_marker_white) {
        pah->action = PLACE_MARKER;
        pah->color_if_needed = WHITE;
    } else if (c == user_inputs.place_marker_magenta) {
        pah->action = PLACE_MARKER;
        pah->color_if_needed = MAGENTA;
    } else {
        pah->action = DO_NOTHING;
    }
}

void FieldDraw_Redraw(Field* this) {
    for (int i = 0; i < this->Height; i++) {
        printf("\033[A");
    }
    for (int x = 0; x < this->Width-1; x++) {
        printf("\033[D");
    }
    FieldDraw_Print(this);
}

Position PMC_GetInfront(Field* this) {
    Position return_position;

    switch (this->PlayerDirection) {
        case RIGHT:
            return_position.x = this->PlayerCoord_x+1;
            return_position.y = this->PlayerCoord_y;
            if (this->PlayerCoord_x+1 < this->Width) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
        case LEFT:
            return_position.x = this->PlayerCoord_x-1;
            return_position.y = this->PlayerCoord_y;
            if (this->PlayerCoord_x-1 >= 0) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
        case DOWN:
            return_position.x = this->PlayerCoord_x;
            return_position.y = this->PlayerCoord_y+1;
            if (this->PlayerCoord_y+1 < this->Height) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
        case UP:
            return_position.x = this->PlayerCoord_x;
            return_position.y = this->PlayerCoord_y-1;
            if (this->PlayerCoord_y-1 >= 0) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
    }
}

Position PMC_GetBehind(Field* this) {
    Position return_position;

    switch (this->PlayerDirection) {
        case LEFT:
            return_position.x = this->PlayerCoord_x+1;
            return_position.y = this->PlayerCoord_y;
            if (this->PlayerCoord_x+1 < this->Width) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
        case RIGHT:
            return_position.x = this->PlayerCoord_x-1;
            return_position.y = this->PlayerCoord_y;
            if (this->PlayerCoord_x-1 >= 0) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
        case UP:
            return_position.x = this->PlayerCoord_x;
            return_position.y = this->PlayerCoord_y+1;
            if (this->PlayerCoord_y+1 < this->Height) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
        case DOWN:
            return_position.x = this->PlayerCoord_x;
            return_position.y = this->PlayerCoord_y-1;
            if (this->PlayerCoord_y-1 >= 0) {return_position.is_valid = 1;}
            else {return_position.is_valid = 0;}
            return return_position;
    }
}