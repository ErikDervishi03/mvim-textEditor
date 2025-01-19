#include "../include/cursor.hpp"
#include <iostream>

Cursor::Cursor() :
  x(0), y(0)
{

}

int Cursor::getX() const
{
  return x;
}

int Cursor::getY() const
{
  return y;
}

void Cursor::move_right()
{
  this->x += 1;
}

void Cursor::move_left()
{
  this->x -= 1;

}

void Cursor::move_up()
{
  this->y -= 1;

}

void Cursor::move_down()
{
  this->y += 1;
}

void Cursor::set(int x, int y)
{
  this->x = x;
  this->y = y;
}

void Cursor::setX(int x)
{
  this->x = x;

}

void Cursor::setY(int y)
{
  this->y = y;
}

void Cursor::update_cursor()
{
    // Move the cursor within the pointed window
    wmove(pointed_window, y, x);  // Move cursor to (y, x) in pointed_window
}

void Cursor::restore(int span) {
    // Ottieni le dimensioni della finestra corrente
    int max_row, max_col;
    getmaxyx(pointed_window, max_row, max_col);

    // Calcola la posizione desiderata del cursore
    int target_y = y;
    int target_x = x + span + 1;

    // Verifica che la posizione sia nei limiti della finestra
    if (target_y < 0 || target_y >= max_row || target_x < 0 || target_x >= max_col) {
        // Termina temporaneamente ncurses per stampare l'errore sul terminale
        endwin();

        // Stampa il messaggio di errore sul terminale
        std::cerr << "Errore: il cursore è fuori dai confini! (" 
                  << "target_y=" << target_y << ", target_x=" << target_x 
                  << ", max_row=" << max_row << ", max_col=" << max_col << ")" 
                  << std::endl;

        // Ritorna alla modalità ncurses
        refresh();

        return; // Non spostare il cursore se fuori dai limiti
    }

    // Ripristina la posizione del cursore
    wmove(pointed_window, target_y, target_x);
}

bool Cursor::is_out_of_bounds(int span) const {
    // Ottieni le dimensioni della finestra corrente
    int max_row, max_col;
    getmaxyx(pointed_window, max_row, max_col);

    // Calcola la posizione effettiva del cursore nella finestra
    int target_y = y;
    int target_x = x + span + 1;

    // Verifica se il cursore è fuori dai limiti
    return target_y < 0 || target_y >= max_row || target_x < 0 || target_x >= max_col;
}




void Cursor::pointToWindow(WINDOW* window){
    pointed_window = window;
}