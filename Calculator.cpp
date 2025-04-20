#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h> // For rounded box drawing
#include <iostream>
#include <vector>

extern "C" {
    #include "Model.c" // Logic for the calculator
}

enum class ButtonType { Number, Operator, Function, Equal };

struct Button {
    SDL_Rect rect;            // The position and size of the button
    SDL_Color color;          // The color of the button
    SDL_Color highlightColor; // The color when the button is hovered
    SDL_Color textColor;      // The color of the button text
    std::string text;         // The text displayed on the button
    ButtonType type;          // The type of button (Number, Operator, etc.)
};

// SDL Window and Renderer
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// Font objects for text rendering
TTF_Font* font = nullptr;
TTF_Font* smallFont = nullptr;
TTF_Font* bigFont = nullptr;

// Global state variables
bool wasMousePressed = false;  // Tracks the state of mouse clicks
std::string resultText = "0";  // The result displayed on the screen
bool running = true;           // Flag to keep the main loop running
int width = 512;               // Window width
int height = 768;              // Window height

// Function declarations
void renderText(Button& button);
void renderButtons(std::vector<Button>& buttons);
void renderResult(const std::string& input, const std::string& result);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Create the window
    window = SDL_CreateWindow("Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load fonts
    font = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 28);
    smallFont = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 20);
    bigFont = TTF_OpenFont("Graphics/Arial Unicode MS Regular.ttf", 36);

    // Check if fonts loaded successfully
    if (!font || !smallFont || !bigFont) {
        std::cerr << "Font loading error: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Set the window icon
    SDL_Surface* icon = SDL_LoadBMP("Graphics/CalculatorIcon.bmp");
    if (icon) {
        SDL_SetWindowIcon(window, icon);
        SDL_FreeSurface(icon);
    }

    // Define the colors used in the UI
    SDL_Color bgColor = { 9, 10, 20, 255 };
    SDL_Color numColor = { 32, 46, 55, 255 };
    SDL_Color opColor = { 190, 119, 43, 255 };
    SDL_Color fnColor = { 165, 48, 48, 255 };
    SDL_Color eqColor = { 117, 167, 67, 255 };
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color highlightColor = { 73, 73, 73, 255 };

    // Button layout settings
    const int buttonRows = 6;
    const int buttonCols = 4;
    const int displayHeight = (height / buttonRows) - 10;
    const int buttonWidth = (width / buttonCols) - 10;
    const int buttonHeight = (height / buttonRows) - 10;

    // Define the button labels
    std::vector<std::string> labels = {
        "^", "(", ")", "/",
        "7", "8", "9", "*",
        "4", "5", "6", "-",
        "1", "2", "3", "+",
        "0", ".", "⌫", "="
    };

    std::vector<Button> buttons;

    // Add a blank button at the top for result display
    buttons.push_back({
        { 0, 0, width, displayHeight },
        bgColor, bgColor, textColor,
        "", ButtonType::Function
    });

    // Create the calculator buttons
    for (int i = 0; i < labels.size(); ++i) {
        int row = i / buttonCols;
        int col = i % buttonCols;

        std::string label = labels[i];
        ButtonType type = ButtonType::Number;

        // Define button types based on label
        if (label == "=") type = ButtonType::Equal;
        else if (label == "AC" || label == "⌫") type = ButtonType::Function;
        else if (label == "+" || label == "-" || label == "*" || label == "/" || label == "(" || label == ")" || label == "^") type = ButtonType::Operator;
        else if (!isdigit(label[0]) && label != ".") type = ButtonType::Function;

        // Define button color based on type
        SDL_Color color = numColor;
        if (type == ButtonType::Operator) color = opColor;
        else if (type == ButtonType::Function) color = fnColor;
        else if (type == ButtonType::Equal) color = eqColor;

        // Add the button to the list
        buttons.push_back({
            {
                col * buttonWidth + (8 * (col + 1)),
                displayHeight + row * buttonHeight + (8 * (row + 1)),
                buttonWidth,
                buttonHeight
            },
            color,
            highlightColor,
            textColor,
            label,
            type
        });
    }

    // Main game loop
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Handle mouse events
        int mx, my;
        Uint32 mouseState = SDL_GetMouseState(&mx, &my);
        bool isMousePressed = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

        // Check for button presses
        for (Button& button : buttons) {
            if (button.type == ButtonType::Function && &button == &buttons[0]) continue;

            if (mx >= button.rect.x && mx <= button.rect.x + button.rect.w &&
                my >= button.rect.y && my <= button.rect.y + button.rect.h) {
                if (isMousePressed && !wasMousePressed) {
                    std::string& input = buttons[0].text;
                    // Handle special button presses
                    if (button.text == "⌫" && !input.empty()) {
                        input.pop_back();
                    } else if (button.text == "AC") {
                        input = "";
                        resultText = "0";
                    } else if (button.text == "=") {
                        resultText = calculate(input.c_str());
                    } else {
                        input += button.text;
                    }
                }
            }
        }

        wasMousePressed = isMousePressed;

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);

        // Render the buttons and result text
        renderButtons(buttons);
        renderResult(buttons[0].text, resultText);

        // Update the window
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    TTF_CloseFont(font);
    TTF_CloseFont(smallFont);
    TTF_CloseFont(bigFont);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// Function to render buttons on the screen
void renderButtons(std::vector<Button>& buttons) {
    int mx, my;
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);
    bool isMousePressed = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

    // Loop through and render each button
    for (auto& button : buttons) {
        if (&button == &buttons[0]) continue;

        SDL_Color color = button.color;

        // Highlight button if mouse is hovering over it
        if (mx >= button.rect.x && mx <= button.rect.x + button.rect.w &&
            my >= button.rect.y && my <= button.rect.y + button.rect.h) {
            color = button.highlightColor;
        }

        // Draw rounded button background
        int radius = 12;
        roundedBoxRGBA(
            renderer,
            button.rect.x,
            button.rect.y,
            button.rect.x + button.rect.w,
            button.rect.y + button.rect.h,
            radius,
            color.r, color.g, color.b, color.a
        );

        // Render button text
        renderText(button);
    }
}

// Function to render text on buttons
void renderText(Button& button) {
    if (!button.text.empty()) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, button.text.c_str(), button.textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {
                button.rect.x + (button.rect.w - surface->w) / 2,
                button.rect.y + (button.rect.h - surface->h) / 2,
                surface->w, surface->h
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}

// Function to render the current input and result on the screen
void renderResult(const std::string& input, const std::string& result) {
    SDL_Color inputColor = { 200, 200, 200, 200 };
    SDL_Color resultColor = { 255, 255, 255, 255 };

    // Render the input string at the top
    if (!input.empty()) {
        SDL_Surface* inputSurface = TTF_RenderUTF8_Blended(smallFont, input.c_str(), inputColor);
        SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_Rect inputRect = { width - 20 - inputSurface->w, 20, inputSurface->w, inputSurface->h };
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
        SDL_FreeSurface(inputSurface);
        SDL_DestroyTexture(inputTexture);
    }

    // Render the result string below the input
    if (!result.empty()) {
        SDL_Surface* resultSurface = TTF_RenderUTF8_Blended(bigFont, result.c_str(), resultColor);
        SDL_Texture* resultTexture = SDL_CreateTextureFromSurface(renderer, resultSurface);
        SDL_Rect resultRect = { width - 20 - resultSurface->w, 60, resultSurface->w, resultSurface->h };
        SDL_RenderCopy(renderer, resultTexture, NULL, &resultRect);
        SDL_FreeSurface(resultSurface);
        SDL_DestroyTexture(resultTexture);
    }
}
