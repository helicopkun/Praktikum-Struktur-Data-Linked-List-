#include "Menu.h"

int main() {
    load_movies();
    load_users();

    User* u = login_menu();
    system("cls");
    main_menu(u);

}
