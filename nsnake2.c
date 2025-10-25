// compile with the -lncurses tag

#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#define GAME_CLEAR_SCORE 20

#define ID_GROUND 1
#define ID_HEAD 2
#define ID_BODY 3
#define ID_FOOD 4
#define ID_LEAF 5
#define ID_WALL 6
#define ID_COCOON 7

#define ID_ART_BLUE 8
#define ID_ART_GREEN 9
#define ID_ART_RED 10
#define ID_ART_YELLOW 11


struct Snake {
    int alive;
    char move_direction;
    int head_pos[2]; // (y,x)
    int tail_pos[2];
    int length;
};

void Print_Snake_part(char part, int y, int x, char dir){
    // NOTE: coords start from (0,0)
    switch(part) {
        case 'G':
            // ground
            attron(COLOR_PAIR(ID_GROUND));
            mvprintw(y,x,"%c",dir);
            attroff(COLOR_PAIR(ID_GROUND));
            break;
        case 'H':
            // head
            attron(COLOR_PAIR(ID_HEAD));
            mvprintw(y,x,"%c",dir);
            attroff(COLOR_PAIR(ID_HEAD));
            break;
        case 'B':
            // body
            attron(COLOR_PAIR(ID_BODY));
            mvprintw(y,x,"%c",dir);
            attroff(COLOR_PAIR(ID_BODY));
            break;
        default:
            endwin();
		    printf("Your terminal does not support color\n");
    }
}

void SetBorder() {
    attron(COLOR_PAIR(ID_WALL));
    mvprintw(2,5, "WWWWWWWWWWWWWWWWWWWWWWW");
    for(int i = 0; i < 8; i++){
        mvprintw(2+i,5, "W");
        mvprintw(2+i,28, "W");

    }
    mvprintw(10,5, "WWWWWWWWWWWWWWWWWWWWWWWW");

    attroff(COLOR_PAIR(ID_WALL));

    mvprintw(11,5, "move:arrows       quit:Q");
    mvprintw(12,5, "           made by teten");

}

void DIE(struct Snake *s) {
    //   -----'x_x'   <- catapillar died because of your incompetence
    nodelay(stdscr,FALSE);

    mvprintw(4,12, "-----'x_x'");
    
    getch();
    s -> alive = 0;
    refresh();
    return;
}

void Dialogue(int y, int x,int will_clear,char *text) {
    mvprintw(y,x,"%s",text);
    refresh();
    getch();
    int space_length = strlen(text);
    move(y,x);
    if(will_clear){
        for(int i = 0; i < space_length; i++){
            mvprintw(y,x+i," ");
        }
    }
    refresh();

}

void Game_Won(struct Snake *s) {
    nodelay(stdscr,FALSE);
    s -> alive = 0;
    
    getch();
    
    // clear leaf text
    mvprintw(1,8,"                     ");
    refresh();
    Dialogue(0,8,1,"Now he wasnt hungry anymore...");
    Dialogue(0,8,1,"he wasn't a little caterpillar anymore");
    Dialogue(0,8,1,"he was a big fat caterpillar");

    refresh();
    

    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 22; j++){
                chtype ch_data = mvinch(i+3,j+6);
                if((char)(ch_data & A_CHARTEXT) != ' '){
                    chgat(1,A_NORMAL,ID_COCOON,NULL);
                }

        }
    }
    getch();

    Dialogue(0,8,0,"He built a small house");
    Dialogue(1,8,1,"called a cocoon around himself");
    mvprintw(0,8,  "                      ");
    Dialogue(0,8,0,"He stayed inside");
    Dialogue(1,8,1,"for more than two weeks");
    mvprintw(0,8,  "                ");
    getch();

    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 22; j++){
                chtype ch_data = mvinch(i+3,j+6);
                printw(" ");
        }
    }


    //     .-.
    // .-.|  / 
    // \_|_/
    // ===''
    // / | \ 
    // '-'|  \
    //     '-'   
    
    mvprintw(0,8,"He was a beautiful butterfly!");

    attron(COLOR_PAIR(ID_ART_BLUE));
    mvprintw(3,12,".-."); // u w h
    mvprintw(4,11,"|  /"); 
    mvprintw(5,11,"|_/"); // u w h
    mvprintw(7,11,"| \\"); //bwh
    mvprintw(8,11,"|  \\"); // bwh
    mvprintw(9,12,"'-'"); // bwh
    attroff(COLOR_PAIR(ID_ART_BLUE));
    
    
    attron(COLOR_PAIR(ID_ART_GREEN));
    mvprintw(4,8,".-."); // u w t
    mvprintw(5,8," \\_"); // u w t
    mvprintw(7,8," /"); // bwt
    mvprintw(8,8,"'-'"); // bwt
    attroff(COLOR_PAIR(ID_ART_GREEN));

    attron(COLOR_PAIR(ID_ART_YELLOW));
    mvprintw(6,8,"  ===");
    attroff(COLOR_PAIR(ID_ART_YELLOW));
    
    attron(COLOR_PAIR(ID_HEAD));
    mvprintw(6,13,"''");
    attroff(COLOR_PAIR(ID_HEAD));

    attron(A_BOLD);
    attron(COLOR_PAIR(ID_ART_RED));
    mvprintw(6,18,"THE END");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(ID_ART_RED));


    move(1,1);

    refresh();

    getch();

    refresh();
    return;



}

