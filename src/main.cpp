#include <flecs.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

struct Grid {
    std::vector<std::vector<bool>> states;
};

struct InputData {
    int x, y;
};

struct Simulate {  };

// constants
const int x = SDL_WINDOWPOS_UNDEFINED;
const int y = x;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;
const int SPACE = 20; // distance between cells
const int COLUMNS = SCREEN_WIDTH / SPACE;
const int ROWS = SCREEN_HEIGHT / SPACE;

int main(int argc, char *argv[]) {
    // create window and renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS, &window, &renderer);
    if (window == nullptr) {
        std::cout << "Could not create SDL2 window" << std::endl;
        return -1;
    } else {
        std::cout << "Successfully created SDL2 window" << std::endl;
    }
    if (renderer == nullptr) {
        std::cout << "Could not create SDL2 renderer" << std::endl;
        return -1;
    } else {
        std::cout << "Successfully created SDL2 renderer" << std::endl;
    }

    flecs::world world(argc, argv);

    // simulation pipeline
    flecs::entity Simulation = world.pipeline()
        .with(flecs::System)
        .with<Simulate>()
        .build();

    // generate grid of cells
    flecs::entity cells = world.entity().set<Grid>({ });
    for (int r = 0; r < ROWS; r++) {
        std::vector<bool> row;
        for (int c = 0; c < COLUMNS; c++) {
            row.push_back( false );
        }
        Grid* g = cells.get_mut<Grid>();
        g->states.push_back(row);
    }

    
    // draw states while they're still being setup
    flecs::system drawLayout = world.system<const Grid>()
        .kind(flecs::OnStore)
        .each([renderer](const Grid &g) {
            SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            int r = 0;
            for (std::vector<bool> row : g.states) {
                int c = 0;
                for (bool alive : row) {
                    SDL_Rect rect = {c * SPACE, r * SPACE, SPACE, SPACE};
                    if (alive) { SDL_RenderFillRect(renderer, &rect); }
                    else { SDL_RenderDrawRect(renderer, &rect); }
                    c++;
                }
                r++;
            }
        });
    
    // draw whichever state the mouse is on
    flecs::entity mouse = world.entity("mouse").set<InputData>({0, 0}); // for tracking mouse movement
    flecs::system drawHover = world.system<const InputData>()
        .kind(flecs::OnStore)
        .each([renderer](const InputData &d) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect rect = {d.x * SPACE, d.y * SPACE, SPACE, SPACE};
            SDL_RenderDrawRect(renderer, &rect);
        });

    // draw simulation
    flecs::system drawSim = world.system<const Grid>()
        .kind<Simulate>()
        .each([renderer](const Grid &g) {
            SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
            int r = 0;
            for (std::vector<bool> row : g.states) {
                int c = 0;
                for (bool alive : row) {
                    SDL_Rect rect = {c * SPACE, r * SPACE, SPACE, SPACE};
                    if (alive) { SDL_RenderFillRect(renderer, &rect); }
                    c++;
                }
                r++;
            }
            SDL_Delay(50);
        });

    // game logic
    flecs::system logic = world.system<Grid>()
        .kind<Simulate>()
        .each([](Grid &g) {
            bool newStates[ROWS][COLUMNS]; // for holding each cell's new state
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLUMNS; c++) {
                    // find neighbors
                    // end value is not inclusive
                    int colStart, colEnd, rowStart, rowEnd; 
                    // initialize them with the most likely scenario
                    colStart = c - 1;
                    colEnd = c + 2;
                    rowStart = r - 1;
                    rowEnd = r + 2;

                    // make corrections if needed
                    if (colStart < 0) { colStart = c; }
                    else if (colEnd > COLUMNS) { colEnd = COLUMNS; }
                    if (rowStart < 0) { rowStart = r; }
                    else if (rowEnd > ROWS) { rowEnd = ROWS; }

                    // get neighbor states
                    std::vector<bool> neighbors;
                    for (int col = colStart; col < colEnd; col++) {
                        for (int row = rowStart; row < rowEnd; row++) {
                            if (col == c && row == r) { continue; } // skip current state
                            neighbors.push_back(g.states[row][col]);
                        }
                    }

                    // current cell's state
                    bool isAlive = g.states[r][c];
                    // iterator over neighbors
                    int liveNeighbors = 0;
                    for (bool alive : neighbors) {
                        if (alive) {
                            liveNeighbors++;
                        }
                    }

                    // calc new state
                    bool newState;
                    if (isAlive) {
                        newState = liveNeighbors == 2 || liveNeighbors == 3;
                    } else {
                        newState = liveNeighbors == 3;
                    }
                    newStates[r][c] = newState;
                }
            }

            // copy them over
            for (int c = 0; c < COLUMNS; c++) {
                for (int r = 0; r < ROWS; r++) {
                    g.states[r][c] = newStates[r][c];
                }
            }
        });

    // get inputs
    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;

                case SDLK_SPACE:
                    world.set_pipeline(Simulation);
                    break;
                }
                break;

            case SDL_MOUSEMOTION: {
                InputData* data = mouse.get_mut<InputData>();
                data->x = e.motion.x / SPACE;
                data->y = e.motion.y / SPACE;
                break;
                }

            case SDL_MOUSEBUTTONDOWN: {
                const InputData* data = mouse.get<InputData>();
                int r = data->y;
                int c = data->x;
                Grid* g = cells.get_mut<Grid>();
                g->states[r][c] = !g->states[r][c];
                break;
                }
            }
        }
        world.progress();
        SDL_RenderPresent(renderer);
    }
    return 0;
}