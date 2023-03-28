#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses/curses.h>
#include <time.h>

int n_choices = 2, width, height;
double  wpm, accuracy;
time_t start_timer, end_timer;

void print_menu(WINDOW *menu_win, int highlight);
void start_test();

void start(){
	int  c, highlight = 1, choice = 0;
	initscr();
	clear();
	noecho();
	cbreak();
	getmaxyx(stdscr, height, width);
	refresh();
	curs_set(0);

	// WINDOW* outer_win = newwin(height, width, 0, 0);
	WINDOW* menu_win = newwin(height, width, 0, 0);

	keypad(menu_win, TRUE);

	print_menu(menu_win, highlight);
	while(1){
		c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else 
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
		}
		// refresh();
		if(choice != 0){
			break;
		}
		print_menu(menu_win, highlight);
	}

	clear();
	if(choice == 1){
		refresh();
		start_test();
	}

	else{
		endwin();
	}

}

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	
	char *choices[] = {"Start Test", "Quit"}; 

	wattron(menu_win, A_UNDERLINE | A_BOLD);
	mvwprintw(menu_win, 4, (width/2) - 5, "TYPING TEST");
	wattroff(menu_win, A_UNDERLINE | A_BOLD);

	x = 15;
	y = 10;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1) /* Highlight the present choice */
		{	
			wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

void calculate(int words_typed, int mistakes){
	// int time = (int)difftime(end_timer, start_timer);
	double time = difftime(end_timer, start_timer);
	time = time/60;
	wpm = (words_typed - mistakes)/time;
	// accuracy = ((words_typed - mistakes)/words_typed)*100;
}

void center_screen(int words_typed, int mistakes){
	char *choices[] = {"Restart", "Quit"}; 
	int c;

	int text_win_width = (width/1.5) - 50;
	int text_win_height = (height/2) - 5;
	int highlight = 1, choice = 0;

	init_pair(5,COLOR_WHITE, COLOR_BLACK);

	refresh:
	WINDOW* text_wi = newwin(text_win_height, text_win_width, height/3, width/6 + 30);
	wbkgd(text_wi, COLOR_PAIR(5));
	keypad(text_wi, TRUE);
	box(text_wi, 0, 0);
	if(words_typed == 0){
		wattron(text_wi, A_UNDERLINE | A_BOLD);
		mvwprintw(text_wi, text_win_height/4, text_win_width/2 - 3, "PAUSED");
		wattroff(text_wi, A_UNDERLINE | A_BOLD);
	}
	else{
		calculate(words_typed, mistakes);
		wattron(text_wi, A_UNDERLINE | A_BOLD);
		mvwprintw(text_wi, text_win_height/4, text_win_width/2 - 7, "YOUR WPM = %.f", wpm);
		// mvwprintw(text_wi, text_win_height/4, text_win_width/2 - 7, "%f %d", difftime(end_timer, start_timer), (int)difftime(end_timer, start_timer));
		wattroff(text_wi, A_UNDERLINE | A_BOLD);
	}
	int y = text_win_height/2;
	for(int i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1) /* Highlight the present choice */
		{	
			wattron(text_wi, A_REVERSE); 
			mvwprintw(text_wi, y, text_win_width/2 - 4, "%s", choices[i]);
			wattroff(text_wi, A_REVERSE);
		}
		else	
			mvwprintw(text_wi, y, text_win_width/2 - 4, "%s", choices[i]);
		++y;
	}

	while(1){
		c = wgetch(text_wi);
		y = text_win_height/2;

		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;

				goto refresh;

				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else 
					++highlight;

				goto refresh;
				break;
			case 10:
				choice = highlight;
				break;
		}
		// refresh();
		if(choice != 0){
			break;
		}

		for(int i = 0; i < n_choices; ++i)
		{	if(highlight == i + 1) /* Highlight the present choice */
			{	
				wattron(text_wi, A_REVERSE); 
				mvwprintw(text_wi, y, text_win_width/2 - 4, "%s", choices[i]);
				wattroff(text_wi, A_REVERSE);
			}
			else	
				mvwprintw(text_wi, y, text_win_width/2 - 4, "%s", choices[i]);
			++y;
		}
		wrefresh(text_wi);
	}
	if(choice == 1){
		refresh();
		start_test();
	}

	else{
		endwin();
	}
}

