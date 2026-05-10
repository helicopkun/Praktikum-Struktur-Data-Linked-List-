#include "Menu.h"

int main() {
    load_movies();
    load_users();

    User* u = login_menu();
    main_menu(u);

}
