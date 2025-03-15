#include "Screen.h"

void Screen::makeScreen(int pixelToScreen, bool palleteSwap)
{
    _greenPallete = palleteSwap;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * pixelToScreen, SCREEN_HEIGHT * pixelToScreen, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!_greenPallete)
    {
        SDL_SetRenderDrawColor(renderer, COLOR_WHITE1);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, COLOR_WHITE2);
    }
    SDL_RenderClear(renderer);

    std::fill(frameBufferBefore, frameBufferBefore + SCREEN_WIDTH * SCREEN_HEIGHT, 0);
    std::fill(frameBufferAfter, frameBufferAfter + SCREEN_WIDTH * SCREEN_HEIGHT, 0);
}

void Screen::updateScreen(uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT])
{
    bool needsUpdate = false;

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        if (frameBuffer[i] != frameBufferBefore[i])
        {
            frameBufferAfter[i] = frameBuffer[i];
            needsUpdate = true;
        }
        else
        {
            frameBufferAfter[i] = frameBufferBefore[i];
        }
    }

    if (needsUpdate)
    {
        SDL_Rect box;
        box.w = 1;
        box.h = 1;

        // Clear the screen (black background)
        if (!_greenPallete)
        {
            SDL_SetRenderDrawColor(renderer, COLOR_WHITE1);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, COLOR_WHITE2);
        }
        SDL_RenderClear(renderer);

        // Now we loop through the frame buffer and draw pixels with the correct color
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        {
            // Choose the color based on the value in the frame buffer
            switch (frameBufferAfter[i])
            {
            case 3:
                // Black
                if (!_greenPallete)
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_BLACK1);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_BLACK2);
                }
                break;
            case 2:
                // Dark gray
                if (!_greenPallete)
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_DARK_GRAY1);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_DARK_GRAY2);
                }
                break;
            case 1:
                // Light gray
                if (!_greenPallete)
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_LIGHT_GRAY1);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_LIGHT_GRAY2);
                }
                break;
            case 0:
                // White
                if (!_greenPallete)
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_WHITE1);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, COLOR_WHITE2);
                }
                break;
            default:
                // If the frameBuffer contains any other value, treat it as red
                SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
                break;
            }

            // If the pixel should be drawn, render it
            if (frameBufferAfter[i] != 0)  // Assuming 0 means "empty", so don't draw that
            {
                box.x = (i % SCREEN_WIDTH);
                box.y = (i / SCREEN_WIDTH);
                SDL_RenderFillRect(renderer, &box);
            }
        }

        SDL_RenderPresent(renderer);

        std::swap(frameBufferBefore, frameBufferAfter);
    }
}

SDL_Window* Screen::getWindow()
{
    return window;
}

SDL_Renderer* Screen::getRenderer()
{
    return renderer;
}
