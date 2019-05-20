#ifndef BALLROW_H
#define BALLROW_H

#include <vector>
#include "point3D.h"
#include "ball.h"
#include "drawable.h"

class BallRow: public Drawable
{
private:
    /* Pocetak i kraj reda */
    Point3D start, end;

    /* Referentna tacka */
    Point3D parent;

    /* Kapacitet - koliko loptica staje u red */
    unsigned int capacity;

    /* Razmak izmedju loptica unutar reda */
    float offset;

    /* Brzina kretanja loptica */
    float speed;

    /* Radijus loptica */
    float ball_radius;

    /* Fleg koji oznacava da se lopte unistavaja kada stignu do kraja */
    bool destroy_at_end;

    /* Loptice */
    std::vector<Ball*> balls;

    /* Pozicije u redu */
    std::vector<Point3D> positions;

public:
    BallRow(
        const Point3D& s, 
        const Point3D& e, 
        unsigned int capacity = 3, 
        float offset = 0, 
        float speed = 2);

    virtual ~BallRow();

    /* Azuriranje */
    void init_positions(void);
    void update(void);
    void update_vel(void);
    void reset(void);

    /* Crtanje */
    void draw_object(void) const override;

    /* Dodavanje i unistavanje loptica */
    void add_ball(Ball* ball);
    void spawn_ball(void);
    void destroy_first_ball(void);

    /* Transfer */
    void transfer_ball_from(BallRow& other);
    Ball* pop_first_ball(void);
    Ball* get_first_ball(void);
    Ball* get_last_ball(void);

    /* Pomocne funkcije */
    bool is_in_row(Ball* ball) const;
    bool all_balls_in_row(void) const;
    bool transfer_ready(void) const;
    bool is_empty(void) const;
    bool is_full(void) const;
    int space_left(void) const;

    /* Seteri */
    void set_capacity(unsigned int num);
    void set_ball_radius(float radius);
    void set_parent(const Point3D& p);
    void set_offset(float dist);
    void set_speed(float s);
    void set_start(const Point3D& s);
    void set_end(const Point3D& e);
    void set_start_end(const Point3D& s, const Point3D& e);
    void set_destroy_at_end(bool destroy);

    Point3D& get_start(void);
    Point3D& get_end(void);
    
    /* Debagovanje */
    void print_positions(void) const;
};

#endif
