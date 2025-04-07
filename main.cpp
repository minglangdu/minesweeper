#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <random>

#include <SDL2/SDL.h>

using namespace std;

const string textdir = "textures/";
const string textext = ".bmp";
const string paths[] = {
    "closed",
    "pressed",
    "face_pressed",
    "face_unpressed",
    "mine", 
    "type1",
    "type2",
    "type3",
    "type4",
    "type5",
    "type6",
    "type7",
    "type8"
};

const int cellsize = 25;
const int gridwidth = 16;
const int gridheight = 16;
const int mineamount = 40;

class GWind {
    public:
        GWind(int width, int height) {
            grid.resize(gridheight, vector<int> (gridwidth, -1));
            mines.resize(gridheight, vector<bool> (gridwidth, false));
            
            random_device rd;
            mt19937 mt (rd());
            uniform_int_distribution<mt19937::result_type> xs(0, gridwidth - 1), ys(0, gridheight - 1);

            for (int i = 0; i < mineamount; i ++) {
                int x = xs(mt), y = ys(mt);
                if (mines[y][x]) continue;
                mines[y][x] = true;
            }

            /*SDL*/

            window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, SDL_WINDOW_SHOWN);
            if (window == NULL) {
                cout << SDL_GetError() << "\n";
            }
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                cout << SDL_GetError() << "\n";
            }
            for (string path : paths) {
                SDL_Surface* surf = SDL_LoadBMP((textdir + path + textext).c_str());
                textures[path] = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_FreeSurface(surf);
            }
        }

        void startLoop() {
            while (!quit) {
                loop();
            }
        }

        void loop() {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
                if (e.type == SDL_MOUSEMOTION || 
                    e.type == SDL_MOUSEBUTTONDOWN || 
                    e.type == SDL_MOUSEBUTTONUP) {
                    int x, y, cellx, celly; 
                    SDL_GetMouseState(&x, &y);
                    if (x >= 50 && x <= cellsize * (gridwidth) + 50
                    && y >= 50 && y <= cellsize * (gridheight) + 50) {
                        cellx = (x - 50) / cellsize;
                        celly = (y - 50) / cellsize;
                        switch (e.type)
                        {
                        case SDL_MOUSEBUTTONDOWN:
                            if (grid[celly][cellx] == -1) {
                                grid[celly][cellx] = -2; // set to pressed
                            }
                            break;
                        case SDL_MOUSEBUTTONUP:
                            if (grid[celly][cellx] == -2) {
                                if (mines[celly][cellx]) {
                                    grid[celly][cellx] = 10;
                                    flipMines();
                                } else {
                                    flipTile(cellx, celly);
                                }
                            }
                            break;
                        default:
                            if (grid[celly][cellx] < -1) {
                                grid[celly][cellx] = -1;
                            }
                            break;
                        }
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
            
            SDL_RenderClear(renderer);
            renderGrid();

            SDL_RenderPresent(renderer);
        }

        void renderGrid() {
            SDL_Rect rect; 
            rect.w = cellsize; rect.h = cellsize;
            for (int gy = 0; gy < gridheight; gy ++) {
                for (int gx = 0; gx < gridwidth; gx ++) {
                    rect.x = cellsize * gx + 50;
                    rect.y = cellsize * gy + 50;
                    switch (grid[gy][gx]) {
                        case -1:
                            SDL_RenderCopy(renderer, textures["closed"], NULL, &rect);
                            break;
                        case 0:
                            SDL_RenderCopy(renderer, textures["pressed"], NULL, &rect);
                            break;
                        case 1:
                            SDL_RenderCopy(renderer, textures["type1"], NULL, &rect);
                            break;
                        case 2:
                            SDL_RenderCopy(renderer, textures["type2"], NULL, &rect);
                            break;
                        case 3:
                            SDL_RenderCopy(renderer, textures["type3"], NULL, &rect);
                            break;
                        case 4:
                            SDL_RenderCopy(renderer, textures["type4"], NULL, &rect);
                            break;
                        case 5:
                            SDL_RenderCopy(renderer, textures["type5"], NULL, &rect);
                            break;
                        case 6:
                            SDL_RenderCopy(renderer, textures["type6"], NULL, &rect);
                            break;
                        case 7:
                            SDL_RenderCopy(renderer, textures["type7"], NULL, &rect);
                            break;
                        case 8:
                            SDL_RenderCopy(renderer, textures["type8"], NULL, &rect);
                            break;
                        case 10:
                            SDL_RenderCopy(renderer, textures["mine"], NULL, &rect);
                            break;
                        default:
                            SDL_RenderCopy(renderer, textures["pressed"], NULL, &rect);
                            break;
                    }
                }
            }
        }
        void flipMines() {
            for (int gy = 0; gy < gridheight; gy ++) {
                for (int gx = 0; gx < gridwidth; gx ++) {
                    if (mines[gy][gx]) {
                        grid[gy][gx] = 10;
                    }
                }
            }
        }
        void flipTile(int x, int y) {
            if (x < 0 || x >= gridwidth || y < 0 || y >= gridwidth) {
                return;
            }
            if (grid[y][x] >= 0) {
                return;
            }
            int mineamt = 0;
            pair<int, int> dirs[8] = {
                {-1, 0},
                {-1, 1},
                {0, 1},
                {1, 1},
                {1, 0},
                {1, -1},
                {0, -1},
                {-1, -1}
            };
            for (pair<int, int> dir : dirs) {
                int cx = x + dir.first;
                int cy = y + dir.second;
                if (cx < 0 || cx >= gridwidth || cy < 0 || cy >= gridwidth) {
                    continue;
                }
                if (mines[cy][cx]) {
                    mineamt ++;
                }
            }
            switch (mineamt) {
                case 0:
                    grid[y][x] = 0;
                    for (pair<int, int> dir : dirs) {
                        int cx = x + dir.first;
                        int cy = y + dir.second;
                        if (cx < 0 || cx >= gridwidth || cy < 0 || cy >= gridwidth) {
                            continue;
                        }
                        flipTile(cx, cy);
                    }
                    break;
                default:
                    grid[y][x] = mineamt;
                    break;
            }
        }
        ~GWind() {
            SDL_DestroyWindow(window);
            window = NULL;
            SDL_DestroyRenderer(renderer);
            renderer = NULL;
            SDL_Quit();
        }

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event e; bool quit = false;
        map<string, SDL_Texture*> textures;
        vector<vector<int>> grid;
        vector<vector<bool>> mines;
};

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << SDL_GetError() << "\n";
    }
    GWind* game = new GWind(500, 500);
    game->startLoop();
    game->~GWind();
}