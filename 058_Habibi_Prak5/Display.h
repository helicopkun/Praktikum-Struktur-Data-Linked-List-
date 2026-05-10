#ifndef DISPLAY_H
#define DISPLAY_H

#include "Files_List.h"

// ── Inputs ───────────────────────────
void press_enter() {
    string dummy;
    cout << "\nPress Enter to go back...";
    getline(cin, dummy);
    system("cls");
}

int input_idx(const string& prompt, int min, int max) {
    int val;
    while (true) {
        cout << prompt;
        cin  >> val;
        if (cin.fail() || val < min || val > max) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input, enter a number between " << min << " and " << max << "\n";
        } else {
            cin.ignore(1000, '\n');
            return val;
        }
    }
}

float input_rate(const string& prompt) {
    float val;
    while (true) {
        cout << prompt;
        cin  >> val;
        if (cin.fail() || val < 1.0f || val > 10.0f) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input, enter a rating between 1 and 10\n";
        } else {
            cin.ignore(1000, '\n');
            return val;
        }
    }
}

string input_name(const string& prompt) {
    string val;
    while (true) {
        cout << prompt;
        getline(cin, val);
        if (val.empty()) {
            cout << "Invalid input, name cannot be empty\n";
        } else {
            return val;
        }
    }
}

string input_optional(const string& prompt) {
    cout << prompt;
    string val;
    getline(cin, val);
    return val;
}

// ── General ───────────────────────────
void tampil_movie_detail(Movies* m) {
    Genre* list_genre[10] = {};
    int genre_count = m->genres.get_all(list_genre, 10);

    Movies* list_related[20] = {};
    int related_count = m->film_terkait.get_all(list_related, 20);

    cout << "---- " << m->nama << " ----" << endl
         << "Genre: ";
    for (int i = 0; i < genre_count; i++) {
        if (!list_genre[i]) break;
        cout << list_genre[i]->nama << ", ";
    }
    cout << '\n';
    if (m->is_film()) {
        cout << "Jumlah ep: -\nJumlah season: -\n";
    } else {
        cout << "Jumlah ep: "<< m->jumlah_episode << '\n'
             << "Jumlah season: " << m->jumlah_season << '\n';
    }
    cout << "Film yang terkait:\n";
    for (int i = 0; i < related_count; i++) {
        if (!list_related[i]) break;
        cout << "\t - " << list_related[i]->nama << "\n";
    }
    cout << "Studio: " << m->studio << '\n';
    cout << "Rating: " << m->total_rating << '\n';
    
}

void tampil_rate(User* u, Movies* m) {
    cout << "\n==== Rate "<< m->nama << " ====\n";
    float rating = input_rate("Beri rating: ");

    bool new_viewer = !u->rated.is_exist(m->nama);
    float old_rate = 0.0f;
    if (!new_viewer) {
        RatingEntry* cur = (RatingEntry*) u->rated.get_exact(m->nama);
        old_rate = cur->rate;
    }
    RatingEntry* all_rating [1000] = {};
    int rating_count = m->rated_by.get_all(all_rating, 1000);
    float old_total_rate = m->total_rating * rating_count;

    link_userRate_to_movie(u, m, rating);

    if (new_viewer) m->total_rating = (old_total_rate + rating)/(rating_count + 1);
    else            m->total_rating = (old_total_rate - old_rate + rating)/(rating_count);

    save_movies();
    save_users();

    cout << "Rating Baru: " << m->total_rating << '\n';
    press_enter();
}

void tampil_list_movie(User* u, string headline, Movies* list[], int idx, int count) {
    system("cls");
    cout << "==== "<< headline << " ====\n\n";
    tampil_movie_detail(list[idx]);
    
    string prompt = "[N]ext, [P]revious, [C]lose, [R]ate\nPilihan: ";
    if (u->isAdmin) prompt = "[N]ext, [P]revious, [C]lose\nPilihan: ";
    string option = input_name(prompt);
    option = get_key(option);

    if (option == "n" || option == "next") idx++;
    else if (option == "p" || option == "prev" || option == "previous") idx--;
    else if (option == "c" || option == "close") return;
    else if ((option == "r" || option == "rate") && !u->isAdmin) tampil_rate(u, list[idx]);
    
    if (idx < 0) idx = count - 1; //jump to back
    else if (idx >= count) idx = 0; //jump to front
    tampil_list_movie(u, headline, list, idx, count);
}

