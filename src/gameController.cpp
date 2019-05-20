#include <iostream>
#include <GL/glut.h>
#include <cmath>
#include "gameController.h"

/* Inicijalizacija trenutnog vremena */
double GameController::current_time = 0.0f;

/* Inicijalizacija proteklog vremena */
double GameController::delta_time = 0.0f;

GameController::GameController()
{
    /* Inicijalizacija spratova */
    init_floors();

    /* Postavljam lift u koordinatni pocetak */
    elevator.set_pos(0, 0, 0);

    /* 
     * Postavljam najmanju Y koordinatu lifta na 0, a najvecu na Y koordinatu
     * poslednjeg sprata, koju racunam tako sto saberem sve razmake izmedju
     * spratova (ima ih "floor_num" - 1)
     */
    elevator.set_min_y(0);
    elevator.set_max_y((floor_num -1) * floor_offset_y);

    /* 
     * Inicijalizujem pocetno i minimalno vreme stvaranja loptica.
     * Jedinice vremena su izrazene u sekundama.
     */
    start_spawn_time = 3.0f;
    min_spawn_time = 0.5f;

    /* 
     * Postavljam pocetno vreme nakon koga ce doci do prvog smanjenja 
     * vremena stvaranja
     */
    start_spawn_time_decrease = 15.0f;

    /* 
     * Tokom igre ce se smanjivanje vremena stvaranja desavati sve redje,
     * tako da koristim promenljivu "current_spawn_time_decrease" da pratim
     * trenutnu vrednost
     */
    current_spawn_time_decrease = start_spawn_time_decrease;

    /* Postavljam vreme sledeceg smanjenja na pocetno */
    next_spawn_time_decrease = start_spawn_time_decrease;

    /* 
     * Postavljam vrednost za koju ce se smanjiti vreme stvaranja, kada dodje
     * vreme za to
     */
    spawn_time_decrease = 0.4f;

    /* Inicijalizujem trenutno vreme stvaranja na pocetno */
    spawn_time = start_spawn_time;

    /* 
     * Postavljam kljuc za teksturu tako da se na pocetku igre
     * iscrtava ekran za pocetak
     */
    screen_texture_key = "start_game";

    /* Podesavam fleg da bi ekran trebalo da se iscrta */
    show_screen = true;
}

GameController::~GameController()
{
    /* 
     * Prilikom unistavanja objekta, unistavam sve spratove, jer su
     * dinamicki alocirani
     */
    for (unsigned int i = 0; i < floors_start.size(); i++)
        delete floors_start[i];

    for (unsigned int i = 0; i < floors_end.size(); i++)
        delete floors_end[i];
}

void GameController::floors_reset(void)
{
    /* Resetujem sve spratove, nakon cega se vracaju u pocetno stanje */
    for (unsigned int i = 0; i < floors_start.size(); i++)
        floors_start[i]->reset();

    for (unsigned int i = 0; i < floors_end.size(); i++)
        floors_end[i]->reset();
}

void GameController::init_floors(void)
{
    /* Postavljam broj spratova */
    floor_num = 6;

    /* Postavljam razmak izmedju spratova */
    floor_offset_y = 2.0f;

    /* X koordinata spratova */
    float floor_x = 3;

    /* Stvaram jednobojne spratove levo od lifta */
    for (int i = 0; i < floor_num; i++) 
    {
        /* 
         * Spratove cuvam u dinamicki alociranoj memoriji.
         * Prvi argument je duzina, drugi kapacitet loptica, a treci (opcioni) 
         * argument je boja sprata.
         */
        Floor* tmp = new Floor(4.0, 10);

        /* 
         * Postavljam poziciju sprata.
         * Razlog zasto stoji set_trans() umesto set_pos() je to sto su
         * spratovi nepokretni, te ne nasledjuju klasu Movable, vec samo klasu
         * Drawable (u kojoj se pozicija namesta sa set_trans())
         */
        tmp->set_trans(-floor_x, floor_offset_y * i, 0);

        /* Ubacujem kreirani sprat u vektor jednobojnih spratova */
        floors_start.push_back(tmp);
    }

    /* Postavljam redosled boja (od dole na gore) kojim se spratovi boje */
    Color colors[] = {RED, YELLOW, GREEN, CYAN, BLUE, PURPLE};

    for (int i = 0; i < floor_num; i++)
    {
        /* 
         * Ovaj put kreiram sprat u boji. Za odabir boja koristim ostatak pri
         * deljenju broja i sa brojem spratova (za slucaj da ima vise spratova
         * nego boja). Kapacitet je malo veci kako bi moglo da se primi vise
         * loptica odjednom.
         */
        Floor* tmp = new Floor(4.0, 20, colors[i % floor_num]);
        tmp->set_trans(floor_x, floor_offset_y * i, 0);

        /* 
         * Za obojene spratove podesavam opciju da unistavaju lopte koje stignu
         * do kraja sprata
         */
        tmp->get_ball_row().set_destroy_at_end(true);

        floors_end.push_back(tmp);
    }
}

