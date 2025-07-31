#include "world.h"

void draw_edges(World *world)
{
    for (int i = 0; i < world->city_capacity; i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (!world->edge[i][j].exists) continue;
            City *city1 = &world->cities[i];
            City *city2 = &world->cities[j];

            DrawLineEx((Vector2){.x = city1->x, .y = city1->y},
                        (Vector2){.x = city2->x, .y = city2->y}, 2,
                        world->edge[i][j].color);
        }
    }
}

World *World_new(int32_t city_count)
{
    World *world = malloc(sizeof(World));

    world->city_capacity = city_count;
    world->city_count = 0;

    world->cities = malloc(sizeof(City) * city_count);
    world->edge = malloc(sizeof(Edge *) * city_count);

    for (int i = 0; i < world->city_capacity; i++)
    {
        world->edge[i] = calloc(city_count, sizeof(Edge));
    }

    return world;
}

void World_free(World *world)
{
    free(world->cities);
    for (int i = world->city_capacity - 1; i >= 0; i--)
    {
        free(world->edge[i]);
    }
    free(world->edge);
    free(world);
}

void World_add_city(World *world, City city)
{
    // PRINT("adding city %s", city.name);
    // PRINT("x: %i, y: %i", city.x, city.y);
    // PRINT("production_rate: %i", city.production_rate);
    world->cities[world->city_count++] = city;
}

void World_draw(World *world, bool edges)
{
    for (int i = 0; i < world->city_count; i++)
    {
        City *city = &world->cities[i];
        City_draw(city);
    }

    if (edges)
        draw_edges(world);
}

void World_create_edges(World *world, Edge edge, int32_t from_idx, int32_t to_idx)
{
    world->edge[from_idx][to_idx] = edge;
    world->edge[to_idx][from_idx] = edge;
}