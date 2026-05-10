#include "Menu.h"


// tes compile
int main() {
    // cout << "tes123\n";
    load_movies();
    load_users();

    // User* list[100] = {};
    // int count = Users.get_all(list, 100);
    // for (int i = 0; i < count; i++)
    //     cout << list[i]->nama << endl;

    // cout << "\n\n";
    // User* u = new User("a", "b");
    // User* admin = new User("c","d");
    // admin->isAdmin = true;

    User* u = (User*) Users.get_exact("helicopkun");

    // menu_genre(u);           working, rating is also working (i hope it always does)
    // menu_search(u, "exact"); working
    // menu_search(u, "match"); working
    // menu_all_movies(u, "Film"); working
    // menu_all_movies(u, "Series"); //BROKEN
    // menu_all_movies(u, "Film / Series"); working
    menu_tambah_movie();

    // cout << "\n\nCompiled";
}