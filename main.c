#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define WIDTH 10
#define HEIGHT 20
#define BLOCK_SIZE 30
#define INFO_PANEL_WIDTH 200
#define WINDOW_WIDTH ((WIDTH * BLOCK_SIZE) + INFO_PANEL_WIDTH)
#define WINDOW_HEIGHT (HEIGHT * BLOCK_SIZE)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Color color = {0, 0, 0};
int board[HEIGHT][WIDTH] = {0};
int block[4][4] = {0};
int nextBlock[4][4] = {0};
int blockX = WIDTH / 2 - 2;
int blockY = 0;
int score = 0;
int speed = 2000;

int blocks[7][4][4] = {
        {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
        {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
        {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
        {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // S
        {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // Z
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},  // J
        {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}}   // L
};
void initGame() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();  // Don't forget to quit SDL before returning
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("C:\\Users\\kay\\CLionProjects\\ccode\\Roboto-Black.ttf", 24);
    if(!font) {
        printf("Error loading font: %s\n", SDL_GetError());
    }

    // Initialize the first nextBlock
    int randomIndex = rand() % 7;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            nextBlock[i][j] = blocks[randomIndex][i][j];
        }
    }
}


int isValidPosition(int newX, int newY) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(block[i][j]) {
                int x = newX + j;
                int y = newY + i;
                if(x < 0 || x >= WIDTH || y >= HEIGHT || (y >= 0 && board[y][x])) {
                    return 0; // Invalid position
                }
            }
        }
    }
    return 1; // Valid position
}

void generateBlock() {
    // Make the nextBlock the current block
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            block[i][j] = nextBlock[i][j];
        }
    }
    blockX = WIDTH / 2 - 2;
    blockY = 0;

    // Generate a new nextBlock
    int randomIndex = rand() % 7;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            nextBlock[i][j] = blocks[randomIndex][i][j];
        }
    }
}


void moveBlock(int dx, int dy) {
    if(isValidPosition(blockX + dx, blockY + dy)) {
        blockX += dx;
        blockY += dy;
    }
}

void rotateBlock() {
    int rotated[4][4] = {0};

    // Rotate the block
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            rotated[j][3 - i] = block[i][j];
        }
    }

    // Check if the rotated block is in a valid position
    if(isValidPosition(blockX, blockY)) {
        // Apply the rotation
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                block[i][j] = rotated[i][j];
            }
        }
    }
}
void renderScore() {
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {WINDOW_WIDTH - 180, WINDOW_HEIGHT - 50, 150, 50};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
void checkAndClearLines() {
    for(int y = 0; y < HEIGHT; y++) {
        int isLineComplete = 1;  // Assume the line is complete

        // Check if the line is complete
        for(int x = 0; x < WIDTH; x++) {
            if(board[y][x] == 0) {
                isLineComplete = 0;  // The line is not complete
                break;
            }
        }

        // If the line is complete, clear it and move down all above lines
        if(isLineComplete) {
            for(int aboveY = y; aboveY > 0; aboveY--) {
                for(int x = 0; x < WIDTH; x++) {
                    board[aboveY][x] = board[aboveY-1][x];
                }
            }
            // Clear the top line
            for(int x = 0; x < WIDTH; x++) {
                board[0][x] = 0;
            }
            // Update the score
            score++;
        }
    }
}

void updateGame() {
    static int counter = 0;
    counter += 16;  // Assuming this function is called every 16ms

    // Move block down
    if(counter >= speed) {
        if(isValidPosition(blockX, blockY + 1)) {
            blockY++;
        } else {
            // Fix the block to the board
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    if(block[i][j]) {
                        board[blockY + i][blockX + j] = 1;
                    }
                }
            }
            // Check and clear lines
            checkAndClearLines();
            // Generate a new block
            generateBlock();

            // Check for game over
            if(!isValidPosition(blockX, blockY)) {
                printf("Game Over! Score: %d\n", score);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                TTF_CloseFont(font);
                TTF_Quit();
                SDL_Quit();
                exit(0);
            }
        }
        counter = 0;
    }
}
void renderNextBlock() {
    int previewX = WINDOW_WIDTH - INFO_PANEL_WIDTH + 50; // Adjust as per your design
    int previewY = 50; // Adjust as per your design

    // Render the next block
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(nextBlock[y][x] == 1) {
                SDL_Rect rect = {previewX + x * BLOCK_SIZE, previewY + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blue
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}
void renderGame() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render the game board
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            if(board[y][x] == 1) {
                SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Render the current block
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(block[y][x] == 1) {
                SDL_Rect rect = {(blockX + x) * BLOCK_SIZE, (blockY + y) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Render grid
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);  // Light grey
    for(int x = 0; x <= WIDTH; x++) {
        SDL_RenderDrawLine(renderer, x * BLOCK_SIZE, 0, x * BLOCK_SIZE, WINDOW_HEIGHT);
    }
    for(int y = 0; y <= HEIGHT; y++) {
        SDL_RenderDrawLine(renderer, 0, y * BLOCK_SIZE, WINDOW_WIDTH - INFO_PANEL_WIDTH, y * BLOCK_SIZE);
    }
    renderNextBlock();
    renderScore();

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));  // Seed the random number generator

    initGame();
    generateBlock();  // Generate the first block

    SDL_Event e;
    while (1) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                TTF_CloseFont(font);
                TTF_Quit();
                SDL_Quit();
                return 0;
            } else if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_LEFT: moveBlock(-1, 0); break;
                    case SDLK_RIGHT: moveBlock(1, 0); break;
                    case SDLK_DOWN: moveBlock(0, 1); break;
                    case SDLK_UP: rotateBlock(); break;
                        // TODO: Add more controls (e.g., drop, etc.)
                }
            }
        }

        updateGame();
        renderGame();
        SDL_Delay(16); // Limit frame rate
    }
}
