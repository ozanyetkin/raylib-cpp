#include <raylib-cpp.hpp>
#include <vector>

// Define screen dimensions
const int screenWidth = 1425;
const int screenHeight = 850;

// Pac-Man structure
struct Pacman
{
    Vector2 position;    // Pac-Man's position
    float drawSize;      // Size used for drawing Pac-Man
    float colliderSize;  // Size used for collision detection (height)
    float colliderWidth; // Width adjustment for the collider
    float buffer;        // Distance to keep from walls
    Vector2 speed;       // Speed vector
    Color color;

    Pacman(float startX, float startY, float drawS, float collSize, float widthAdj, float buf, Color c)
        : position({startX + 10, startY + 10}), drawSize(drawS), colliderSize(collSize), colliderWidth(widthAdj), buffer(buf), speed({2, 0}), color(c) {}

    // Update Pacman's position and handle collision
    void Update(const std::vector<std::vector<int>> &maze, int tileSize)
    {
        // Calculate the next position based on speed
        Vector2 newPos = {position.x + speed.x, position.y + speed.y};

        // Create a rectangle for Pac-Man's square collider with a buffer
        Rectangle pacmanRect = {newPos.x - (colliderSize / 2) - buffer, newPos.y - (colliderSize / 2) - buffer,
                                colliderSize + 2 * buffer + colliderWidth, colliderSize + 2 * buffer};

        // Ensure Pacman stays in bounds and doesn't collide with walls
        bool collisionDetected = false;
        if (newPos.x < 50 + colliderSize / 2 + buffer || newPos.x > screenWidth - (50 + colliderSize / 2 + buffer) ||
            newPos.y < 100 + colliderSize / 2 + buffer || newPos.y > screenHeight - (100 + colliderSize / 2 + buffer))
        {
            collisionDetected = true;
        }
        else
        {
            for (size_t y = 0; y < maze.size(); ++y)
            {
                for (size_t x = 0; x < maze[y].size(); ++x)
                {
                    if (maze[y][x] == 1)
                    {
                        Rectangle wallRect = {static_cast<float>(x * tileSize) + 50, static_cast<float>(y * tileSize) + 100, static_cast<float>(tileSize), static_cast<float>(tileSize)};
                        if (CheckCollisionRecs(pacmanRect, wallRect))
                        {
                            collisionDetected = true;
                            break;
                        }
                    }
                }
                if (collisionDetected)
                    break;
            }
        }

        // Update position only if there's no collision
        if (!collisionDetected)
        {
            position = newPos;
        }

        // Draw Pacman
        DrawCircleV(position, drawSize / 2, color);
    }
};

// Check if moving in a given direction would result in a collision
bool CanMoveInDirection(const Pacman &pacman, const std::vector<std::vector<int>> &maze, Vector2 direction, int tileSize)
{
    Vector2 newPos = {pacman.position.x + direction.x, pacman.position.y + direction.y};

    // Create a rectangle for Pac-Man's square collider with a buffer
    Rectangle pacmanRect = {newPos.x - (pacman.colliderSize / 2) - pacman.buffer, newPos.y - (pacman.colliderSize / 2) - pacman.buffer,
                            pacman.colliderSize + 2 * pacman.buffer + pacman.colliderWidth, pacman.colliderSize + 2 * pacman.buffer};

    // Check if the new position will collide with a wall
    if (newPos.x < 50 + pacman.colliderSize / 2 + pacman.buffer || newPos.x > screenWidth - (50 + pacman.colliderSize / 2 + pacman.buffer) ||
        newPos.y < 100 + pacman.colliderSize / 2 + pacman.buffer || newPos.y > screenHeight - (100 + pacman.colliderSize / 2 + pacman.buffer))
    {
        return false; // Out of bounds
    }

    for (size_t y = 0; y < maze.size(); ++y)
    {
        for (size_t x = 0; x < maze[y].size(); ++x)
        {
            if (maze[y][x] == 1)
            {
                Rectangle wallRect = {static_cast<float>(x * tileSize) + 50, static_cast<float>(y * tileSize) + 100, static_cast<float>(tileSize), static_cast<float>(tileSize)};
                if (CheckCollisionRecs(pacmanRect, wallRect))
                {
                    return false; // Collision with wall
                }
            }
        }
    }

    return true; // No collision
}

int main()
{
    // Initialize the window
    raylib::Window window(screenWidth, screenHeight, "Simple Maze with Pac-Man");

    // Maze settings
    const int tileSize = 25; // Size of each tile (in pixels)

    // Simple maze layout (1 = wall, 0 = path)
    std::vector<std::vector<int>> maze = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    };

    // Initialize Pac-Man (start moving to the right)
    Pacman pacman(100, 140, tileSize * 1.5f, tileSize * 1.5f, 2.0f, 5, YELLOW); // drawing size, collider size, width adjustment, buffer

    // Maze rendering function (only draw walls, skip paths)
    auto drawMaze = [&maze, tileSize]()
    {
        for (size_t y = 0; y < maze.size(); ++y)
        {
            for (size_t x = 0; x < maze[y].size(); ++x)
            {
                if (maze[y][x] == 1)
                { // Only draw rectangles for walls (1)
                    Color color = GREEN;
                    DrawRectangle(x * tileSize + 50, y * tileSize + 100, tileSize, tileSize, color);
                }
            }
        }
    };

    window.SetTargetFPS(60); // Set the desired FPS

    // Main game loop
    while (!window.ShouldClose())
    { // Check if the window should close
        // Handle input for Pac-Man movement
        if (IsKeyDown(KEY_RIGHT) && CanMoveInDirection(pacman, maze, {2, 0}, tileSize))
            pacman.speed = {2, 0};
        if (IsKeyDown(KEY_LEFT) && CanMoveInDirection(pacman, maze, {-2, 0}, tileSize))
            pacman.speed = {-2, 0};
        if (IsKeyDown(KEY_DOWN) && CanMoveInDirection(pacman, maze, {0, 2}, tileSize))
            pacman.speed = {0, 2};
        if (IsKeyDown(KEY_UP) && CanMoveInDirection(pacman, maze, {0, -2}, tileSize))
            pacman.speed = {0, -2};

        // Update Pac-Man
        pacman.Update(maze, tileSize);

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        drawMaze();
        pacman.Update(maze, tileSize);
        EndDrawing();
    }

    return 0;
}
