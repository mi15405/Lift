#ifndef CAMERA_H
#define CAMERA_H

#include "point3D.h"
#include "movable.h"

class Camera: public Movable
{
private:
    Point3D direction;          /* Pravac pogleda */
    Point3D right_vector;       /* Pravac u desno */
    Point3D up_vector;          /* Pravac na gore */
    Point3D center;             /* Koordinata tacke u centru pogleda */

    double theta;               /* Horizontalna rotacija ( 0, 2PI) */
    double phi;                 /* Vertikalna rotacija ( 0, PI) */
    double move_speed;          /* Brzina kretanja */
    double rotation_speed;      /* Brzina rotacije */
    bool free_look;             /* Omogucena kontrola kamere, sluzi za debagovanje */
    bool is_following_target;   /* Fleg koji oznacava da kamera prati neki objekat  */

    Movable* target;            /* Objekat koji se prati */
    Point3D offset;             /* Rastojanje od objekta koji se prati */

public:
    Camera(double x = 0, double y = 0, double z = 0);

    void turn_on(void);
    void fixate_look(const Point3D& look);
    void set_follow_target(Movable* target, float off_x, float off_y, float off_z);
    void update_view(void);

    /* Rotacija */
    void add_theta(double delta_theta);
    void add_phi(double delta_phi);

    /* Kretanje */
    void move_forward(void);
    void move_backward(void);
    void move_left(void);
    void move_right(void);
    void move_up(void);
    void move_down(void);
    void slow_down(void);
    void stop_moving(void);
    
    /* Geteri */
    Point3D& get_direction(void);
    Point3D& get_center(void);
    double get_move_speed(void) const;
    double get_rotation_speed(void) const;
    bool is_free_look(void) const;

    /* Seteri */
    void set_move_speed(double ms);
    void set_rotation_speed(double rs);
    void set_free_look(bool look);
    void set_center(double x, double y, double z);
    void set_direction(double x, double y, double z);
};

#endif
