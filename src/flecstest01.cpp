#include <iostream>
#include <flecs.h>
#include <cstdio>

// 1. Define your component data structures
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

// 1. Define the standalone function
void MoveEntities(flecs::entity e, Position& p, const Velocity& v) {
    p.x += v.x;
    p.y += v.y;
    std::cout << e.name() << " moved to: (" << p.x << ", " << p.y << ")\n";
    // printf("loop...");
}

int main(int argc, char *argv[]) {
    // 2. Initialize the Flecs world
    flecs::world ecs;

    // 3. Define a System to process entities with both Position and Velocity
    // Change [] to [&] to allow external variables like std::cout or local configurations
    // ecs.system<Position, const Velocity>()
    //     .each([](Position& p, const Velocity& v) { // Change this line to [&]
    //         p.x += v.x;
    //         p.y += v.y;
    //         std::cout << "Entity moved to: (" << p.x << ", " << p.y << ")\n";
    //     });

    ecs.system<Position, const Velocity>("MoveSystem")
      .each(MoveEntities);

    // ─── ADD ENTITIES HERE ──────────────────────────────────────────
    // Create an entity named "Hero" with initial Position and Velocity
    ecs.entity("Hero")
        .set<Position>({0.0f, 0.0f})
        .set<Velocity>({1.5f, 2.0f});

    // Create a second entity named "Enemy"
    // ecs.entity("Enemy")
    //     .set<Position>({10.0f, 10.0f})
    //     .set<Velocity>({-1.0f, -1.0f});


    std::cout << "Simulating 3 frames of execution:\n";

    // 5. Run the main processing loop for 3 steps
    for (int i = 0; i < 3; ++i) {
      // printf("loop ..");
      ecs.progress(); 
    }

    printf("loop end");

    return 0;
}
