# Lift

# Screenshot
![nema slike](https://github.com/mi15405/Lift/blob/master/screenshots/08.png)

## Opis
Kugle se stvaraju iz crnih portala sa leve strane i zadatak je prebaciti ih do sarenih portala sa desne strane.
Igrac na raspolaganju ima lift koji u pocetku moze da ponese samo 3 kugle.
Nakon odrejednog broja prebacenih kugli lift se sam unapredjuje (povecava se
kapacitet ili brzina kretanja).

Maksimalni broj kugli na jednom spratu je 10. Kada se taj broj dostigne,
podize se nivo do kog lava moze da dodje.
Ako u nekom trenutku svih 6 spratova bude popunjeno, maksimalni nivo lave ulazi
u kriticnu zonu. 

Ukoliko igrac ne oslobodi prostor na nekom od spratova, dok lava ne stigne
do kriticnog nivoa, igra je zavrsena.

Vremenom se kugle sve brze stvaraju te redosled prebacivanja postaje bitan.

## :video_game: Kontrole

| Akcija | Dugme |
| ---| --- |
|  kretanje na gore  | w |
|  kretanje na dole | s |
|  spustanje levog mosta | a |
|  spustanje desnog mosta | d |
|  prekid prebacivanja kugli | space |
|  izlaz iz igre  | Esc |

## Instalacija

1. sudo apt-get update 
2. apt-get install freeglut3
3. git clone https://github.com/mi15405/Lift
4. cd Lift/src
5. make
6. ./lift


