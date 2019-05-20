#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>
#include "ball.h"

Ball::Ball(double radius, Color draw_color)
    : radius(radius), color(draw_color)
{
    /* Ucitavam rgb vrednosti iz boje */
    float rgb[3];
    draw_color.set_rgb_array(rgb);
   
    /* Postavljam parametre za materijal lopte */
    Material mat(
        rgb[0], rgb[1], rgb[2], 1,
        0.2, 0.2, 0.2, 1,
        1, 1, 0, 1,
        50
    );

    /* Postavljam materijal */
    set_material(mat);
}

Ball::~Ball()
{}

void Ball::draw_object(void) const
{
    /* Iscrtava se sfera */
    glutSolidSphere(radius, 20, 20);
}

void Ball::update(void)
{
    /*Azuriraju se pozicija lopte i prosledjuje klasi za iscrtavanje */
    Movable::update();
    Drawable::set_trans(get_pos());
}

/* Pomocne funkcije */
Ball::operator const char*()
{
    std::ostringstream stream;
    stream << radius << std::endl;
    stream << "Movable: " << std::endl;
    stream << get_movable() << std::endl;
    stream << "Color: " << color.get_rgb() << std::endl;
    to_string = stream.str();
    return to_string.c_str();
}

/* Seteri */
void Ball::set_radius(double rad) { radius = rad; }

/* Geteri */
double Ball::get_radius(void) const { return radius; }

Color Ball::get_color(void) const { return color; }
