#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include<SDL2/SDL.h>

#ifdef __TINYC__
    #undef main
#endif

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event e;

int singleParticleSize = 6;
int windowSize = 768;

int cellsOld[128][128] = {};
int cells[128][128] = {};
bool quit = false;
bool isPaused = false;
bool showGrid = true;

int mouseX, mouseY;
bool mouseHeld = false;

void RenderScreen();
void PopulateCells();
void CreateArrayBackup();
int GetNeighbours(int x, int y);
void StepLife();

void init(){
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Cellular Automata (CWGOL)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowSize, windowSize, SDL_WINDOW_OPENGL);
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
}

void RenderScreen(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);
	
	if(!isPaused){
		CreateArrayBackup();
		StepLife();
	}
	
	SDL_Rect cell = {0, 0, 5, 5};
	SDL_Rect verticalGrid = {0, 0, 1, windowSize};
	SDL_Rect horizontalGrid = {0, 0, windowSize, 1};
	for(int y = 0; y < 128; y++){
		if(showGrid){
			//Horizontal gird-lines
			SDL_SetRenderDrawColor(renderer, 22, 22, 22, 0xff);
			horizontalGrid.y = (y * singleParticleSize) - 1;
			SDL_RenderDrawRect(renderer, &horizontalGrid);	
			//Vertical grid-lines
			verticalGrid.x = (y * singleParticleSize) - 1;
			SDL_RenderDrawRect(renderer, &verticalGrid);
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xff);
		for(int x = 0; x < 128; x++){
			cell.x = x * singleParticleSize;
			cell.y = y * singleParticleSize;
			if(cells[y][x] == 1){
				SDL_RenderFillRect(renderer, &cell);
			}
		}
	}
	
	SDL_RenderPresent(renderer);
}

void PopulateCells(){//Randomly assign cells
	for(int y = 0; y < 128; y++){
		for(int x = 0; x < 128; x++){
			if(rand() % 3 == 1){
				cells[y][x] = 1;
			}
		}
	}
}

void CreateArrayBackup(){//Use unchanged data when modifying the next generation
	for(int y = 0; y < 128; y++){
		for(int x = 0; x < 128; x++){
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
	for(int y = 0; y < 128; y++){
		for(int x = 0; x < 128; x++){
			neighbours = GetNeighbours(x, y);
			if(neighbours == 3){
				cells[y][x] = 1;
			}else if(neighbours < 2 || neighbours > 3){
				cells[y][x] = 0;
			}
		}
	}
}