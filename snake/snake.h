#include <stdbool.h>
int snake_get_i_at(int index);
int snake_get_j_at(int index);

int snake_get_curr_i();
void snake_set_curr_i(int i);

int snake_get_curr_i();
void snake_set_curr_i(int i);

int snake_get_curr_j();
void snake_set_curr_j(int j);

int snake_get_size();
void snake_set_size(int size);

void snake_grow();
void snake_free_i_body();
void snake_free_j_body();

int snake_get_previous_i();
int snake_get_previous_j();
void snake_set_previous_i(int value);
void snake_set_previous_j(int value);

void snake_move();
void snake_print();

void snake_init();

int snake_get_size();

void snake_reinitialize();

bool snake_did_snake_hit_self();