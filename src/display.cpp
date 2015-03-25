

#include <string>

#include <SDL.h>

#include "collector.h"
#include "config.h"
#include "file.h"
#include "cli.h"
#include "grid.h"
#include "display.h"



Display::Display()
{
	cli = new CLI;
	grid = new Grid;

	selection = NULL;
}

Display::~Display()
{
	delete cli;
	delete grid;

	if(selection != NULL)
		delete selection;
}

void Display::on_resize(file_list::iterator begin, file_list::iterator end)
{
	SDL_GetWindowSize(window,
					  &(config->window_w),
					  &(config->window_h));

	grid->layout(begin, end);
}

void Display::on_key(SDL_KeyboardEvent &e)
{
	switch(e.keysym.sym)
	{
		case SDLK_ESCAPE:
			send_quit();
			break;
		case SDLK_RETURN:
			break;
		default:
			cli->on_key(e);
	}

	send_selector();
}

void Display::on_text(SDL_TextInputEvent &e)
{
	cli->on_text(e);
	send_selector();
}

void Display::on_wheel(SDL_MouseWheelEvent &e)
{
	grid->on_wheel(e);
}

void Display::render(file_list::iterator begin, file_list::iterator end)
{
	if(selection == NULL)
		grid->render(begin, end);
	else
		grid->render(begin, end, selection);

	cli->render();
}

void Display::on_select(Selection* new_selection)
{
	if(selection != NULL)
		delete selection;
	selection = new_selection;

	//let the components update themselves
	grid->read_selection(selection);
	cli->read_selection(selection);
}





void Display::send_quit()
{
	SDL_Event e;
	e.type = SDL_QUIT;

	//SDL copies memory into event queue, so this is memory safe
	SDL_PushEvent(&e);
}

void Display::send_selector()
{
	Selector* selector = new Selector;

	//ask the various components for their data
	cli->fill_selector(selector);

	//wrap in SDL_Event, and push into queue
	SDL_Event e;
	e.type = SDL_USEREVENT;
	e.user.type = SELECTOR;
	e.user.data1 = (void*) selector;
	SDL_PushEvent(&e);
}
