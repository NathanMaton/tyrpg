#include <ncurses.h> // used for screen
#include <cstdlib> // for rand
#include <iostream> // for rand seed (system time)
#include <fstream> // used for working with save file

struct creature { // used for both the player and monsters currently. initialise their attributes
	int hp, dmg, lvl, lvlhp; // lvlhp = max hp
	const char* name; // name is a char array pointer thing, ~a string
};

// initialise player and mobs
creature player;
creature wolf;
creature bear;
creature ogre;
creature blueDragon;
creature demigod;
creature blackDragon;
creature mHold; // used for fight(). holds monster that will be in a particular fight i think

// call functions to initialise them and to help me see what order they are in/should be in
void init();
void initWithoutSave(); // init when no save file is present
void load(); // load save
void mInit(); // monster init
void cInit(); // color init

void town();
void monsPrep(); // rand to select which mob i fight..
void fight();
void inn(); // heal
void lvlup();

void save();

// oook
void init() 
{
	std::ifstream test("save");        
	test ? load() : initWithoutSave(); // if save exists load it, otherwise init fresh values.

	mInit(); // mobs

	initscr(); // ncurses
	noecho();

	cInit(); // colors

	int row, col; // ncurses/screen config
	getmaxyx(stdscr,row,col); // get the number of rows and columns
	scrollok(stdscr,true); // allow scrolling

	mvprintw(row-2,0,"Welcome, adventurer.\nThere is a fearsome dragon in the land, and it is up to you to defeat him!\n");		
	refresh(); // refresh screen
	getch(); // pause for input (any input works)
	town(); // load town
}

void initWithoutSave()
{
	player.hp = 100; // set values
	player.dmg = 10;
	player.lvl = 1;
	player.lvlhp = player.hp;
}

void load() // load save
{
	int count = 0; // local vars to help out here
	int a;
	std::ifstream myfile ("save"); // find/open file named 'save' in current dir
	while (!myfile.eof()) { // repeat until end of file
		myfile >> a;
		if (count == 0 && myfile.peek() == '\n') {
			myfile >> player.hp; // reads hp from file to active game var player.hp
			count++; // and move on to next var, etc
		}
		if (count == 1 && myfile.peek() == '\n') { // count = 1? read this var, we're on the second line of the save file now
			myfile >> player.dmg;
			count++;
		}
		if (count == 2 && myfile.peek() == '\n') {
			myfile >> player.lvl;
			count++;
		}
		if (count == 3 && myfile.peek() == '\n') {
			myfile >> player.lvlhp; // .. should i rename lvlhp to maxhp? might make more sense
			count++;
		}
	} // end while
} // end load()

void mInit()
{
	wolf.hp = 50;
	wolf.dmg = 5;
	wolf.name = "wolf";

	bear.hp = 80;
	bear.dmg = 8;
	bear.name = "bear";
	
	ogre.hp = 100;
	ogre.dmg = 10;
	ogre.name = "ogre";

	blueDragon.hp = 150;
	blueDragon.dmg = 15;
	blueDragon.name = "blue dragon";

	demigod.hp = 500;
	demigod.dmg = 50;
	demigod.name = "demi god";

	blackDragon.hp = 30000;
	blackDragon.dmg = 300;
	blackDragon.name = "black dragon";
}

void cInit()
{
	start_color();
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	//init_pair(4, COLOR_BLUE, COLOR_BLACK); //// in case i ever want these 2 colors
	//init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
}

void town() {
	printw("\nYou are in a town.\n"); 
	refresh(); 
	getch();
	printw("Type e to try your luck in the wilderness outside this town, r to sleep in the inn, or q to quit: \n"); 
	refresh();
	loop: // used for gotos, not actually a loop unless called by a goto
	switch (getch()) {
	case 'e':
		monsPrep();
		printw("\nYou encounter a %s.\n",mHold.name); 
		refresh(); 
		fight();
		break;
	case 'r':
		inn();
		break;
	case 'q':
		printw("\n\nBye!\n\n");
		refresh();
		endwin();
		save();
		break;
	default:
		goto loop;
	}
}

void monsPrep() {
	srand (time(NULL));
	switch(rand() % 6) {
	case 0:
		mHold = wolf;
		break;
	case 1:
		mHold = bear;
		break;
	case 2:
		mHold = ogre;
		break;
	case 3:
		mHold = blueDragon;
		break;
	case 4:
		mHold = demigod;
		break;
	case 5:
		mHold = blackDragon;
		break;
	}
}

void fight() {
	getch();
	printw("\nThe %s attacks and deals ",mHold.name);
	attron(COLOR_PAIR(2));
	printw("%d",mHold.dmg);
	attroff(COLOR_PAIR(2));
	printw(" damage.\n");
	refresh();
	player.hp -= mHold.dmg;
	if (player.hp <= 0){
		printw("You die! Game over :(\n\n");
		refresh();
		getch();
		endwin();
	}
	else{
		printw("You are now at ");
		attron(COLOR_PAIR(3));
		printw("%d",player.hp);
		attroff(COLOR_PAIR(3));
		printw(" hp.\n\n"); 
		refresh();
		printw("Type f to fight and r to run: \n"); 
		refresh();
		loop:	
		switch(getch()) {
		case 'r':		
			printw("\nYou escape successfully and return to the town.\n"); 
			refresh();
			town();
			break;
		case 'f':
			printw("\nYou attack and deal ");
			attron(COLOR_PAIR(1));
			printw("%d",player.dmg);
			attroff(COLOR_PAIR(1));
			printw(" damage.\n"); 
			refresh();
			mHold.hp -= player.dmg;
			if (mHold.hp <= 0) {
				// if (mHold.name == "blackDragon") //// hmm
				//	win();
				// else {
					printw("You have defeated the %s!\n",mHold.name);
					refresh();
					getch();
					lvlup();
					getch();
					printw("You return to town, victorious.\n");
					refresh();
					getch();
					town();
				// }
			}
			else{
				printw("The %s is now at ",mHold.name);
				attron(COLOR_PAIR(4));
				printw("%d",mHold.hp);
				attroff(COLOR_PAIR(4));
				printw(" hp.\n"); 
				refresh();
				fight();
			}
			break;
		default:
			goto loop;
		}
	}
}

void inn() {
	player.hp = player.lvlhp;
	printw("\nYou sleep well and wake up the next morning feeling rejuvenated.\n");
	getch();
	//printw("You return to town.\n");
	//getch();
	town();
}

void lvlup() {
	if (player.lvl < 50) {
		player.hp = player.hp + player.lvlhp * 1.1 - player.lvlhp; // player gains a small amount of hp
		player.lvlhp *= 1.1;
		player.dmg *= 1.1;
	}
	else {
		player.hp = player.hp + player.lvlhp * 1.01 - player.lvlhp; // player gains a small amount of hp
		player.lvlhp *= 1.01;
		player.dmg *= 1.01;
	}
	player.lvl ++;
	printw("\n\nCongrats! You are now level %d!",player.lvl);
	printw("\nYour max hp has risen to %d!",player.lvlhp);
	printw("\nYour strength has risen to %d!\n\n",player.dmg);
}

void save()
{
	std::ofstream myfile ("save");
	myfile << 1 << '\n' << player.hp << '\n' << player.dmg << '\n' << player.lvl << '\n' << player.lvlhp << '\n' ;
}

int main() {
	init();
}
