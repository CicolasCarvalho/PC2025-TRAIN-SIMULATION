#include "city.h"

void City_draw(City *city) {
    DrawCircle(city->x, city->y, 5, BLUE);
    // DrawText(city->name, city->x + 10, city->y + 10, 12, WHITE);
}
