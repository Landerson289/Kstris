#include "graphics.h"

#define default_alpha 255

#define check_rgb_overflow(channel) ( channel > 255 ) ? ( channel = 255 ) : ( channel )
#define check_rgb_underflow(channel) ( channel < 0 ) ? ( channel = 0 ) : ( channel )

// define the SDL window and renderer 
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* gFont;

// set the default render state 
bool render_changed = true;

void draw_block(SDL_Renderer* renderer, u8 x_pos, u8 y_pos, u8 colour){
    //TODO: Add more comments
    assert(colour >= 0 && colour <= NUM_BLOCK_COLOURS);
    u8 rgb_code[3] = {*BLOCK_COLOURS_RGB[colour], *(BLOCK_COLOURS_RGB[colour] + 1), *(BLOCK_COLOURS_RGB[colour] + 2)};

    SDL_Rect outer;
    SDL_Rect inner;
    
    // setting boundary for outer colour of tetronimo block 
    outer.x = (x_pos + 1) * BLOCK_SIZE;
    outer.y = (y_pos + 1) * BLOCK_SIZE; 
    outer.w = BLOCK_SIZE;
    outer.h = BLOCK_SIZE; 

    // setting boundary for inner colour of tetronimo block
    inner.x = ((x_pos + 1) * BLOCK_SIZE) + 3;
    inner.y = ((y_pos + 1) * BLOCK_SIZE) + 3; 
    inner.w = BLOCK_SIZE - 6;
    inner.h = BLOCK_SIZE - 6;

    // Perform shifts to change the colour of the outside edge
    unsigned int r, g, b;
    r = (*rgb_code >> 1) & 0xFF;
    check_rgb_overflow(r); check_rgb_underflow(r);
    g = *(rgb_code + 1) >> 1 & 0xFF;
    check_rgb_overflow(g); check_rgb_underflow(g);
    b = *(rgb_code + 2) >> 1 & 0xFF; 
    check_rgb_overflow(b); check_rgb_underflow(b);

    SDL_SetRenderDrawColor(renderer, r, g, b, default_alpha);
    SDL_RenderFillRect(renderer, &outer);

    SDL_SetRenderDrawColor(renderer, *rgb_code, *(rgb_code + 1), *(rgb_code + 2), default_alpha);
    SDL_RenderFillRect(renderer, &inner);   

    render_changed = true;
}

void draw_outline(SDL_Renderer *renderer, u8 x_pos, u8 y_pos, u8 colour){
    assert(colour >= 0 && colour <= NUM_BLOCK_COLOURS);
    u8 rgb_code[3] = {*BLOCK_COLOURS_RGB[colour], *(BLOCK_COLOURS_RGB[colour] + 1), *(BLOCK_COLOURS_RGB[colour] + 2)};
    
    SDL_Rect outline;
    SDL_Rect inner;

    outline.x = (x_pos + 1) * BLOCK_SIZE;
    outline.y = (y_pos + 1) * BLOCK_SIZE; 
    outline.w = BLOCK_SIZE;
    outline.h = BLOCK_SIZE; 

    inner.x = ((x_pos + 1) * BLOCK_SIZE) + 2;
    inner.y = ((y_pos + 1) * BLOCK_SIZE) + 2; 
    inner.w = BLOCK_SIZE - 4;
    inner.h = BLOCK_SIZE - 4; 

    SDL_SetRenderDrawColor(renderer, *rgb_code, *(rgb_code + 1), *(rgb_code + 2), default_alpha);
    SDL_RenderFillRect(renderer, &outline);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, default_alpha);
    SDL_RenderFillRect(renderer, &inner);  

    render_changed = true;
}

void draw_playfield(SDL_Renderer* renderer){
    // Function to draw the playfield, starting by drawing the border around which 
    // the playfield will be contained

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, default_alpha);
    
    SDL_RenderClear(renderer);       

    // Drawing border
    for (int width = 0; width < PLAYFIELD_WIDTH; width++){
	for (int height = 0; height < PLAYFIELD_HEIGHT; height++){
	    if (height == 0 || height == PLAYFIELD_HEIGHT - 1){
		// Draw the row of gray border blocks for the top and bottom row
		
		draw_block(renderer, width, height, GRAY);
	    }
	    else if (width == 0 || width == PLAYFIELD_WIDTH - 1){
		// Draw the column of border blocks for the left and right columns 
		
		draw_block(renderer, width, height, GRAY);
	    }   
	}
    }

    render_changed = true;
}

// Initialise graphics
void init_graphics(){
    u32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS;
    if (SDL_Init(sdl_flags) != 0){
	printf("error initialising SDL: %s\n", SDL_GetError());
	exit(1);
    }

    window = SDL_CreateWindow(
	// Window title 
	WINDOW_TITLE,
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	WINDOW_WIDTH, WINDOW_HEIGHT,
	0
    );


    if (!window){
	printf("error creating window: %s\n", SDL_GetError());
	SDL_Quit();
	exit(1);
    }

    // Create a renderer, which sets up the graphics hardware 
    u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, render_flags);

    if (!renderer){
	printf("Error creating renderer: %s\n", SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	exit(1);
    }
    
    // TODO: Use the render_graphics() function in place with repeated use of SDL_RenderPresent();
    draw_playfield(renderer);
   
    /*
    // TODO: Create the texture for render context and load in a .ttf font
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Create a texture for a rendering context 
    SDL_Texture *texture = SDL_CreateTexture(
	renderer, 
	SDL_PIXELFORMAT_RGBA8888, 
	SDL_TEXTUREACCESS_TARGET, 
	WINDOW_WIDTH, WINDOW_HEIGHT
    );

    SDL_SetRenderTarget(renderer, texture);
 
    // Load font 
    gFont = TTF_OpenFont("src/font/PressStart2P.ttf", 30);

    if (gFont == NULL){
	printf("Error loading font %s\n", SDL_GetError());
	exit(1);
    }

    // Set the font hinting 
    TTF_SetFontHinting(gFont, TTF_HINTING_MONO);
    */
}

void render_frame(SDL_Renderer* renderer){ 
    // setting the target to NULL will automatically render to the window
    SDL_RenderPresent(renderer);
}

void update_render(){
    if (render_changed){
	// run the render_frame function
	
	printf("RENDERING FRAME\n");
	render_frame(renderer);
	render_changed = false;
    }
}

void cleanup_graphics(){
    // Clean resources by destroying window and quitting from SDL2 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