void GameController::start_game(void)
{
    /* Ukoliko je postavljen fleg "game_over", funkcija zavrsava izvrsavanje */
    if (game_over)
        return;

    /* Postavljam trenutno vreme */
    time = Clock::now();

    /* Postavljam flegove na FALSE kako bi igra mogla da pocne */
    game_over = false;
    game_restart = false;
    show_screen = false;
}

void GameController::update(void)
{
    if (game_over)
    {
        /* 
         * Ukoliko je igra zavrsena, tok programa se prebacuje na funkciju
         * "on_game_over" 
         */
        on_game_over();
        return;
    }
    else if (show_screen)
    {
        /* Ukoliko bi trebalo da se prikaze ekran, izlazi se iz funkcije */
        return;
    }
    
    /* Azurira se vreme */
    update_time();

    /* Stvaraju se loptice */
    spawn_balls();

    /* Azurira se vreme stvaranja loptica */
    update_spawn_rate();

    /* Azuriraju se lift, spratovi i lava */
    elevator.update();
    update_floors();
    update_lava();
}

void GameController::on_game_over(void)
{
    /* Ova funkcija se poziva ukoliko je ukljuce fleg "game_over" */
    if (game_restart)
    {
        /* 
         * Ukoliko je ukljucen fleg "game_restart", tok programa se prebacuje
         * na funkciju "on_game_restart"
         */
        on_game_restart();
    }
    else if (!show_screen)
    {
        /* 
         * Ukoliko "game_restart" nije ukljucen i prikazivanje ekrana takodje
         * nije ukljuceno, ukljucuje se prikazivanje ekrana i postavlja
         * tekstura za iscrtavanje "restart_game"
         */
        show_screen = true;
        screen_texture_key = "restart_game";
    }
}

void GameController::on_game_restart(void)
{
    /* 
     * Ova funkcija se poziva ukoliko je ukljucen fleg "game_restart".
     * Prvo sto radim je gasenje flega za restart, kako bi se funkcija izvrsila
     * samo jednom
     */
    game_restart = false;

    /* Resetuje se lift na pocetna podesavanja */
    elevator.reset();

    /* Postavlja se pozicija lifta u koordinatni pocetak */
    elevator.set_pos(0, 0, 0);

    /* Resetuju se spratovi */
    floors_reset();

    /* Trenutni nivo lave se postavlja na minimalni nivo */
    lava.current_y = lava.min_y;

    /* Resetuje se trenutno vreme */
    current_time = 0.0f;

    /* Azurira se vreme koje se koristi za racunanje proteklog vremena */
    time = Clock::now();

    /* Resetuju se vremena za stvaranje loptica */
    spawn_time = start_spawn_time;
    next_spawn_time_decrease = start_spawn_time_decrease;
    next_spawn = 0.0f;

    /* Gasi se fleg za kraj igre */
    game_over = false;

    /* Gasi se iscrtavanje ekrana */
    show_screen = false;
}

void GameController::update_time(void) 
{
    /* Cuvam trenutno vreme */
    auto new_time = Clock::now();

    /* 
     * Racunam razliku izmedju trenutnog vremena i prethodno sacuvanog vremena.
     * Jedinica merenja je jedan sekund ( std::ratio<1> )
     */
    delta_time = 
        std::chrono::duration<double, std::ratio<1>> (new_time - time).count();

    /* Cuvam trenutno vreme kako bih ga iskoristio u sledecem pozivu funkcije */ 
    time = new_time;

    /* Uvecavam trenutno vreme za izracunatu razliku */
    current_time += delta_time;
}

