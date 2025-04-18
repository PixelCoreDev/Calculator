#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

struct Button {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color colorHighlight;
    SDL_Color textColor;
    std::string text;
};

int main(int argc, char* argv[]) {

    bool running = true;
    int width = 512;
    int height = 768;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL couldn't be Initialized! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could't be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer couldn't be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 24); // 24 is the font size
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    /*
    Create Layout
    */
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color buttonColor = { 50, 50, 50, 255 };
    SDL_Color buttonColorHighlight = { 73, 73, 73, 255 };
    SDL_Color bgColor = { 32, 32, 32, 255 };

    std::vector<Button> buttons;
    buttons.push_back({{ 0, 0, width, height/6}, bgColor, bgColor, textColor, "" });
    buttons.push_back({{ (width/4) * 0, (height/6) * 1, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "(" });
    buttons.push_back({{ (width/4) * 1, (height/6) * 1, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, ")" });
    buttons.push_back({{ (width/4) * 2, (height/6) * 1, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "^" });
    buttons.push_back({{ (width/4) * 3, (height/6) * 1, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "/" });
    
    buttons.push_back({{ (width/4) * 0, (height/6) * 2, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "7" });
    buttons.push_back({{ (width/4) * 1, (height/6) * 2, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "8" });
    buttons.push_back({{ (width/4) * 2, (height/6) * 2, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "9" });
    buttons.push_back({{ (width/4) * 3, (height/6) * 2, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "*" });
    
    buttons.push_back({{ (width/4) * 0, (height/6) * 3, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "4" });
    buttons.push_back({{ (width/4) * 1, (height/6) * 3, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "5" });
    buttons.push_back({{ (width/4) * 2, (height/6) * 3, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "6" });
    buttons.push_back({{ (width/4) * 3, (height/6) * 3, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "+" });
    
    buttons.push_back({{ (width/4) * 0, (height/6) * 4, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "1" });
    buttons.push_back({{ (width/4) * 1, (height/6) * 4, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "2" });
    buttons.push_back({{ (width/4) * 2, (height/6) * 4, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "3" });
    buttons.push_back({{ (width/4) * 3, (height/6) * 4, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "-" });
    
    buttons.push_back({{ (width/4) * 0, (height/6) * 5, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "0" });
    buttons.push_back({{ (width/4) * 1, (height/6) * 5, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "⌫" });
    buttons.push_back({{ (width/4) * 2, (height/6) * 5, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "," });
    buttons.push_back({{ (width/4) * 3, (height/6) * 5, width/4, height/6}, buttonColor, buttonColorHighlight, textColor, "=" });

    
    bool wasMousePressed = false; // Tracks the previous state of the left mouse button

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit when the window is closed
                break;
            }
        }

        // Get the current mouse state
        int x, y;
        Uint32 mouseState = SDL_GetMouseState(&x, &y);

        // Check if the left mouse button is currently pressed
        bool isMousePressed = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

        for (Button& button : buttons) { // Check if the mouse is inside the button rect
            if (x >= button.rect.x && x < button.rect.x + button.rect.w &&
                y >= button.rect.y && y < button.rect.y + button.rect.h) {
                SDL_SetRenderDrawColor(renderer, button.colorHighlight.r, button.colorHighlight.g, button.colorHighlight.b, button.colorHighlight.a);

                // Handle "just pressed" logic
                if (isMousePressed && !wasMousePressed) {
                    if (button.text == "=") {
                        // Handle "=" logic
                    } else if (button.text == "⌫") {
                        // Handle delete logic
                        if (!buttons[0].text.empty()) {
                            buttons[0].text.pop_back(); // Remove the last character
                        }
                    } else if (button.text != buttons[0].text) {
                        buttons[0].text += button.text; // Add the button's text to the input
                    }
                }
            } else {
                SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, button.color.a);
            }

            SDL_RenderFillRect(renderer, &button.rect);

            if (!button.text.empty()) {
                // Use TTF_RenderText_Blended for better font resolution
                SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, button.text.c_str(), button.textColor);
                if (textSurface) {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_Rect textRect = {
                        button.rect.x + (button.rect.w - textSurface->w) / 2,
                        button.rect.y + (button.rect.h - textSurface->h) / 2,
                        textSurface->w,
                        textSurface->h
                    };
                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_FreeSurface(textSurface);
                    SDL_DestroyTexture(textTexture);
                } else {
                    std::cerr << "Failed to render text: " << TTF_GetError() << std::endl;
                }
            }
        }

        // Update the previous mouse state
        wasMousePressed = isMousePressed;

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}