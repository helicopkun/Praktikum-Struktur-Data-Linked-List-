#ifndef MENU_H
#define MENU_H

#include "Display.h"

bool keluarAdmin = false, 
     keluar = false;

//------------------------------------------ Menus ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
void main_menu(User* u) {
    cout << R"(==== Wordboxd ====
1. Genre
2. Search Film / Series
3. Search Film / Series (Approx)
4. Semua Film
5. Semua Series
6. Semua Film / Series)";
    if (u->isAdmin) cout << "\n7. Tambah Film / Series";
    cout << "\n\n";
    int max = 6;
    if (u->isAdmin) max = 7;
    int pilihan = input_idx("Pilihan: ", 1, max);

    system("cls");
    if      (pilihan == 1) menu_genre(u);
    else if (pilihan == 2) menu_search(u, "exact");
    else if (pilihan == 3) menu_search(u, "match");
    else if (pilihan == 4) menu_all_movies(u, "Film");
    else if (pilihan == 5) menu_all_movies(u, "Series");
    else if (pilihan == 6) menu_all_movies(u, "Film / Series");
    else if (pilihan == 7 && u->isAdmin) menu_tambah_movie();

    system("cls");
    main_menu(u);
}

User* register_menu(string name) {
    system("cls");
    cout << "==== Register Page ====\n";
    cout << "Username: " << name << endl;
    string pw = input_name("Password: ");

    User* newU = new User(name, pw);
    add_user_to_table(newU);
    append_user(newU);
    cout << "User telah berhasil terdaftar!\n";
    press_enter();

    return newU;
}

User* login_menu() {
    system("cls");
    cout << "==== Login Page ====\n";
    string name = input_name("Username: ");
    if (name == "akuadmin727" || name == "admin") {
        User* admin = new User(">_<", "^ v ^");
        admin->isAdmin = true;
        return admin;
    }

    if (!Users.is_exist(name)) {
        cout << "\n\n";
        string pilihan = input_name("Username doesnt exist, Register?\n\n[Y]es, [N]o\nPilihan: ");
        pilihan = get_key(pilihan);
        if      (pilihan == "y" || pilihan == "yes") return register_menu(name);
        else if (pilihan == "n" || pilihan == "no"){}
        else    {cout << "Invalid input"; press_enter();}
        return login_menu();
    }

    string pw = input_name("Password: ");
    User* u = (User*)Users.get_exact(name);

    if (u->password != pw) {
        cout << "Bozo forgot their pw lmao\n";
        press_enter();
        return login_menu();
    }

    return u;
}



#endif