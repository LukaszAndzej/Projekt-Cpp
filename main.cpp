#include "my_lib/game/game.h"
#include "my_lib/graph/GraphAsMatrix.h"
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_image.h"

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int NUM_PIGGIES = 5;
const int KEY_WIDTH = 64;
const int KEY_HEIGHT = 64;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* warriorTexture = nullptr;
SDL_Texture* fireTexture = nullptr;
std::vector<SDL_Texture*> piggyTextures;
std::vector<SDL_Texture*> keyTextures;
std::vector<SDL_Rect> keyRects;
std::vector<bool> isKeyActive;
SDL_Rect warriorRect;
SDL_Rect fireRect;
std::vector<SDL_Rect> piggyRects;
int warriorSpeed = 5;
bool isFireActive = false;
char lastKeyPressed = 'd';
int fireSpeed = 2;
std::vector<bool> isPiggyActive;
std::vector<int> piggyNumbers;

bool InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Nie można zainicjować SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Skarbonki", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Log("Nie można utworzyć okna: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_Log("Nie można utworzyć renderera: %s", SDL_GetError());
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        SDL_Log("Nie można zainicjować biblioteki SDL_image: %s", IMG_GetError());
        return false;
    }

    return true;
}

bool LoadMedia() {
    SDL_Surface* surface = IMG_Load("pictures/boy.png");
    if (surface == nullptr) {
        SDL_Log("Nie można załadować obrazka: %s", IMG_GetError());
        return false;
    }

    warriorTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (warriorTexture == nullptr) {
        SDL_Log("Nie można utworzyć tekstury z obrazka: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);

    surface = IMG_Load("pictures/fire.png");
    if (surface == nullptr) {
        SDL_Log("Nie można załadować obrazka: %s", IMG_GetError());
        return false;
    }

    fireTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (fireTexture == nullptr) {
        SDL_Log("Nie można utworzyć tekstury z obrazka: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);

    for (int i = 1; i <= NUM_PIGGIES; ++i) {
        std::string piggyFilePath = "pictures/piggy" + std::to_string(i) + ".png";
        surface = IMG_Load(piggyFilePath.c_str());
        if (surface == nullptr) {
            SDL_Log("Nie można załadować obrazka: %s", IMG_GetError());
            return false;
        }

        SDL_Texture* piggyTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (piggyTexture == nullptr) {
            SDL_Log("Nie można utworzyć tekstury z obrazka: %s", SDL_GetError());
            SDL_FreeSurface(surface);
            return false;
        }

        piggyTextures.push_back(piggyTexture);
        SDL_FreeSurface(surface);

        std::string keyFilePath = "pictures/key" + std::to_string(i) + ".png";
        surface = IMG_Load(keyFilePath.c_str());
        if (surface == nullptr) {
            SDL_Log("Nie można załadować obrazka: %s", IMG_GetError());
            return false;
        }

        SDL_Texture* keyTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (keyTexture == nullptr) {
            SDL_Log("Nie można utworzyć tekstury z obrazka: %s", SDL_GetError());
            SDL_FreeSurface(surface);
            return false;
        }

        keyTextures.push_back(keyTexture);
        SDL_FreeSurface(surface);
    }

    return true;
}

void CloseSDL() {
    for (SDL_Texture* piggyTexture : piggyTextures) {
        SDL_DestroyTexture(piggyTexture);
    }

    for (SDL_Texture* keyTexture : keyTextures) {
        SDL_DestroyTexture(keyTexture);
    }

    SDL_DestroyTexture(fireTexture);
    SDL_DestroyTexture(warriorTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void Update() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return;

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    warriorRect.y -= warriorSpeed;
                    lastKeyPressed = 'w';
                    break;

                case SDLK_a:
                    warriorRect.x -= warriorSpeed;
                    lastKeyPressed = 'a';
                    break;

                case SDLK_s:
                    warriorRect.y += warriorSpeed;
                    lastKeyPressed = 's';
                    break;

                case SDLK_d:
                    warriorRect.x += warriorSpeed;
                    lastKeyPressed = 'd';
                    break;

                case SDLK_SPACE:
                    if (!isFireActive) {
                        isFireActive = true;
                        fireRect.x = warriorRect.x;
                        fireRect.y = warriorRect.y + warriorRect.h / 2 - fireRect.h / 2;
                    }
                    break;

                case SDLK_q:
                    SDL_Quit();
                    exit(0);
                    break;

                default:
                    break;
            }
        }
    }

    // Sprawdzanie granic ekranu dla wojownika
    if (warriorRect.x < 0) warriorRect.x = 0;
    if (warriorRect.x > SCREEN_WIDTH - warriorRect.w) warriorRect.x = SCREEN_WIDTH - warriorRect.w;
    if (warriorRect.y < 0) warriorRect.y = 0;
    if (warriorRect.y > SCREEN_HEIGHT - warriorRect.h) warriorRect.y = SCREEN_HEIGHT - warriorRect.h;

    if (isFireActive)
    {
        switch (lastKeyPressed) {
            case 'w':
                fireRect.y -= fireSpeed;
                break;

            case 'a':
                fireRect.x -= fireSpeed;
                break;

            case 's':
                fireRect.y += fireSpeed;
                break;

            case 'd':
                fireRect.x += fireSpeed;
                break;

            default:
                break;
        }

        // Sprawdzanie, czy płomień opuścił obszar ekranu
        if (fireRect.x < 0 || fireRect.x > SCREEN_WIDTH || fireRect.y < 0 || fireRect.y > SCREEN_HEIGHT) {
            isFireActive = false;
        }
    }

    // Sprawdzanie kolizji między ogniem a świnką
    for (int i = 0; i < NUM_PIGGIES; ++i) {
        if (isPiggyActive[i] && SDL_HasIntersection(&fireRect, &piggyRects[i])) {
            if (!isKeyActive[i]) {
                isKeyActive[i] = true;
                SDL_Rect keyRect = { piggyRects[i].x, piggyRects[i].y, KEY_WIDTH, KEY_HEIGHT };
                keyRects.push_back(keyRect);
            }

            isPiggyActive[i] = false;
            piggyNumbers.push_back(i);
            for (int i : piggyNumbers) {
                GraphAsMatrix::Log::Info("Piggy number " + std::to_string(i + 1) + " was shot down!");
            }
        }
    }
}

void Render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, warriorTexture, nullptr, &warriorRect);

    if (isFireActive) SDL_RenderCopy(renderer, fireTexture, nullptr, &fireRect);

    for (int i = 0; i < NUM_PIGGIES; ++i) {
        if (isPiggyActive[i]) SDL_RenderCopy(renderer, piggyTextures[i], nullptr, &piggyRects[i]);
    }

    for (int i = 0; i < keyRects.size(); ++i) {
        if (isKeyActive[i]) SDL_RenderCopy(renderer, keyTextures[i], nullptr, &keyRects[i]);
    }

    SDL_RenderPresent(renderer);
}