void GameController::spawn_balls(void)
{
    /* Ukoliko nije vreme da se stvori loptica, prekida se dalje izvrsavanje */
    if (current_time < next_spawn)
        return;

    /* Pravim vektor indeksa spratova na kojima ima mesta za novu lopticu */
    std::vector<unsigned int> spawnable_floors;

    /* 
     * Prolazim kroz jednobojne spratove i njihov indeks dodajem u vektor
     * ukoliko sprat nije popunjen
     */
    for (unsigned int i = 0; i < floors_start.size(); i++)
        if (!floors_start[i]->is_full())
            spawnable_floors.push_back(i);

    /* 
     * Ukoliko su svi spratovi popunjeni, loptica nema gde da se stvori, pa
     * se prekida dalje izvrsavanje 
     */
    if (spawnable_floors.size() == 0)
        return;

    /* Bira se nasumican element iz niza indeksa nepopunjenih spratova */
    int random_floor = rand() % spawnable_floors.size(); 

    /* Cuvam indeks izabranog sprata u ovoj promenljivoj */
    int random_floor_index = spawnable_floors[random_floor];

    /* Na izabranom spratu stvaram lopticu */
    floors_start[random_floor_index]->spawn_ball();

    /* Povecavam vreme sledeceg stvaranja loptice za "spawn_time" */
    next_spawn += spawn_time;
}

void GameController::update_spawn_rate(void)
{
    /* Proveravam da li je vreme da se umanji vreme stvaranja loptica */
    if (current_time > next_spawn_time_decrease)
    {
        /* Umanjujem vreme stvaranja */
        spawn_time -= spawn_time_decrease;

        /* Ukoliko ode ispod minimuma, vracam ga na minimum */
        if (spawn_time < min_spawn_time)
            spawn_time = min_spawn_time;

        /* Dupliram vreme potrebno za aktiviranje novog umanjenja */
        current_spawn_time_decrease *= 2;

        /* 
         * Dodajem duplirano vreme na trenutno vreme, i dobijam vreme sledeceg
         * umanjenja 
         */
        next_spawn_time_decrease = current_time + current_spawn_time_decrease;
    }
}

void GameController::update_floors(void)
{
    /* Azuriram sve spratove */
    for (unsigned int i = 0; i < floors_start.size(); i++)
        floors_start[i]->update();

    for (unsigned int i = 0; i < floors_end.size(); i++)
        floors_end[i]->update();
}

