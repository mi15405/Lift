#ifndef BALL_H
#define BALL_H

#include <GL/glut.h>
#include <string>
#include "movable.h"
#include "drawable.h"
#include "color.h"

class Ball: public Movable, public Drawable
{
private:
    /* Radijus */
    double radius;

    /* Boja */
    Color color;

    /* String ispis lopte, za debagovanje */
    std::string to_string;
public:
    Ball(double radius = 0.1, Color color = NO_COLOR);
    virtual ~Ball();

    /* Nasledjene metode */
    void draw_object(void) const override;
    void update() override;
    
    /* Pomocne funkcije */
    operator const char*();
        
    /* Geteri */
    double get_radius(void) const;

    /* Seteri */
    void set_radius(double radius);
    Color get_color(void) const;
};

#endif
