#include "formatter.h"

const char* formatter_vector2(Vector2 vector2)
{
    return TextFormat("Vector2(%f, %f)", vector2.x, vector2.y);
}

const char* formatter_rect(Rectangle rectangle)
{
    return TextFormat("Rectangle(%f, %f, %f, %f)", rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}