// snack can be in
// x: 6 to 59
// y: 3 to 14

int Spawn_Snack_Position(){
    int x_upper = 27;
    int x_lower = 6;
    int y_upper = 9;
    int y_lower = 3;
    int snack_pos_x = snack_pos_x = (rand() % (x_upper - x_lower + 1)) + x_lower;
    int snack_pos_y = snack_pos_y = (rand() % (y_upper - y_lower + 1)) + y_lower;
    return snack_pos_x * 100 + snack_pos_y;
}

void SpawnSnack(int leaf){
    
    int snack_pos = Spawn_Snack_Position();
    chtype snack_pos_char = mvinch(snack_pos % 100,snack_pos / 100);

    do {
        snack_pos = Spawn_Snack_Position();
        snack_pos_char = mvinch(snack_pos % 100,snack_pos / 100);
    } while(PAIR_NUMBER(snack_pos_char) != ID_GROUND);

    if(leaf){
        attron(COLOR_PAIR(ID_LEAF));
        mvprintw(snack_pos % 100,snack_pos / 100, "O");
        attroff(COLOR_PAIR(ID_LEAF));
    }else{
        attron(COLOR_PAIR(ID_FOOD));
        mvprintw(snack_pos % 100,snack_pos / 100, "O");
        attroff(COLOR_PAIR(ID_FOOD));
    }
}

void Direction_Switch(int coordinate[], char direction){
    switch (direction) {
        case 'R':
            // right
            coordinate[1] += 1;
            break;
        case 'L':
            // left
            coordinate[1] -= 1;
            break;
        case 'U':
            // up
            coordinate[0] -= 1;
            break;
        case 'D':
            // down
            coordinate[0] += 1;
            break;
        default:
            printf("ERROR WRONG CHAR IN MOVE_SNAKE: %c",direction);
    }
}


void Move_Snake(struct Snake *s, char direction) {
    // saves direction on tail so next tail position can be told
    char tail_dir = mvinch(s->tail_pos[0],s->tail_pos[1]) & A_CHARTEXT;
    // old tail position
    int init_tail_pos[] = {s->tail_pos[0],s->tail_pos[1]};

    // print direction on old head slot
    Print_Snake_part('B', s->head_pos[0], s->head_pos[1], direction); 
    // new head pos
    Direction_Switch(s -> head_pos, direction);

    // if new head slot is not blank then quit
    chtype head_char = mvinch(s -> head_pos[0],s -> head_pos[1]);

    // if head new head position is body / wall -> die
    if(PAIR_NUMBER(head_char) == ID_BODY || PAIR_NUMBER(head_char) == ID_WALL){
        DIE(s);
    } 

    // when area is clear
    // make new head slot into a head
    if(direction == 'R' || direction == 'L'){
        Print_Snake_part('H',s->head_pos[0],s->head_pos[1],'\'');
    } else{
        Print_Snake_part('H',s->head_pos[0],s->head_pos[1],'.');
    }


    if(PAIR_NUMBER(head_char) != ID_FOOD && PAIR_NUMBER(head_char) != ID_LEAF){
        // make old tail into a background
        Print_Snake_part('G',init_tail_pos[0],init_tail_pos[1],' ');
        // update tail position by its direction
        Direction_Switch(s -> tail_pos, tail_dir);

    } else {
        // NOM NOM
        s -> length ++;
        int current_score = s -> length - 5;
        int game_clear_score = GAME_CLEAR_SCORE;
        if(current_score ==  game_clear_score - 1){
            mvprintw(1,8,"Sunday: he ate a leaf");

            SpawnSnack(1);
        }else if(current_score >= game_clear_score){
            Game_Won(s);
            return;
        } else{
            SpawnSnack(0);
        }
    }
    
    //make new tail position into a body(just in case when length is 2)

    Print_Snake_part('B',s->tail_pos[0],s->tail_pos[1],mvinch(s->tail_pos[0],s->tail_pos[1]) & A_CHARTEXT);
    move(0,0);
    refresh();
}