void GameController::update_lava(void)
{
    /* 
     * Postavljam maksimalni nivo lave na "lava.game_over" i oduzimam mu
     * razliku broja spratova i broja popunjenih spratova.
     * Ukoliko su svi spratovi popunjeni, ova razlika je jednaka 0, te
     * je maksimalni nivo lave u nivou "lava.game_over", i ako ga lava dostigne
     * igra se zavrsava
    */
    lava.max_y = lava.game_over - (floors_start.size() - (float) count_full_floors());

    /* 
     * Posto se maksimalni nivo menja sa promenom popunjenosti spratova, moze
     * da se dogodi da se nivo lave nalazi iznad maksimalnog nivoa. Kako bih
     * sprecio naglo spustanje nivoa lave, u ovom slucaju, postavljam
     * maksimalni nivo lave na trenutni nivo.
     */
    if (lava.current_y > lava.max_y)
        lava.max_y = lava.current_y;

    /* Pomeram nivo lave u odnosu na trenutnu brzinu i proteklo vreme */
    lava.current_y += lava.speed * delta_time;

    /* Ispitujem da li je nivo lave preko maksimalnog nivoa */
    if (lava.current_y > lava.max_y)
    {
        /* 
         * Ukoliko jeste, ispitujem da li je taj maksimalni nivo jednak nivou
         * za kraj igre
         */
        if (lava.max_y == lava.game_over)
        {
            /* Ako jeste, postavljam odgovarajuci fleg */
            game_over = true;
            return;
        }
        else
        {
            /* 
             * Ako nije, zelim da lava promeni smer kretanja i da pocne da se
             * krece nanize. to radim mnozenjem brzine lave sa -1.
             */
            lava.speed *= -1;

            /* Vracam nivo lave na maksimalni, u slucaju da je presao preko */
            lava.current_y = lava.max_y;
        }
    }
    /* Ako nivo lave nije iznad maksimuma, ispitujem da li je ispod minimuma */
    else if (lava.current_y < lava.min_y)
    {
        /* Ako jeste, radim isto sto i gore, bez provere za kraj igre */
        lava.speed *= -1;
        lava.current_y = lava.min_y;
    }

    /* 
     * Povecavam animaciju promene boje u odnosu na brzinu kretanja lave
     * i proteklog vremena. Koristi se apsolutna vrednost brzine, da ne bi
     * doslo do smanjivanja "lava.animation" promenljive 
     */
    lava.animation += std::abs(lava.speed) * delta_time;

    /* 
     * Postavljam crvenu i zelenu komponentu boje po datoj formuli.
     * Ne postoji neki specijalan razlog zasto se koriste ovi brojevi,
     * testiranjem sam dosao do njih.
     * Dobijene vrednosti su u intervalu [0, range * 2]
     */
    float range = 0.2;
    lava.color_red = range * (1 + sin(lava.animation * 5));
    lava.color_green = range * (1 + sin(lava.animation * 3));
}

int GameController::count_full_floors(void)
{
    /* Prolazi kroz jednobojne spratove i inkrementira brojac ukoliko su popunjeni */
    int count = 0;
    for (unsigned int i = 0; i < floors_start.size(); i++)
        if (floors_start[i]->is_full())
            count++;
    return count;
}

bool GameController::on_keyboard(unsigned char key)
{
    /* 
     * Razlog zasto funkcija vraca BOOL je to sto zelim da se, u slucaju da
     * pritisnuto dugme nije neko od predvidjenih kontrola, obrada pritisnutog
     * dugmeta nastavlja u funkciji koja je pozvala ovu funkciju
     */
    
    /* Vrednost ENTER-a je 13 */
    const unsigned char ENTER = 13;
    switch(key)
    {
        case 'W':
        case 'w':
            /* Pomera lift na gore */
            elevator.set_movement(MOVING_UP);
            return true;
        case 'S':
        case 's':
            /* Pomera lift na dole */
            elevator.set_movement(MOVING_DOWN);
            return true;
        case 'A':
        case 'a':
            /* Zapocinje utovar loptica */
            elevator.start_transfer(floors_start);
            return true;
        case 'D':
        case 'd':
            /* Zapocinje istovar loptica */
            elevator.start_transfer(floors_end);
            return true;
        case ' ':
            /* Prekida se trenutni transfer (utovar/istovar) loptica */
            elevator.cancel_transfer();
            return true;
        case 'r':
        case 'R':
            /* 
             * Ukoliko je uljucen fleg "game_over", a "game_restart" nije,
             * ukljucuje fleg "game_restart"
             */
            if (game_over && !game_restart)
                game_restart = true;
            return true;
        case ENTER:
            /* Zapocinje igru */
            start_game();
            return true;
        
        default:
            /* 
             * Ako pritisnuto dugme nije medju navedenim slucajevima,
             * dalja obrada se nastavlja u funkciji koja je pozvala ovu,
             * a to se signalizira vracanjem FALSE vrednosti
             */
            return false;
    }
}

bool GameController::on_keyboard_up(unsigned char key)
{
    /* 
     * Isti slucaj kao u funkciji "on_keyboard", s'tim sto se ovde obradjuje
     * dogadjaj kada se dugme pusti 
     */
    switch (key)
    {
        case 'w':
        case 's':
            /* Zaustavlja lift */
            elevator.set_movement(STOP_MOVING);
            return true;
        default:
            return false;
    }
}

