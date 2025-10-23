// conpile with the -lncurses tag

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define ID_GROUND 1
#define ID_HEAD 2
#define ID_BODY 3
#define ID_FOOD 4
#define ID_WALL 5



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

void SetBorder(){
    attron(COLOR_PAIR(ID_WALL));
    mvprintw(2,5, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
    for(int i = 0; i < 10; i++){
        mvprintw(2+i,5, "W");
        mvprintw(2+i,33, "W");

    }
    mvprintw(12,5, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWW");

    attroff(COLOR_PAIR(ID_WALL));
}

// snack can be in
// x: 6 to 59
// y: 3 to 14

int Spawn_Snack_Position(){
    int x_upper = 32;
    int x_lower = 6;
    int y_upper = 11;
    int y_lower = 3;
    int snack_pos_x = snack_pos_x = (rand() % (x_upper - x_lower + 1)) + x_lower;
    int snack_pos_y = snack_pos_y = (rand() % (y_upper - y_lower + 1)) + y_lower;
    return snack_pos_x * 100 + snack_pos_y;
}

void SpawnSnack(){
    
    int snack_pos = Spawn_Snack_Position();
    chtype snack_pos_char = mvinch(snack_pos % 100,snack_pos / 100);

    do {
        snack_pos = Spawn_Snack_Position();
        snack_pos_char = mvinch(snack_pos % 100,snack_pos / 100);
    } while(PAIR_NUMBER(snack_pos_char) != ID_GROUND);

    attron(COLOR_PAIR(ID_FOOD));
    mvprintw(snack_pos % 100,snack_pos / 100, "O");
    attroff(COLOR_PAIR(ID_FOOD));
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
    mvprintw(0,0,"%d",PAIR_NUMBER(head_char));

    // if head new head position is body / wall -> die
    if(PAIR_NUMBER(head_char) == ID_BODY || PAIR_NUMBER(head_char) == ID_WALL){
        //if it isnt empty ground
        mvprintw(1,0,"DEAD");
        s -> alive = 0;
        refresh();
        return;
    } 

    // when area is clear
    // make new head slot into a head
    if(direction == 'R' || direction == 'L'){
        Print_Snake_part('H',s->head_pos[0],s->head_pos[1],'\'');
    } else{
        Print_Snake_part('H',s->head_pos[0],s->head_pos[1],'.');
    }


    if(PAIR_NUMBER(head_char) != ID_FOOD){
        // make old tail into a background
        Print_Snake_part('G',init_tail_pos[0],init_tail_pos[1],' ');
        // update tail position by its direction
        Direction_Switch(s -> tail_pos, tail_dir);

    } else {
        // NOM NOM
        s -> length ++;
        SpawnSnack();
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
    
    /*when color attribute 1 is on
    text color is red
    background colors is yellow
    
    init_pair(1, COLOR_RED, COLOR_YELLOW);
    */
    init_pair(ID_GROUND, COLOR_BLACK, COLOR_WHITE); // background    
    init_pair(ID_HEAD, COLOR_WHITE, COLOR_RED); // head
    init_pair(ID_BODY, COLOR_GREEN, COLOR_GREEN); // body
    init_pair(ID_FOOD, COLOR_RED, COLOR_WHITE); // food
    init_pair(ID_WALL, COLOR_BLUE, COLOR_BLUE); // wall

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
    

    SpawnSnack();
    // move(7,7);
    // attron(COLOR_PAIR(ID_FOOD));
    // printw("O");
    // attroff(COLOR_PAIR(ID_FOOD));
    
    
    refresh();  // updates output?
    
    
    
    SetBorder();
    
    
    mvprintw(5,7, "<Press Any Key to Start>");
    getch(); //wait for input
    mvprintw(5,7, "                        ");
    
    char current_direction = 'R';
    nodelay(stdscr,TRUE);
    while(s.alive){
        mvprintw(2,0,"%d", s.alive);
        mvprintw(4,0,"%c", current_direction);
        mvprintw(5,0,"%d", s.length-5);
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

    echo();     // input will now echo so my text wont be invisible(by duran duran plays) anymore
    getch();    // wait for user input
    endwin();   // ends curses mode

    return 0;
}

// gcc nsnake2.c -lcurses -o snake