// Funkcja wczytująca obrazek z pliku i zwracająca teksturę
SDL_Texture* LoadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (surface == nullptr) {
        std::cout << "Failed to load image: " << path << ", Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Funkcja renderująca teksturę na ekranie
void RenderTexture(SDL_Texture* texture, int x, int y) {
    SDL_Rect dstRect;
    dstRect.x = x;
    dstRect.y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &dstRect.w, &dstRect.h);
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
}

bool isInCycle(const std::vector<std::vector<int>>& cycles, std::vector<bool>& cyclesCheck, int number) {
    for (int i = 0; i < cycles.size(); i++) {
        for (int value : cycles[i]) {
            if (value == number) {
                if (cyclesCheck[i]) break;

                cyclesCheck[i] = true;
                return true;
            }
        }
    }
    return false; // Liczba nie należy do żadnego cyklu
}


int main(int argc, char* args[]) {

    srand(static_cast<unsigned int>(time(nullptr)));

    if (!InitSDL()) {
        SDL_Log("Nie można zainicjować SDL");
        return -1;
    }

    if (!LoadMedia()) {
        SDL_Log("Nie można załadować mediów");
        return -1;
    }
 
    // GraphAsMatrix graph(5, "res/base1.csv");
    // GraphAsMatrix graph(4, "res/base2.csv");
    GraphAsMatrix graph(5, "res/base3.csv");

    std::vector<std::vector<int>> cycles = graph.find_cycles();
    std::vector<bool> cyclesCheck(cycles.size());


    warriorRect = { SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT / 2 - 25, 50, 50 };
    fireRect = { 0, 0, 50, 25 };

    const int numberOfPiggies = graph.get_all_vertex();
    for (int i = 0; i < numberOfPiggies; ++i) {
        isPiggyActive.push_back(true);
        isKeyActive.push_back(false);

        SDL_Rect piggyRect;
        piggyRect.w = 64;
        piggyRect.h = 64;
        piggyRect.x = rand() % (SCREEN_WIDTH - piggyRect.w);
        piggyRect.y = rand() % (SCREEN_HEIGHT - piggyRect.h);
        piggyRects.push_back(piggyRect);
    }

    
    // Wczytanie obrazków win oraz game over
    SDL_Texture* winTexture = LoadTexture("pictures/win.png");
    SDL_Texture* overTexture = LoadTexture("pictures/over.png");

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    bool quit = false;
    while (!quit) {

        Update();
        Render();

        bool allTrue = std::all_of(cyclesCheck.begin(), cyclesCheck.end(), [](bool value) {
            return value;
        });

        bool check = true;
        if (!piggyNumbers.empty()) {
            check = isInCycle(cycles, cyclesCheck, piggyNumbers[piggyNumbers.size() - 1]);
            piggyNumbers.pop_back();
        }

        if (allTrue || !check) {
            //* Wyświetlanie obrazka "win.png"q
            if (allTrue) RenderTexture(winTexture, SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 100);

            //* Wyświetlanie obrazka "over.png"
            if (!check) RenderTexture(overTexture, SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 140);

            // Renderowanie na ekranie
            SDL_RenderPresent(renderer);

            // Zwolnienie zasobów
            SDL_DestroyTexture(winTexture);
            SDL_DestroyTexture(overTexture);

            SDL_Delay(2000);
        }
        
    }

    CloseSDL();

    return 0;
}