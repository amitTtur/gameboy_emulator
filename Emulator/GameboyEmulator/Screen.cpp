#include "Screen.h"

void Screen::makeScreen()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCREEN_TO_WORLD_PIXEL, SCREEN_HEIGHT * SCREEN_TO_WORLD_PIXEL, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);  // black background
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
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        // Now we loop through the frame buffer and draw pixels with the correct color
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        {
            // Choose the color based on the value in the frame buffer
            switch (frameBufferAfter[i])
            {
            case 0:
                // Black
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                break;
            case 1:
                // Dark grey
                SDL_SetRenderDrawColor(renderer, 0x25, 0x25, 0x25, 0xFF);
                break;
            case 2:
                // Light grey
                SDL_SetRenderDrawColor(renderer, 0xD0, 0xD0, 0xD0, 0xFF);
                break;
            case 3:
                // White
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                break;
            default:
                // If the frameBuffer contains any other value, treat it as black
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
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
