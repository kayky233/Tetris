#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <time.h>

#define WIDTH 10
#define HEIGHT 20
#define BLOCK_SIZE 30
#define INFO_PANEL_WIDTH 200
#define WINDOW_WIDTH ((WIDTH * BLOCK_SIZE) + INFO_PANEL_WIDTH)
#define WINDOW_HEIGHT (HEIGHT * BLOCK_SIZE)

void renderGame();
void moveBlock(int dx, int dy);
void rotateBlock();
enum GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// 全局变量来存储当前的游戏状态
enum GameState gameState = MENU;
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
void handleMenuInput(SDL_Event e) {
    // 在这里添加代码来处理开始菜单的输入
    // 例如，如果玩家按下Enter键，改变游戏状态到PLAYING
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
        gameState = PLAYING;
        // 可能还需要执行其他的初始化代码，例如重置游戏板
    }
}

void handlePlayingInput(SDL_Event e) {
    // 在这里添加代码来处理游戏中的输入
    // 例如，移动和旋转方块，以及暂停游戏
    if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_LEFT: moveBlock(-1, 0); break;
            case SDLK_RIGHT: moveBlock(1, 0); break;
            case SDLK_DOWN: moveBlock(0, 1); break;
            case SDLK_UP: rotateBlock(); break;
            case SDLK_LCTRL: // 如果按下左Ctrl键
            case SDLK_RCTRL: // 或者按下右Ctrl键
                gameState = PAUSED; // 切换到PAUSED状态
                break;
        }
    }
}

void handlePausedInput(SDL_Event e) {
    // 处理在游戏暂停时的输入
    if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_LCTRL: // 如果按下左Ctrl键
            case SDLK_RCTRL: // 或者按下右Ctrl键
                gameState = PLAYING; // 切换回PLAYING状态
                break;
            case SDLK_ESCAPE: // 如果按下Escape键
                gameState = MENU; // 切换回MENU状态
                break;
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
void animateLineClear(int lineY) {
    int flashTimes = 5;
    int flashInterval = 100; // ms

    for (int i = 0; i < flashTimes; i++) {
        renderGame(lineY);
        SDL_RenderPresent(renderer);
        SDL_Delay(flashInterval);

        renderGame(-1);
        SDL_RenderPresent(renderer);
        SDL_Delay(flashInterval);
    }
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
            animateLineClear(y);
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
void renderMenu() {
    // 清除屏幕
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 设置为白色
    SDL_RenderClear(renderer);

    // 设置字体颜色为黑色
    SDL_Color textColor = {0, 0, 0, 255};

    // 加载字体
    TTF_Font* font = TTF_OpenFont("C:\\Users\\kay\\CLionProjects\\ccode\\Roboto-Black.ttf", 28); // 请替换为您的字体文件的路径和所需的字体大小
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return;
    }

    // 创建一个表面来渲染文本
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Press Enter to Start", textColor);
    if (textSurface == NULL) {
        printf("Failed to render text: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    // 创建一个纹理从表面
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return;
    }

    // 设置渲染文本的位置
    SDL_Rect textRect;
    textRect.x = (WINDOW_WIDTH - textSurface->w) / 2; // 水平居中
    textRect.y = (WINDOW_HEIGHT - textSurface->h) / 2; // 垂直居中
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    // 渲染文本
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // 清理
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

    // 更新屏幕
    SDL_RenderPresent(renderer);
}
void renderPaused() {
    // 渲染一个表示游戏已暂停的文本消息
    SDL_Color color = {255, 0, 0}; // 红色
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Game Paused", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {WINDOW_WIDTH / 2 - surface->w / 2, WINDOW_HEIGHT / 2 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    // 释放资源
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
}

void renderGame(int skipLine) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render the game board
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            if(board[y][x] == 1) {
                SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                // Check if this line should be highlighted
                if(y == skipLine) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White for highlight
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green otherwise
                }
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
            } else {
                switch(gameState) {
                    case MENU: handleMenuInput(e); break;
                    case PLAYING: handlePlayingInput(e); break;
                    case PAUSED: handlePausedInput(e); break;
                }
            }
        }

        switch(gameState) {
            case MENU: renderMenu(); break;
            case PLAYING:
                updateGame();
                renderGame(-1);
                break;
            case PAUSED:
                renderPaused();
                break;
        }

        SDL_Delay(16); // Limit frame rate
    }
}