void GameController::draw_scene(void) const
{
    /* 
     * Ukoliko je fleg ukljucen, iscrtava se ekran i zavrsava iscrtavanje
     * scene, jer je ekran pozicioniran ispred kamere tako da se scena svakako
     * ne vidi
     */
    if (show_screen)
    {
        display_screen();
        return;
    }

    /* Iscrtava se pozadina */
    draw_background();

    /* Iscrtavaju se svi spratovi */
    for (unsigned int i = 0; i < floors_start.size(); i++)
        floors_start[i]->draw();

    for (unsigned int i = 0; i < floors_end.size(); i++)
        floors_end[i]->draw();

    /* Iscrtava se lift */
    elevator.draw();
}

void GameController::display_screen(void) const
{
    /* 
     * Postavlja se mod iscrtavanja tekstura na GL_REPLACE, sto omogucava da
     * se tekstura iscrta nezavisno od materijala i osvetljenja 
     */
    TextureManager::set_mode(GL_REPLACE);

    /* 
     * Posto nam kamera stalno prati lift, a zelim da iscrtam ekran ispred
     * kamere, kao referentnu tacku koristim poziciju lifta
     */
    Point3D pos = elevator.get_pos();

    /* 
     * Osiguravam se da sve transformacije koje izvrsim u nastavku koda, nemaju
     * efekta nakon izlaska iz funkcije
     */
    glPushMatrix();

        /* Transliram koordinatni pocetak u polozaj lifta */
        glTranslatef(pos.get_x(), pos.get_y(), pos.get_z());

        /* Rotiram oko X ose, kako bih okrenuo ekran ka kameri */
        glRotatef(-40, 1, 0, 0);
        
        /* Postavljam velicinu ekrana i poziciju na Z osi */
        float size = 2.0f;
        float pos_z = 2.0f;

        /* Ukljucujem teksturu za ekran */
        TextureManager::enable_texture(screen_texture_key);
        
        /* 
         * Zapocinjem definisanje cetvorougaonika(u ovom slucaju pravougaonika) 
         * i navodim koordinate teksture kao i koordinate temena pravougaonika
         * kako bih mapirao teksturu
         */
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex3f(-size * 2, -size, pos_z);

            glTexCoord2f(1, 0);
            glVertex3f(size * 2, -size, pos_z);

            glTexCoord2f(1, 1);
            glVertex3f(size * 2, size, pos_z);

            glTexCoord2f(0, 1);
            glVertex3f(-size * 2, size, pos_z);
        glEnd();

        /* Gasim koriscenje teksture */
        TextureManager::disable_texture();

        /* 
         * Ukoliko se iscrtava ekran za kraj igre, potrebno je da se iscrta
         * broj osvojenih poena.
         */
        if (screen_texture_key == "restart_game")
        {
            /* 
             * Da bi se poeni videli, moraju biti iscrtani ispred ekrana, pa
             * transliram jos jednom u pozitivnom smeru Z ose.
             */
            glTranslatef(0.0f, 0.0f, pos_z + 0.1f);

            /* 
             * Iscrtavaju se poeni, prva dva argumenta zadaju pocetnu tacku, 
             * a drugi argument je velicina slike.
             */
            display_points(3.0f, -1.5f, 0.5f);
        }

    /* Ponistavam prethodno izvrsene transformacije */
    glPopMatrix();

    /* Vracam mod iscrtavanja na GL_ADD */
    TextureManager::set_mode(GL_ADD);
}

void GameController::display_points(float x, float y, float size) const
{
    /* 
     * Osvojeni poeni se cuvaju u liftu, po jedan poen za svaku istovarenu
     * kuglu. Ucitavam poene u promenljivu "points"
     */
    unsigned int points = elevator.get_points();

    /* Podesavam razmak izmedju dve cifre */
    float offset = 0.1f;

    /* Izdvajam cifru po cifru i iscrtavam je */
    while (points != 0)
    {
        /* Poslednja cifra */
        int digit = points % 10;

        /* Iscrtavanje poslednje cifre, prva dva argumenta su pozicije,
         * treci argument je velicina slike, a cetvrti cifra, kao sto i pise */
        display_digit(x, y, size, digit);

        /* 
         * Nakon iscrtane cifre, pomeram X koordinatu u levo za velicinu slike
         * i razmak, kako bih podesio ispravnu koordinatu za sledecu cifru
         */
        x -= size + offset;

        /* 
         * Isecam poslednju cifru iz broja poena i nastavljam dalje dok se ne
         * obrade sve cifre. Polako sticem utisak da previse komentarisem...
         */
        points /= 10;
    }
}