void start_test(){
	char word[50], word_typed[100];
	int word_count = 0, done_pos = 0, c, i, j, start_pos, mistakes, flag = 1;
	int text_win_width = (width/1.5);
	int text_win_height = (height/2) - 5;

	srand(time(0));
	int num = (rand() % 7) + 1;
	char *name;
	sprintf(name, "%d", num);

	// noecho();
	// cbreak();
	// fclose(fw);
	FILE* fc = fopen("store.txt", "w");
	fclose(fc);

	next_word:
	char word_typing[50] = "\0";
	memset(word_typing, 0, 50);
	FILE* fw = fopen("store.txt", "a+");

	refresh:
	word_count = 0;
	mistakes = 0;
	WINDOW* outer_win = newwin(height, width, 0, 0);
	WINDOW* text_win = newwin(text_win_height, text_win_width, height/2, width/6);
	WINDOW* type_win = newwin(3, text_win_width-5, height/2 + text_win_height - 4, width/6 + 2);
	mvwprintw(type_win, 1, 1, "%s ", word_typing);	

	FILE* f = fopen(name, "r");

	start_color();
	init_color(COLOR_GREEN, 0, 700, 0);
	init_color(COLOR_RED, 800, 0, 0);
	init_pair(1,COLOR_BLACK, COLOR_WHITE);
	init_pair(2,COLOR_GREEN, COLOR_WHITE);
	init_pair(3,COLOR_RED, COLOR_WHITE);
	wbkgd(text_win, COLOR_PAIR(1));
	wbkgd(type_win, COLOR_PAIR(1));

	box(outer_win, 0, 0);
	box(text_win, 0, 0);
	box(type_win, 0, 0);
	wattron(outer_win, A_UNDERLINE | A_BOLD);
	mvwprintw(outer_win, 4, (width/2) - 5, "TYPING TEST");
	wattroff(outer_win, A_UNDERLINE | A_BOLD);

	start_pos = i = j = -1;
	for(i = 1;  i <= text_win_height - 1 && !feof(f); i++){
		j = start_pos + 2;
		for(; j <= text_win_width - 1 && !feof(f); j++){
			fscanf(f, "%s", word);
			fscanf(fw, "%s", word_typed);
			if (strlen(word) < text_win_width - 1 - j){
				if(word_count == done_pos){
					wattron(text_win, A_UNDERLINE | A_BOLD);
					mvwprintw(text_win, i,j, "%s", word);
					wattroff(text_win, A_UNDERLINE | A_BOLD);
				}
				else if(!feof(fw)){
					if(strcmp(word_typed, word) == 0){
						// color_set(2, NULL);
						// wbkgd(text_win, COLOR_PAIR(1));
						wattron(text_win, COLOR_PAIR(2));
						mvwprintw(text_win, i,j, "%s", word);
						wattroff(text_win, COLOR_PAIR(2));

					}
					else{
						// wbkgd(text_win, COLOR_PAIR(2));
						// wattroff(text_win, COLOR_PAIR(3));
						// color_set(2, NULL);
						wattron(text_win, COLOR_PAIR(3));
						mvwprintw(text_win, i,j, "%s", word);
						wattroff(text_win, COLOR_PAIR(3));
						mistakes++;
						// wattroff(text_win, COLOR_PAIR(3));
						}

				}
				else
					mvwprintw(text_win, i,j, "%s", word);

				j += strlen(word);
				word_count++;
			}
			else{
				if(word_count == done_pos){
					wattron(text_win, A_UNDERLINE | A_BOLD);
					mvwprintw(text_win, i+1, 1, "%s", word);
					wattroff(text_win, A_UNDERLINE | A_BOLD);
				}
				else if(!feof(fw)){
					if(strcmp(word_typed, word) == 0){
						// color_set(2, NULL);
						// wbkgd(text_win, COLOR_PAIR(1));
						wattron(text_win, COLOR_PAIR(2));
						mvwprintw(text_win, i+1,1, "%s", word);
						wattroff(text_win, COLOR_PAIR(2));

					}
					else{
						// wbkgd(text_win, COLOR_PAIR(2));
						// wattroff(text_win, COLOR_PAIR(3));
						// color_set(2, NULL);
						wattron(text_win, COLOR_PAIR(3));
						mvwprintw(text_win, i+1,1, "%s", word);
						wattroff(text_win, COLOR_PAIR(3));
						// wattroff(text_win, COLOR_PAIR(3));
						mistakes++;
						}

				}
				else
					mvwprintw(text_win, i+1,1, "%s", word);

				start_pos = strlen(word);
				word_count++;
				break;	
			}
		}
		 // mvwprintw(outer_win, 1,1, "%s", word_typed);
	}

	while(flag == 1){
		wrefresh(outer_win);
		wrefresh(text_win);
		wrefresh(type_win);
		
		c = (int)getch();

		if(c && done_pos == 0){
			start_timer = time(NULL);
		}

		switch(c){
			case 32:
				// memset(word_typing, 0, 50);
				// mvwprintw(outer_win, 1, 1, "");
				if(word_typing){
					fprintf(fw, "%s \n", word_typing);
				}
				else{
					fprintf(fw, "~\n");
				}
				done_pos++;
				fclose(fw);
				if (done_pos == word_count){
					end_timer = time(NULL);
					// endwin();
					// exit(1);
					flag = 3;
				}
				goto next_word;
				break;

			case 27:
				// endwin();
				// fclose(f);
				// fclose(fw);
				// exit(1);
				flag = 2;
				// center_screen(0, 0);
				break;	

			case 127:
				// mvwprintw(outer_win, 1, 1, "%d", c);
				word_typing[strlen(word_typing) - 1] = '\0';
				goto next_word;
				break;

			case 10:
					break;

			default:
				word_typing[strlen(word_typing)] = (char)c;
				mvwprintw(type_win, 1, 1, "%s", word_typing);
				// mvwprintw(outer_win, 1, 1, "%d", c);
		}
	}

	if (flag == 2){
		center_screen(0, 0);
	}
	else{
		center_screen(word_count, mistakes);
	}
}

int main(){
	start();
	return 0;
}