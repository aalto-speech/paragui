/* GOAL: Test various paragui features.
*/

/* Interesting; when InitScreen is given invalid parameters along with SDL_FULLSCREEN,
 * the program hangs rather than giving a reasonable error message. */

/* Ask: How do you create a custom theme? */

/*
    Submitted by: Mark Krosky <krosky@concentric.net>
*/

#include <pgapplication.h>
#include <pgpopupmenu.h> 
#include <pgbutton.h>
#include <pgmenubar.h> 
#include <pgscrollbar.h> 

#define ID_APP_EXIT		1

PARAGUI_CALLBACK(exit_handler) {

	// we can pass in some pointer to any userdata
	// (in this case we get a pointer to the application object)
	PG_Application* app = (PG_Application*) clientdata;

	// exit the application eventloop
	app->Quit();

	// return true to signal that we have processed this message
	return true;
}

PARAGUI_CALLBACK_SELECTMENUITEM(handle_menu_click) {

	switch (id) {
	  case ID_APP_EXIT:
		static_cast<PG_Application*>(clientdata)->Quit();
		break;
	}

	return true;
}

class PlayField : public PG_ThemeWidget {
public:

	// the constructor
	PlayField(PG_Widget* parent, PG_Rect r);

	// the destructor
	~PlayField();

	void timer_callback(void);

protected:

	// our custom event handler to redraw our stuff
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);

private:

	// the color we want to draw the lines with
	SDL_Color my_color;

	int tickstate;
};

// implementation of MyWidget

PlayField::PlayField(PG_Widget* parent, PG_Rect r) : PG_ThemeWidget(parent, r) {
	// here we do some initialization e.g the color :)

	// the red value
	my_color.r = 200;

	// the green value
	my_color.g = 50;

	// the blue value
	my_color.b = 10;

	tickstate = 0;
}

PlayField::~PlayField() {
	// here we could do some cleanup
}


// implementation of the draw event
// (fills the widget with some nice gfx)

void PlayField::eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst) {

	SDL_FillRect(my_srfScreen, (SDL_Rect*)&dst, 0);
	
	if (tickstate==0) {
		// we draw our first line
		DrawLine(
			0,
			0,
			my_width-1,
			my_height-1,
			my_color,
			1
			);

		// we draw our second line
		DrawLine(
			my_width-1,
			0,
			0,
			my_height-1,
			my_color,
			1
			);
	}
	else if (tickstate == 1) {
		DrawHLine(0, my_height/2, my_width-1, my_color.r, my_color.g, my_color.b);
	}
}

void PlayField::timer_callback(void) {

	int prev_tickstate = tickstate;

	tickstate = (SDL_GetTicks()/1000)%3;

	if (tickstate != prev_tickstate) {
		Update();
	}
}

class PlayField2 : public PG_ThemeWidget
{
public:

	// the constructor
	PlayField2(PG_Widget* parent, PG_Rect r);

	// the destructor
	~PlayField2();

	void timer_callback(void);

protected:

	// our custom event handler to redraw our stuff
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);

private:

	// the color we want to draw the lines with
	SDL_Color my_color;

	int tickstate;
};

PlayField2::PlayField2(PG_Widget* parent, PG_Rect r) : PG_ThemeWidget(parent, r) {
	// here we do some initialization e.g the color :)

	// the red value
	my_color.r = 0;

	// the green value
	my_color.g = 0;

	// the blue value
	my_color.b = 255;

	tickstate = 0;
}

PlayField2::~PlayField2() {
	// here we could do some cleanup
}


// implementation of the draw event
// (fills the widget with some nice gfx)

void PlayField2::eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst) {

	SDL_FillRect(my_srfScreen, (SDL_Rect*)&dst, 0);
	
	for (int i=0; i<3; i++)
	{
	  DrawLine(
		i*40 + tickstate,
		0,
		i*40 + tickstate,
		my_height/2,
		my_color,
		1
		);
	}

	SDL_Color temp_color;
	PG_Rect temp_rect;
	Uint32 temp_int;

	temp_rect.x = dst.x;
	temp_rect.y = dst.y+(dst.h/2);
	temp_rect.w = dst.w;
	temp_rect.h = dst.h-(dst.h/2);

	temp_color.r = (my_color.r * tickstate)/40;
	temp_color.g = (my_color.g * tickstate)/40;
	temp_color.b = (my_color.b * tickstate)/40;

	temp_int = SDL_MapRGB(my_srfScreen->format, temp_color.r, temp_color.g, temp_color.b);
	SDL_FillRect(my_srfScreen, (SDL_Rect *)&temp_rect, temp_int);
}

