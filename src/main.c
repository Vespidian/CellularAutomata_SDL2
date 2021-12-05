#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include<SDL.h>
// #include<SDL2/SDL_thread.h>

#ifdef __TINYC__
    #undef main
#endif

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event e;

int singleParticleSize = 6;
int windowSize = 768;

#define NUM_CELLS 128

int cellsOld[NUM_CELLS][NUM_CELLS] = {};
int cells[NUM_CELLS][NUM_CELLS] = {};
bool quit = false;
bool isPaused = false;
bool showGrid = true;
SDL_Rect cursor;

int mouseX, mouseY;
bool mouseHeld = false;

void RenderScreen();
void PopulateCells();
void CreateArrayBackup();
int GetNeighbours(int x, int y);
void StepLife();

void init(){
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Cellular Automata (CWGOL)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowSize, windowSize, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	printf(
		"[Space] to pause and unpause\n"
		"[F] move a single frame forward in time\n"
		"[C] to clear the screen\n"
		"[P] to randomly populate the screen\n"
		"[G] to toggle grid\n"
		"[Left-Click] Make the currently hovered cell living\n"
		"[Right-Click] Make the currently hovered cell dead\n"
		"[Esc] exit\n");
		
	memset(cells, 0, sizeof(cellsOld));
	memset(cellsOld, 0, sizeof(cells));
	
	cursor = (SDL_Rect){0, 0, singleParticleSize, singleParticleSize};
	
	PopulateCells();
}

int main(int argc, char **argv){
	init();
	while(!quit){
		mouseHeld = false;
		
		SDL_GetMouseState(&mouseX, &mouseY);
		
		if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) || SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
			mouseHeld = true;
		}
		
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_ESCAPE){
					quit = true;
				}
			}
			if(e.type == SDL_KEYUP){
				if(e.key.keysym.sym == SDLK_SPACE){//Toggle pause
					isPaused = !isPaused;
					if(isPaused){
						SDL_SetWindowTitle(window, "Cellular Automata (CWGOL) (PAUSED)");
					}else{
						SDL_SetWindowTitle(window, "Cellular Automata (CWGOL)");
					}
				}
				if(e.key.keysym.sym == SDLK_f){//Step forward in time 1 frame
					CreateArrayBackup();
					StepLife();
				}
				if(e.key.keysym.sym == SDLK_g){//Step forward in time 1 frame
					showGrid = !showGrid;
				}
				if(e.key.keysym.sym == SDLK_c){//Set all cells to dead
					memset(cells, 0, sizeof(cellsOld));
					memset(cellsOld, 0, sizeof(cells));
				}
				if(e.key.keysym.sym == SDLK_p){
					PopulateCells();
				}
			}
			if(e.type == SDL_QUIT){
				quit = true;
			}
		}
		
		if(mouseHeld){
			if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){	
				cells[mouseY / singleParticleSize][mouseX / singleParticleSize] = 1;
			}
			if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
				cells[mouseY / singleParticleSize][mouseX / singleParticleSize] = 0;
			}
		}
		RenderScreen();
		SDL_Delay(17);//60 fps
	}
	return 0;
}

void RenderScreen(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);
	
	if(!isPaused){
		CreateArrayBackup();
		StepLife();
	}
	
	SDL_Rect cell = {0, 0, singleParticleSize - 1, singleParticleSize - 1};
	SDL_Rect verticalGrid = {0, 0, 1, windowSize};
	SDL_Rect horizontalGrid = {0, 0, windowSize, 1};
	for(int y = 0; y < NUM_CELLS; y++){
		if(showGrid){
			SDL_SetRenderDrawColor(renderer, 22, 22, 22, 0xff);
			//Horizontal gird-lines
			horizontalGrid.y = (y * singleParticleSize) - 1;
			//Vertical grid-lines
			verticalGrid.x = (y * singleParticleSize) - 1;
			SDL_RenderDrawRect(renderer, &horizontalGrid);	
			SDL_RenderDrawRect(renderer, &verticalGrid);
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xff);
		for(int x = 0; x < NUM_CELLS; x++){
			cell.x = x * singleParticleSize;
			cell.y = y * singleParticleSize;
			if(cells[y][x] == 1){
				SDL_RenderFillRect(renderer, &cell);
			}
		}
	}
	
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
	cursor.x = ((windowSize / singleParticleSize) * mouseX / windowSize) * singleParticleSize;
	cursor.y = ((windowSize / singleParticleSize) * mouseY / windowSize) * singleParticleSize;
	SDL_RenderFillRect(renderer, &cursor);
	
	SDL_RenderPresent(renderer);
}

void PopulateCells(){//Randomly assign cells
	for(int y = 0; y < NUM_CELLS; y++){
		for(int x = 0; x < NUM_CELLS; x++){
			if(rand() % 3 == 1){
				cells[y][x] = 1;
			}
		}
	}
}

void CreateArrayBackup(){//Use unchanged data when modifying the next generation
	for(int y = 0; y < NUM_CELLS; y++){
		for(int x = 0; x < NUM_CELLS; x++){
			cellsOld[y][x] = cells[y][x];
		}
	}
}

int GetNeighbours(int x, int y){//Count the number of alive cells neighbouring a cell
	int count = 
		cellsOld[y - 1][x - 1] + cellsOld[y - 1][x] + cellsOld[y - 1][x + 1] + 
		cellsOld[y][x - 1] + 			 0			+ cellsOld[y][x + 1] +
		cellsOld[y + 1][x - 1] + cellsOld[y + 1][x] + cellsOld[y + 1][x + 1];
	return count;
}

void StepLife(){//Implement CWGOL rules to iterate array
	int neighbours = 0;
	for(int y = 0; y < NUM_CELLS; y++){
		for(int x = 0; x < NUM_CELLS; x++){
			neighbours = GetNeighbours(x, y);
			if(neighbours == 3){
				cells[y][x] = 1;
			}else if(neighbours < 2 || neighbours > 3){
				cells[y][x] = 0;
			}
		}
	}
}