// ── In-Menu ───────────────────────────
void menu_tambah_movie() {
    cout << "==== Tambah Film / Series ====\n";
    string nama     = input_name("Nama: ");
    string genres   = input_name("Genre: ");
    int ep          = input_idx("Jumlah Episode: ", 0, 10000);
    int season      = input_idx("Jumlah Season: ", 0, 100);
    string related  = input_optional("Film terkait: ");
    string studio   = input_name("Studio: ");
    float rating = 0.0f;

    // 
    Movies* exist = search_movie_BST(BST, get_key(nama));
    if (exist) {
        cout << exist->nama << " sudah terdaftar di database...\n";
        press_enter();
        return;
    }
        
    Movies* newM = new Movies(nama, studio, ep, season, rating);

    string genre = "";
    bool valid = false;
    for (int i = 0; i < genres.size(); i++) {
        if (genres[i] == ',') {
            i++; //skip space before another genre
            Genre * g = (Genre*) (Genres.get_exact(genre));

            if (g) { link_genre_to_movie(g, newM);  valid = true; } 
            else cout << "Genre " << genre << " Tidak terdaftar\n";

            genre = "";
        } else {
            genre += genres[i];
        }
    }
    if (!genre.empty()) { //last token in line
        Genre* g = (Genre*) Genres.get_exact(genre);

        if (g) { link_genre_to_movie(g, newM);  valid = true; } 
        else cout << "Genre " << genre << " Tidak terdaftar\n";
    }

    if (!valid) {
        cout << "Tidak ada Genre yang terdaftar, Movie gagal di daftarkan";
        delete newM;
        return;
    }

    add_movie_to_BST(BST, newM);
    string relate = "";
    for (int i = 0; i < related.size(); i++) {
        if (related[i] == ',') {
            i++; //skip space before another relate
            exist = search_movie_BST(BST, get_key(relate));

            if (exist) link_movie_to_universe(newM, exist);
            else cout << "Film " << relate << " tidak dapat ditemukan\n";

            relate = "";
        } else {
            relate += related[i];
        }
    }
    if (!relate.empty()) { // last token in line
        exist = search_movie_BST(BST, get_key(relate));

        if (exist) link_movie_to_universe(newM, exist);
        else cout << "Film " << relate << " tidak dapat ditemukan\n";
    }
    cout << newM->nama << " kini terdaftar ke database!\n";
    save_movies();
}

void menu_genre (User* u) {
    cout << "==== Pilih Genre ====\n";
    Genre* all_genre[100] = {};
    int genre_count = Genres.get_all(all_genre, 100);
    
    for (int i = 0; i < genre_count; i++)
        cout << i + 1 << ". " << all_genre[i]->nama << endl;
    
    int pilihan = input_idx("\nPilih: ", 1, genre_count); // idx = pilihan - 1
    Genre* g = all_genre[pilihan - 1];

    Movies* movie_list [100] = {};
    int movie_count = g->movies.get_all(movie_list, 100);
    int idx = 0;
    string headline = "Genre " + g->nama;
    tampil_list_movie(u, headline, movie_list, idx, movie_count);
}

void menu_all_movies (User* u, string method) { //method = ["Film", "Series", "Film / Series"]
    Movies* all_movies[1000];
    int count = 0;
    get_all_movie_BST(BST, all_movies, count, method);
    cout << "==== Semua "<< method << " ====\n";
    for (int i = 0; i < count; i++) {
        string type = "Series";
        if (all_movies[i]->is_film()) type = "Film";
        cout << i + 1 << ". " << all_movies[i]->nama << "  > " << type << " | " << all_movies[i]->total_rating << "\n";
    }
    cout << "\n\n";
    string prompt = "[C]lose, [R]ate\nPilihan: ";
    if (u->isAdmin) prompt = "[C]lose\nPilihan: ";

    string option = input_name(prompt);
    option = get_key(option);
    if (option == "c" || option == "close") return;
    else if ((option == "r" || option == "rate") && !u->isAdmin) {
        int pilihan = input_idx("\nPilih film: ", 1, count);
        tampil_rate(u, all_movies[pilihan - 1]);
    }
}

void menu_search (User* u, string option) { // option = ["match", "exact"]
    cout << "==== Search Film / Series ====\n";
    string keyword = input_name("Nama movie: ");
    cout << "\n\n";
    if (option == "match") {
        int count = 0;
        Movies* results [10] = {};
        search_match_movie_BST(BST, get_key(keyword), results, count);

        if (count == 0) {
            cout << "No matches found.\n";
            press_enter();
            system("cls");
            return menu_search(u, option);
        }
        string headline = "Matching result for " + keyword;
        int idx = 0;
        tampil_list_movie(u, headline, results, idx, count);

    }
    else if (option == "exact") {
        Movies* result = search_movie_BST(BST, get_key(keyword));
        if(!result){
            cout << "No Movie with the name - " << keyword << '\n';
            press_enter();
            return;
        }
        tampil_movie_detail(result);

        string prompt = "[C]lose, [R]ate\nPilihan: ";
        if (u->isAdmin) prompt = "[C]lose\nPilihan: ";
        string option = input_name(prompt);
        option = get_key(option);

        if (option == "c" || option == "close") return;
        else if ((option == "r" || option == "rate") && !u->isAdmin) tampil_rate(u, result);
    }
}

#endif