void PlayField2::timer_callback(void) {

	int prev_tickstate = tickstate;

	tickstate = (SDL_GetTicks()%1000)/25;

	if (tickstate != prev_tickstate) {
		Update();
	}
}

Uint32 timer_callback(Uint32 interval, void* parameter) {
	((PlayField2 *)parameter)->timer_callback();
	return interval;
}

PARAGUI_CALLBACK(appidle_handler) {
	((PlayField *)clientdata)->timer_callback();
	return true;
}

int main(int argc, char* argv[]) {

	// every ParaGUI application need an application-object
	PG_Application app;

	// let us escape with "ESC"
	app.SetEmergencyQuit(true);
	
	// every application needs a theme (the look & feel of the widgets)
	//app.LoadTheme("default");
	app.LoadTheme("simple");

	// we must initialize the screen where we want to draw on

	// 640 - screen width
	// 480 - screen height
	// 0 - use screen bitdepth
	// SDL_SWSURFACE - PG_ option to generate surface in system memory

	app.InitScreen(800, 600, 0, SDL_SWSURFACE);

	// ok - now we have a nice 640x480x16 window on the screen :)

	PG_Rect rect(0, 0, 80, 30);

	PG_Button myButton(
		NULL,		// an optional parent widget for our button - NULL for no parent
		1,		// the widget id (used to identify events)
		rect,		// the screen position where the button should appear
		"Quit"		// some textlabel for the button
		);

	// this defines our callback handler for the message MSG_BUTTONCLICK,
	// we pass a pointer to the app object as userdata

	PG_MenuBar menubar(NULL, PG_Rect(100, 0, 400, 30));
	PG_PopupMenu   popmenu(NULL, 425, 140, "File");

	popmenu.addMenuItem("Nail", 99, handle_menu_click).
        addMenuItem("Quit", ID_APP_EXIT, handle_menu_click, &app);
 
	menubar.Add("File", &popmenu);

	menubar.Show();

	myButton.SetEventCallback(MSG_BUTTONCLICK, exit_handler, &app);

	// now we have to make the button visible

	myButton.Show();

	// Every ParaGUI application is event driven, so we need a loop where
	// we process all events (like mouse handling, keystrokes,...)

	// usually this is done with PG_Application::Run()

	PG_Rect sc_rect(50, 50, 100, 300);
	PG_ScrollBar myscroll(NULL, 100, sc_rect, PG_SB_VERTICAL);
	myscroll.Show();

	PG_Rect sc_rect2(200, 200, 300, 100);
	PG_ScrollBar myscroll2(NULL, 101, sc_rect2, PG_SB_HORIZONTAL);

	myscroll2.SetWindowSize(10);
	myscroll2.SetRange(0, 100);
	myscroll2.SetPageSize(10);
	myscroll2.SetLineSize(5);

	myscroll2.Show();

	// Attempt to get animation
	PlayField anim_test(
		// still no parent widget
		NULL,
		// a static function to create rects
		PG_Rect(260,120,120,50)
		);

	// show me your .... lines ;-)

	anim_test.Show();

    PlayField2 anim_test2(
	  NULL,
	  PG_Rect(260, 300, 120, 100)
	  );
	anim_test2.Show();

	SDL_Init(SDL_INIT_TIMER);
	SDL_AddTimer(20, timer_callback, (void *)&anim_test2);

	app.SetEventCallback(MSG_APPIDLE, appidle_handler, (void *)&anim_test);
	app.EnableAppIdleCalls();

	app.Run();

	// this function will only exit when the application was closed

	return 0;
}

// Maybe should use #define and ID #s for widet IDs.
