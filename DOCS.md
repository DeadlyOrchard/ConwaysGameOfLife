# Components
### Grid
- Stores a 2 dimensional vector of boolean values which represent each cell's state in the world.
### InputData
- Stores an x and y postition which represent the column and row ( respectively ) of the current mouse position.

# Systems
### drawLayout
- Pipeline: default
- Phase: `flecs::OnStore`
- This system is responsible for drawing all of the cells while the user sets up the initial board state.
    - If the cell is alive, it get's filled in. If it's not, it get's an outline. 
### drawHover
- Pipeline: default
- Phase: `flecs::OnStore`
- This system is responsible for drawing a bright outline for whichever cell the mouse is currently hovering over.
### drawSim
- Pipeline: `Simulation`
- Phase: `Simulate`
- This system is responsible for drawing all of the cells during the `Simulate` phase of the program.
    - If the cell is alive, it get's filled in. Otherwise, nothing is drawn.
### logic
- Pipeline: `Simulation`
- Phase: `Simulate`
- This system is responsible for calculating a new board state and then updating all of the cells inside the `Grid` appropriately. 

# Pipelines
### Default
- The game logic does not execute in this pipeline.
- Instead, only rendering is done.
- Systems which run during this pipeline:
    - `drawLayout`, and `drawHover`

### Simulation
- This Pipeline as a custom phase: `Simulate`.
- This is the phase where game logic is executed and then rendered.
- Systems which run during this pipeline:
    - `drawSim`, and `logic`.

# Input Handling
- All inputs are handled in the main loop of the program. I haven't managed to find a way to turn this into a system, but I'll keep searching. In the meantime, 
    - Escape to quit, 
    - Spacebar to begin simulating,
    - Click to make hover cell alive