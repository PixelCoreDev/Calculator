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

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
bool wasMousePressed = false; // Tracks the previous state of the left mouse button
std::string resultText = "0";  // Variable to hold the result text
bool running = true;
int width = 512;
int height = 768;

void renderText(Button& button);
void renderButton(std::vector<Button>& buttons);
void renderResultText(const std::string& result);
void renderCalculationText(Button& button);

int main(int argc, char* argv[]) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL couldn't be Initialized! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could't be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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

    font = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 32); // 32 is the font size
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

    const int rows = 6;
    const int cols = 4;

    std::vector<std::string> labels = {
        "(", ")", "^", "/",
        "7", "8", "9", "*",
        "4", "5", "6", "+",
        "1", "2", "3", "-",
        "0", "⌫", ",", "="
    };

    
    std::vector<Button> buttons;
    buttons.push_back({{ 0, 0, width, height/rows}, bgColor, bgColor, textColor, "" });
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            buttons.push_back({{ 
                (width/cols) * i,
                (height/rows) * (j + 1),
                width/cols, height/rows},
                buttonColor,
                buttonColorHighlight,
                textColor, labels[(j * 4) + i] 
            });
        }
    }
    

    

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit when the window is closed
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);

        // Get the current mouse state

        // Check if the left mouse button is currently pressed

        renderButton(buttons);
        renderResultText(resultText);

        // Update the previous mouse state

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void renderText(Button& button){
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

void renderButton(std::vector<Button>& buttons) {
    int x, y;
    Uint32 mouseState = SDL_GetMouseState(&x, &y);
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
        if (button.text == buttons[0].text) {
            renderCalculationText(button); // Render the calculation text
        }
        else {
            renderText(button); // Render the button text
        }
        
        
    }

    wasMousePressed = isMousePressed;
}

void renderCalculationText(Button& button) {
    SDL_Color textColor = { 235, 235, 235, 220 };
    TTF_Font* resultFont = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 24);  // Smaller font for the result text
    if (!resultFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(resultFont, button.text.c_str(), textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = { width - 25, (height / 6)/8, textSurface->w, textSurface->h };  // Position the result text at the top right corner
        textRect.x -= textRect.w;  // Adjust x position to align right
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    TTF_CloseFont(resultFont);
}

void renderResultText(const std::string& result) {
    SDL_Color textColor = { 255, 255, 255, 255 };
    TTF_Font* resultFont = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 48);  // Smaller font for the result text
    if (!resultFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(resultFont, result.c_str(), textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = { width - 25, (height / 6)/3 + 5, textSurface->w, textSurface->h };  // Position the result text at the top right corner
        textRect.x -= textRect.w;  // Adjust x position to align right
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    TTF_CloseFont(resultFont);
}