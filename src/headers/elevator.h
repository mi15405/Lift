#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <vector>
#include "floor.h"
#include "ball.h"
#include "point3D.h"
#include "movable.h"
#include "drawable.h"
#include "bridge.h"
#include "ballRow.h"

typedef enum 
{ 
    MOVING_UP,               /* Faza kretanja na gore */
    MOVING_DOWN,             /* Faza kretanja na dole */
    STOP_MOVING              /* Faza zaustavljanja */
} Movement;

typedef enum 
{ 
    ALIGN_POSITION,   /* Vertikalno poravnanje lifta sa spratom */
    LOWER_BRIDGE,     /* Spustanje mosta */
    LOADING,          /* Utovar loptica */    
    UNLOADING,        /* Istovar loptica */
    LOADED,           /* Loptice su prebacene */
    TRANSFER_DONE              /* Most je podignut, transfer je zavrsen */
} Transfer;

class Elevator: public Movable, public Drawable
{
private:
    /* Dimenzije lifta */
    float length; 
    float height; 
    float width;

    /* Trenutni i maksimalni nivo lifta */
    unsigned int level;
    unsigned int max_level;

    /* Osvojeni poeni */
    unsigned int points;

    /* Brzine: */
    float load_speed;      /* Transfer loptica */
    float move_speed;      /* Kretanje */
    float min_move_speed;  /* Minimalna brzina kretanja */
    float acc_speed;       /* Ubrzanje */

    /* Granice kretanja po Y osi */
    float min_y; 
    float max_y;

    /* Trenutna faza kretanja */
    Movement movement;
    /* Trenutna faza transfera */
    Transfer transfer;

    /* Kapacitet jednog reda loptica */
    int row_capacity;

    /* Redovi loptica */
    std::vector<BallRow*> ball_rows;     

    /* Mostovi */
    Bridge left_bridge;
    Bridge right_bridge;

    /* Sprat sa kojim se trenutno vrsi transfer loptica */
    Floor* transfer_floor;

    /* Most koji je trenutno u funkciji */
    Bridge* active_bridge;

public:
    Elevator();
    ~Elevator();

    /* Nasledjene metode */
    void draw_object(void) const override;
    void update(void) override;

    /* Transfer loptica */
    void start_transfer(std::vector<Floor*> floors);
    void cancel_transfer(void); 

    /* Vraca lift na pocetne vrednosti */
    void reset(void);

    /* Seteri */
    void set_movement(Movement m);
    void set_min_y(float y);
    void set_max_y(float y);

    /* Geteri */
    unsigned int get_points(void) const;

private:
    /* Inicijalizacija */
    void init(void);
    void init_bridges(void);

    /* Kretanje */
    void update_movement(void);
    void move(void);
    void slow_down(void);
    void limit_velocity_and_position(void);

    /* Transfer loptica: */
    void update_transfer(void);
    bool is_on_same_height(Floor* floor);  
    void align_with_floor(Floor* floor);
    void transfer_balls_inside(void);
    void transfer_balls_outside(void);
    bool all_balls_inside(void);
    bool any_space_left(void) const;
    bool all_rows_empty(void) const;

    /* Odabir reda loptica za transfer */
    BallRow* pick_row_to_load(void);
    BallRow* emptiest_row(void);
    BallRow* same_last_color_row(Color& ball_color);

    /* Unapredjenje lifta: */
    void update_level(void);
    void arange_rows(void);
    void expand_size(void);
    void add_ball_row(void);    
    void increase_speed(void);

    /* Pomocne funkcije */
    float get_vertical_distance(Floor* floor);
    Floor* find_nearest_floor(std::vector<Floor*> floors);
    unsigned int get_total_capacity(void) const;
};

#endif