void GameController::display_digit(float x, float y, float size, int digit) const
{
    /* Aktivira se odgovarajuca tekstura */
    TextureManager::enable_texture("digit_" + std::to_string(digit));

    /* Mapira se tekstura, ovog puta krecem od donjeg desnog coska */
    glBegin(GL_QUADS);
        glTexCoord2f(1, 0);
        glVertex2f(x, y);

        glTexCoord2f(0, 0);
        glVertex2f(x - size, y);

        glTexCoord2f(0, 1);
        glVertex2f(x - size, y +size);

        glTexCoord2f(1, 1);
        glVertex2f(x, y + size);
    glEnd();

    /* Gasim teksturu */
    TextureManager::disable_texture();
}

void GameController::draw_background(void) const
{
    /* 
     * Iscrtavaju se zidovi i lava. Zajedno cine pozadinu.
     * Zato se funkcija tako i zove, kao i odgovarajuci pozivi ispod 
     */
    draw_walls();
    draw_lava();    
}

void GameController::draw_walls(void) const
{
    /* Velicina zida */
    float size = 25.0f;

    /* Broj ponavljanja teksture */
    float repeat = 10.0f;

    /* Udaljenost zida od lifta */
    float distance = 12.0f;

    /* Ugao pod kojim se crtaju bocni zidovi */
    float angle = 40.0f;
    
    /* Ukljucujem dati mod za teksture */
    TextureManager::set_mode(GL_REPLACE);

    /* Crtam glavni zid */
    glPushMatrix();

        /* Transliram po Z osi */
        glTranslatef(0, 0, -distance);

        /* 
         * Koristim funkciju "draw_quad" klase Drawable
         * kojoj prosledjujem kljuc za teksturu, velicinu QUAD-a,
         * kao i broj ponavljanja teksture.
         */
        Drawable::draw_quad("wall", size, repeat);

    glPopMatrix();

    /* 
     * Druga dva zida se razlikuju od prvog po tome sto su dodatno translirani 
     * po X osi i rotirani oko Y ose, ostatak je isti
     */
    glPushMatrix();
        glTranslatef(-distance, 0, -distance);
        glRotatef(angle, 0, 1, 0);
        Drawable::draw_quad("wall", size, repeat);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(distance, 0, -distance);
        glRotatef(-angle, 0, 1, 0);
        Drawable::draw_quad("wall", size, repeat);
    glPopMatrix();

    /* Vracam mod za crtanje tekstura na vrednost koju inace koristim */
    TextureManager::set_mode(GL_ADD);
}

void GameController::draw_lava(void) const
{
    /* Podesavanje materijala za lavu */
    GLfloat ambient[] = {lava.color_red, lava.color_green, 0.4f, 1.0f};
    GLfloat diffuse[] = {lava.color_red, lava.color_green, 0.2f, 1.0f};
    GLfloat specular[] = {lava.color_red, lava.color_green, 0.0f, 1.0f};
    GLfloat shininess = 150.0f;

    /* Ukljucivanje podesenog materijala */
    Material::enable(ambient, diffuse, specular, shininess);

    glPushMatrix();
        /* Transliram na trenutni nivo lave */
        glTranslatef(0, lava.current_y, 0);

        /* Rotiram oko X ose, kako bi lava bila paralelna sa ravni XZ */
        glRotatef(90, 1, 0, 0);

        /* Crtam lavu */
        Drawable::draw_quad("lava", 30.0f, 40.0f);
    glPopMatrix();
}

/* Geteri */
double GameController::get_current_time(void) { return current_time; }

double GameController::get_delta_time(void) { return delta_time; }

Elevator* GameController::get_elevator_pointer(void) { return &elevator; }

Elevator& GameController::get_elevator_reference(void) { return elevator; }

Movable* GameController::get_elevator_movable(void)
{
    return &(elevator.get_movable());
}
