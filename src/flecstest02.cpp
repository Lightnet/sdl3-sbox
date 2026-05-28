#include <iostream>
#include <flecs.h>

// 1. Define your component data structures
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    // 2. Initialize the Flecs world
    flecs::world ecs;

    // 3. Define a System to process entities with both Position and Velocity
    // Change [] to [&] to allow external variables like std::cout or local configurations
    ecs.system<Position, const Velocity>()
        .each([](Position& p, const Velocity& v) { // Change this line to [&]
            p.x += v.x;
            p.y += v.y;
            std::cout << "Entity moved to: (" << p.x << ", " << p.y << ")\n";
        });


    std::cout << "Simulating 3 frames of execution:\n";

    // 5. Run the main processing loop for 3 steps
    for (int i = 0; i < 3; ++i) {
        ecs.progress(); 
    }

    return 0;
}
