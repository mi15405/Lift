#ifndef FLOOR_H
#define FLOOR_H

#include <vector>
#include "drawable.h"
#include "ball.h"
#include "point3D.h"
#include "ballRow.h"

class Floor: public Drawable
{
private:
    /* Duzina sprata */
    float length;

    /* Boja sprata */
    Color color;

    /* Red loptica */
    BallRow ball_row;

    /* Random broj koji sluzi za animaciju, nista posebno */
    double random_num;

public:
    Floor(float length, unsigned int capacity, Color color = NO_COLOR);
    virtual ~Floor();

    /* Crtanje */
    void draw_object(void) const override;
    void draw_portal(void) const;

    /* Azuriranje */
    void update(void);
    void reset(void);

    /* Stvaranje loptica */
    void spawn_ball(void);

    /* Pomocne funkcije */
    bool is_empty(void) const;
    bool is_full(void) const;
    bool all_balls_inside(void) const;

    /* Geteri */
    BallRow& get_ball_row(void);
    Color get_color(void) const;
};

#endif