int main() {
    int starting_head_position[2] = {8,13}; 
    struct Snake s = {
        .alive = 1,
        .move_direction = 'R',
        .head_pos = {8,13},
        .tail_pos = {8,9},
        .length = 5
    };

    initscr();  // starts curses mode
    int ch;
    keypad(stdscr,TRUE);

    if(has_colors() == FALSE) {
    	endwin();
		printf("Your terminal does not support color\n");
	}
    start_color();
    
    /* example note: when color attribute 1 is on
        text color is red
        background colors is yellow
                    <FOREground> <BACKground>
        init_pair(1, COLOR_RED, COLOR_YELLOW);
    */
    init_pair(ID_GROUND, COLOR_BLACK, COLOR_WHITE); // background    
    init_pair(ID_HEAD, COLOR_WHITE, COLOR_RED); // head
    init_pair(ID_BODY, COLOR_GREEN, COLOR_GREEN); // body
    init_pair(ID_FOOD, COLOR_RED, COLOR_WHITE); // food
    init_pair(ID_LEAF, COLOR_GREEN, COLOR_WHITE); // leaf
    init_pair(ID_WALL, COLOR_BLUE, COLOR_BLUE); // wall
    init_pair(ID_COCOON, COLOR_YELLOW, COLOR_YELLOW); // cocoon

    init_pair(ID_ART_BLUE, COLOR_BLUE, COLOR_WHITE);
    init_pair(ID_ART_GREEN, COLOR_GREEN, COLOR_WHITE);
    init_pair(ID_ART_RED, COLOR_RED, COLOR_WHITE);
    init_pair(ID_ART_YELLOW, COLOR_YELLOW, COLOR_WHITE);

    bkgd(COLOR_PAIR(ID_GROUND));

    raw();      // disable line buffer
    keypad(stdscr, TRUE);   // starts reading Keyboard keys
    noecho();   // doesnt print keyboard input



    for(int i = 1; i < 5; i++) {
        move(starting_head_position[0],starting_head_position[1]-i);
        attron(COLOR_PAIR(ID_BODY));
        printw("R");
        attroff(COLOR_PAIR(ID_BODY));
    }

    move(starting_head_position[0],starting_head_position[1]);
    attron(COLOR_PAIR(ID_HEAD));
    printw("'");
    attroff(COLOR_PAIR(ID_HEAD));
    
    SpawnSnack(0);
    
    
    refresh();  // updates output?
    
    
    
    SetBorder();
    
    mvprintw(3,9, "The Very Hungry");
    mvprintw(4,12, "Caterpillar");
    mvprintw(5,9, "<Press to Start>");
    getch(); //wait for input
    
    mvprintw(3,9, "               ");
    mvprintw(4,12, "           ");
    mvprintw(5,9, "                ");
    
    char current_direction = 'R';
    nodelay(stdscr,TRUE);
    while(s.alive){

        mvprintw(1,0,"Score");
        mvprintw(2,0,"%3d", s.length-5);
        mvprintw(3,0,"%3c", current_direction);
        ch = getch();
        if(ch != ERR){
            switch(ch){
                case KEY_UP:
                    if(current_direction != 'D'){
                        current_direction = 'U';
                    }
                    
                    refresh();
                    break;
                case KEY_DOWN:
                    if(current_direction != 'U'){
                        current_direction = 'D'; 
                    }
                    refresh();
                    break;

                case KEY_LEFT:
                    if(current_direction != 'R'){
                        current_direction = 'L';
                    }
                    refresh();
                    break;

                case KEY_RIGHT:
                    if(current_direction != 'L'){
                        current_direction = 'R';
                    }
                    refresh();
                    break;
                case 'q':
                    endwin();
                    echo();
                    s.alive = 0;
                    break;
            }           
        }


        Move_Snake(&s,current_direction);

        refresh();
        usleep(250000);

    }


    //after death

    getch();    // wait for user input
    echo();     // input will now echo so my text wont be invisible(by duran duran plays) anymore
    endwin();   // ends curses mode

    return 0;
}

// gcc nsnake2.c -lncurses -o snake
