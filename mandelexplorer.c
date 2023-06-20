#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

const int WINDOW_WIDTH= 1280;
const int WINDOW_HEIGHT = 720;

// Init here so everything can use it
SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;

int mandelpoint(double point[2], int recursions, int prettycolors) {
    int i = 0;
    long double z[2] = {0, 0};
	long double z1[2] = {0, 0};
    while ((i < recursions) && (4.0 > (z[0]*z[0] + z[1]*z[1]))) {
		z1[0] = (z[0]*z[0]-z[1]*z[1]) + point[0]; // z = z^2 + c, except instead of complex numbers, an array is used
		z1[1] = (z[0]*z[1]+z[0]*z[1]) + point[1]; //
		z[0] = z1[0];
		z[1] = z1[1];
		++i;
		}
		// Debug
		// printf("%d, %d, %d\n", i, (int)(255.0 - (255.0 * ((float)i/(float)recursions))), i/recursions);
		return (int) ((255*32) - ((255) * ((i*32)/recursions)))/(32+((prettycolors-1)*15));
    }


void renderMandelbrot(double movex, double movey, double divfact, int recursions, int prettycolors){
	
	// Uncommenting may solve visual bugs. Clears the screen every redraw
	// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	// SDL_RenderClear(renderer);
	
	int bright;
	double pointc[2];
	int im = 0;
	int jm = 0;
	
	while (im <= WINDOW_WIDTH){
		pointc[0] = movex + (((double)(im) - (0.5 * (double)WINDOW_WIDTH))/divfact);
		jm = 0;
		while (jm <= WINDOW_HEIGHT){
			pointc[1] = movey + ( (double)jm - (0.5 * WINDOW_HEIGHT))/(divfact);
			
			bright = mandelpoint(pointc, recursions, prettycolors);
			SDL_SetRenderDrawColor(renderer, bright, bright*.8, bright*.6, 255);
			SDL_RenderDrawPoint(renderer, im, jm);
			++jm;
		}
		im++;
	}
	
	SDL_RenderPresent(renderer);

}

int function(double movex, double movey, double divfact, int recursions, int prettycolors){
	
	// wow, creating a window
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
   	renderMandelbrot(movex, movey, divfact, recursions, prettycolors);
	// such controls
	while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
		else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
				case SDLK_z:
					divfact *=1.41;
					break;
				case SDLK_o:
					divfact /=1.41;
					break;
				case SDLK_q:
					recursions += 5;
					break;
				case SDLK_w:
					recursions -=5;
					break;
				case SDLK_UP:
					movey -= 100.0/divfact;
					break;
				case SDLK_DOWN:
					movey += 100.0/divfact;
					break;
				case SDLK_LEFT:
					movex -= 100.0/divfact;
					break;
				case SDLK_RIGHT:
					movex += 100.0/divfact;
					break;
				case SDLK_p:
					prettycolors *= -1;
					break;
				case SDLK_ESCAPE:
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					SDL_Quit();
					return EXIT_SUCCESS;
					break;
			}
			// redraw every time a key is pressed
			renderMandelbrot(movex, movey, divfact, recursions, prettycolors);
		}
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
	//default params
	double zoomfactor = 300.0;
	double translatex = 0.0;
	double translatey = 0.0;
	int recursions = 50;
	int prettycolors = 1; // 1: false, -1: true
	function(translatex, translatey, zoomfactor, recursions, prettycolors);
	return 0;
}
