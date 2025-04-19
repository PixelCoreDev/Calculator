#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
extern "C" {
    #include "Model.c"
}

struct Button {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color colorHighlight;
    SDL_Color textColor;
    std::string text;
};

// SDL
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// Fonts
TTF_Font* font = nullptr;
TTF_Font* smallFont = nullptr;
TTF_Font* bigFont = nullptr;

// Global variables
bool wasMousePressed = false; 
std::string resultText = "0";
bool running = true;
int width = 512;
int height = 768;

// Function prototypes
void renderText(Button& button);
void renderButton(std::vector<Button>& buttons);
void renderResultText(const std::string& result);
void renderCalculationText(Button& button);

int main(int argc, char* argv[]) {
    #pragma region InitializeSystems
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL couldn't be Initialized! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    // create window and renderer
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
    
    // Load TTF
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return 1;
    }
    // Load font
    font = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 32); // 32 is the font size
    smallFont = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 24); // 24 is the font size for smaller text
    bigFont = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 48); // 48 is the font size for larger text
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }
    if (!smallFont) {
        std::cerr << "Failed to load smallFont: " << TTF_GetError() << std::endl;
        return 1;
    }
    if (!bigFont) {
        std::cerr << "Failed to load bigFont: " << TTF_GetError() << std::endl;
        return 1;
    }
    #pragma endregion
    
    #pragma region CreateLayout
    //Create Layout
    
    // Set Colors
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color buttonColor = { 50, 50, 50, 255 };
    SDL_Color buttonColorHighlight = { 73, 73, 73, 255 };
    SDL_Color bgColor = { 32, 32, 32, 255 };

    // Set Button Dimensions
    const int rows = 6;
    const int cols = 4;

    std::vector<std::string> labels = {
        "(", ")", "^", "/",
        "7", "8", "9", "*",
        "4", "5", "6", "+",
        "1", "2", "3", "-",
        "0", "⌫", ".", "="
    };

    
    std::vector<Button> buttons;
    // Create TopBar
    buttons.push_back({{ 0, 0, width, height/rows}, bgColor, bgColor, textColor, "" });
    // Create Buttons
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
    #pragma endregion

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit when the window is closed
                break;
            }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);
        	
        //  Render the Scene
        renderButton(buttons);
        renderResultText(resultText);

        SDL_RenderPresent(renderer);
    }
    
    #pragma region Quit
    TTF_CloseFont(font);
    TTF_CloseFont(bigFont);
    TTF_CloseFont(smallFont);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
    #pragma endregion
}
void renderButton(std::vector<Button>& buttons) {
    // get mouse state
    int x, y;
    Uint32 mouseState = SDL_GetMouseState(&x, &y);
    bool isMousePressed = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

    for (Button& button : buttons) {
        // Check if the mouse is inside the button rect
        if (x >= button.rect.x && x < button.rect.x + button.rect.w &&
            y >= button.rect.y && y < button.rect.y + button.rect.h) {
            // Change color to highlight
            SDL_SetRenderDrawColor(renderer, button.colorHighlight.r, button.colorHighlight.g, button.colorHighlight.b, button.colorHighlight.a);
    
            // Handle "just pressed" logic
            if (isMousePressed && !wasMousePressed) {
                if (button.text == "=") {
                    resultText = calculate(buttons[0].text.c_str()); // Call the calculate function from Model.c
                    
                } else if (button.text == "⌫") {
                    // Handle delete logic
                    if (!buttons[0].text.empty()) {
                        // Remove the last character
                        buttons[0].text.pop_back(); 
                    }
                } else if (button.text != buttons[0].text) {
                    // Add the button's text to the input
                    buttons[0].text += button.text; 
                }
            }
        } else {
            // Reset color to default
            SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, button.color.a);
        }
        // Render the button
        SDL_RenderFillRect(renderer, &button.rect); 


        if (button.text == buttons[0].text) {
            // Render the calculation text
            renderCalculationText(button); 
        }
        else {
            // Render the button text
            renderText(button); 
        }
    }

    wasMousePressed = isMousePressed;
}

void renderText(Button& button){
    if (!button.text.empty()) {
        // create surface with text
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, button.text.c_str(), button.textColor);
        if (textSurface) {
            // create texture from surface
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            // create rect with surface dimensions
            SDL_Rect textRect = {
                button.rect.x + (button.rect.w - textSurface->w) / 2,
                button.rect.y + (button.rect.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            // Render the button text
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            // Free the surface and texture
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        } else {
            std::cerr << "Failed to render text: " << TTF_GetError() << std::endl;
        }
    }
}

void renderCalculationText(Button& button) {
    SDL_Color textColor = { 235, 235, 235, 220 };

    // create surface with text
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(smallFont, button.text.c_str(), textColor);
    if (textSurface) {
        // create texture from surface
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        // create rect with surface dimensions
        SDL_Rect textRect = { width - 25, (height / 6)/8, textSurface->w, textSurface->h };  // Position the result text at the top right corner
        textRect.x -= textRect.w;  // Adjust x position to align right
        // Render the button text
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        // Free the surface and texture
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

void renderResultText(const std::string& result) {
    SDL_Color textColor = { 255, 255, 255, 255 };

    // create surface with text
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(bigFont, result.c_str(), textColor);
    if (textSurface) {
        // create texture from surface
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        // create rect with surface dimensions
        SDL_Rect textRect = { width - 25, (height / 6)/3 + 5, textSurface->w, textSurface->h };  // Position the result text at the top right corner
        textRect.x -= textRect.w;  // Adjust x position to align right
        // Render the button text
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        // Free the surface and texture
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

