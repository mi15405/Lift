#ifndef H_MOVABLE
#define H_MOVABLE

#include <string>
#include <ctime>
#include <chrono>
#include "point3D.h"

class Movable
{
    using Clock = std::chrono::system_clock;

private:
    /* Pozicija objekta */
    Point3D pos;              

    /* Brzina objekta */
    Point3D vel;              

    /* Proteklo vreme izmedju dva azuriranja */
    double delta_time;  

    /* Trenutak u vremenu */
    std::chrono::time_point<Clock> time; 

    /* Ispis objekta, za debagovanje */
    std::string to_string;    

public:
    Movable(
        double pos_x = 0, double pos_y = 0, double pos_z = 0,
        double vel_x = 0, double vel_y = 0, double vel_z = 0);

    virtual void update();
    void move_to(const Point3D& p, double speed);

    void reset_pos(void);
    void reset_vel(void);

    void bounce_in_box(
        double x_min, double x_max,
        double y_min, double y_max,
        double z_min, double z_max);

    /* Ispis za debagovanje */
    operator const char*();

    /* Geteri */
    double get_delta_time(void);
    Point3D get_pos(void) const;
    Point3D get_vel(void) const;
    Point3D& get_pos_reference(void);
    Point3D& get_vel_reference(void);
    Movable& get_movable(void);

    /* Seteri */
    void set_pos_x(double px);
    void set_pos_y(double py);
    void set_pos_z(double pz);
    void set_pos(double px, double py, double pz);
    void set_vel(double px, double py, double pz);
    void set_pos(const Point3D& p);
    void set_vel(const Point3D& p);
};

#endif
