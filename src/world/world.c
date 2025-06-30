#include "world.h"

void draw_railways(World *world)
{
    for (int i = 0; i < world->city_capacity; i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (world->railway[i][j].distance <= 0) continue;
            City *city1 = &world->cities[i];
            City *city2 = &world->cities[j];

            if (LINE_TYPE == 0)
            {
                DrawLineEx((Vector2){.x = city1->x, .y = city1->y},
                           (Vector2){.x = city2->x, .y = city2->y}, 1,
                           world->railway[i][j].color);
            }
            else if (LINE_TYPE == 1)
            {
                DrawLineBezier((Vector2){.x = city1->x, .y = city1->y},
                               (Vector2){.x = city2->x, .y = city2->y}, 1,
                               world->railway[i][j].color);
            }
            else if (LINE_TYPE == 2)
            {
                int32_t weight_x = ((city2->x - city1->x) / SPLINE_WEIGHT);
                int32_t weight_y = ((city2->y - city1->y) / SPLINE_WEIGHT);

                Vector2 points[3] = {
                    (Vector2){.x = city1->x, .y = city1->y},
                    (Vector2){.x = (city1->x + city2->x) / 2 - weight_y, .y = (city1->y + city2->y) / 2 - weight_x},
                    (Vector2){.x = city2->x, .y = city2->y}};
                DrawSplineBezierQuadratic(points, 3, 1, world->railway[i][j].color);
            }
            else if (LINE_TYPE == 3)
            {
                int32_t weight_x = ((city2->x - city1->x) / SPLINE_WEIGHT);
                int32_t weight_y = ((city2->y - city1->y) / SPLINE_WEIGHT);

                Vector2 points[4] = {
                    (Vector2){.x = city1->x, .y = city1->y},
                    (Vector2){.x = (city1->x + city2->x) / 2 - weight_y, .y = (city1->y + city2->y) / 2 - weight_x},
                    (Vector2){.x = (city1->x + city2->x) / 2 + weight_y, .y = (city1->y + city2->y) / 2 + weight_x},
                    (Vector2){.x = city2->x, .y = city2->y}};
                DrawSplineBezierCubic(points, 4, 1, world->railway[i][j].color);
            }
        }
    }
}

World *World_new(int32_t city_count)
{
    World *world = malloc(sizeof(World));

    world->city_capacity = city_count;
    world->city_count = 0;

    world->cities = malloc(sizeof(City) * city_count);
    world->railway = malloc(sizeof(Railway) * city_count);

    for (int i = 0; i < world->city_capacity; i++)
    {
        world->railway[i] = calloc(city_count, sizeof(Railway));
    }

    return world;
}

void World_free(World *world)
{
    free(world->cities);
    for (int i = world->city_capacity - 1; i >= 0; i--)
    {
        free(world->railway[i]);
    }
    free(world->railway);
    free(world);
}

void World_add_city(World *world, City city)
{
    // PRINT("adding city %s", city.name);
    // PRINT("x: %i, y: %i", city.x, city.y);
    // PRINT("production_rate: %i", city.production_rate);
    world->cities[world->city_count++] = city;
}

void World_draw(World *world)
{
    for (int i = 0; i < world->city_count; i++)
    {
        City *city = &world->cities[i];
        City_draw(city);
    }

    draw_railways(world);
}

void World_create_railways(World *world, Railway railway, int32_t from_idx, int32_t to_idx)
{
    world->railway[from_idx][to_idx] = railway;
    world->railway[to_idx][from_idx] = railway;
}