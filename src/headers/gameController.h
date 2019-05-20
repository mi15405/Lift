#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <ctime>
#include <chrono>
#include <vector>
#include <string>
#include "floor.h"
#include "elevator.h"

typedef struct 
{
    float min_y = -8.0f;      /* Minimalni nivo lave */
    float max_y = -1.0f;      /* Maksimalni nivo lave */
    float game_over = max_y;  /* Nivo lave koji aktivira kraj igre */
    float current_y = min_y;  /* Trenutni nivo lave */
    float speed = 0.5f;       /* Brzina podizanja/spustanja nivoa lave */
    float animation = 0.0f;   /* Animacija menjana boje lave */
    float color_red = 0.0f;   /* Crvena komponenta boje */
    float color_green = 0.0f; /* Zelena komponenta boje */
} Lava;

class GameController
{
    using Clock = std::chrono::system_clock;

private:
    /* Trenutno vreme */
    static double current_time;          

    /* Vreme proteklo izmedju dva poziva funkcije azuriranja */
    static double delta_time; 

    /* Broj spratova */
    int floor_num;                       

    /* Razmak izmedju spratova */
    float floor_offset_y;                

    /* Spratovi koji stvaraju loptice */
    std::vector<Floor*> floors_start;    

    /* Spratovi do kojih loptice dolaze */
    std::vector<Floor*> floors_end;      

    /* Lift */
    Elevator elevator;

    /* Lava */
    Lava lava; 

    /* Trenutak u vremenu, koristi se za racunanje proteklog vremena */
    std::chrono::time_point<Clock> time; 


    /* 
     * Pocetno vreme stvaranja loptica, pod tim mislim na vreme proteklo
     * izmedju stvaranja dve loptice.
     */
    double start_spawn_time;

    /* Trenutno vreme stvaranja loptica */
    double spawn_time;

    /* Vreme kada bi trebalo da se stvori sledeca loptica */
    double next_spawn;

    /* Minimalno vreme stvaranja loptica */
    double min_spawn_time;

    /* Pocetno vreme nakon koga se smanjuje vreme stvaranja loptica */
    double start_spawn_time_decrease;

    /* Trenutno vreme izmedju dva smanjivanja vremena stvaranja loptica */
    double current_spawn_time_decrease;

    /* Vrednost kojom se umanjuje vreme stvaranja loptica */
    double spawn_time_decrease;

    /* Vreme kada se "spawn_time" umanjuje za "spawn_time_decrease" */
    double next_spawn_time_decrease;

    /* Fleg koji oznacava da je igra zavrsena */
    bool game_over;

    /* Fleg koji oznacava da treba restartovati igru */
    bool game_restart;

    /* 
     * Fleg koji oznacava da li bi trebalo da se iscrta ekran za pocetak (kraj)
     * igre
     */
    bool show_screen;

    /* 
     * Kljuc za teksturu koja se koristi prilikom crtanja ekrana za pocetak
     * (kraj) igre
     */
    std::string screen_texture_key;

public:
    GameController();
    ~GameController();

    /* Azuriranje stanja igre */
    void update(void);

    /* Crtanje scene */
    void draw_scene(void) const;

    /* Obrada pritiskanja dugmica */
    bool on_keyboard(unsigned char key);
    bool on_keyboard_up(unsigned char key);

    /* Geteri */
    static double get_current_time(void);
    static double get_delta_time(void);
    Elevator* get_elevator_pointer(void);
    Elevator& get_elevator_reference(void);
    Movable* get_elevator_movable(void);

private:
    /* Pocetak igre */
    void start_game(void);

    /* Azuriranje */ 
    void update_time(void);
    void update_floors(void);
    void update_lava(void);
    void update_spawn_rate(void);

    /* Stvaranje loptica */
    void spawn_balls(void);

    /* Spratovi */
    void init_floors(void);
    void floors_reset(void);
    int count_full_floors(void);

    /* Crtanje */
    void draw_background(void) const;
    void draw_walls(void) const;
    void draw_lava(void) const;

    /* Prikaz ekrana sa tekstom */
    void display_screen(void) const;
    void display_points(float x, float y, float size) const;
    void display_digit(float x, float y, float size, int digit) const;

    /* Kraj igre */
    void on_game_over(void);
    void on_game_restart(void);
};

#endif
