#ifndef FILES_H
#define FILES_H

#include "Linked_List.h"
#include <iostream>
#include <fstream>

// ── Read ───────────────────────────
void load_movies() {
    ifstream file("../movies.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file movies.txt\n";
        return;
    }
    string curLine;
    char nama[100], studio[100], genreLine[500], relLine[500];
    int   ep, season;
    float rating;

    // ── add new movie + genre ──────────────────────────
    while (getline(file, curLine)) {
        if (trim(curLine).empty()) continue;

        // ── movie line ──
        sscanf(curLine.c_str(), "%[^|]| %[^|]| %d| %d| %f", 
                                nama, studio, &ep, &season, &rating);

        Movies* newM = new Movies(nama, studio, ep, season, rating);
        add_movie_to_BST(BST, newM);
        
        // ── genre line ──
        getline(file, curLine);
        string genre = "";
        Genre* g = nullptr;
        
        for (int i = 0; i < curLine.size(); i++) {
            if (curLine[i] == '|') {
                if (!Genres.is_exist(genre)){
                    g = new Genre(genre);
                    add_genre_to_table(g);
                } else {
                    g = (Genre*) Genres.get_exact(genre);
                }
                link_genre_to_movie(g, newM);
                genre = "";
                i++;  // skip space after |
            } else {
                genre += curLine[i];
            }
        }
        
        getline(file, curLine); // skip relation line, empty until BST is completed
    }

    // Movies* list[100] = {}; //list works
    // int count = 0;
    // get_all_movie_BST(BST, list, count, "Film / Series");
    // for (int i = 0; i < count; i++) {
    //     cout << list[i]->nama << "-list\n";
    // }

    file.clear();
    file.seekg(0, std::ios::beg); //reset file ptr to beginning

    // ── link related movie ────────────────────────────
    while (getline(file, curLine)) {
        if (trim(curLine).empty()) continue;

        // ── movie line ──
        sscanf(curLine.c_str(), "%[^|]|", nama);
        // cout << endl << nama << "2nd\n"; //debug
        Movies* cur = search_movie_BST(BST, get_key(nama));
        getline(file, curLine); // skip genre line
        
        // ── related line ───
        getline(file, curLine);
        string related_name = "";
        Movies* U = nullptr;
        
        for (int i = 0; i < curLine.size(); i++) {
            if (curLine[i] == '|') {
                // cout << related_name << " and " << cur->nama << endl; //debug
                if (!cur->film_terkait.is_exist(related_name)) {
                    
                    U = search_movie_BST(BST, get_key(related_name));
                    if (U) link_movie_to_universe(cur, U);
                }
                related_name = "";
                i++; // skip space after |
            } else {
                related_name += curLine[i];
            }
        }
        
    }
}

void load_users() {
    ifstream file("../users.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file users.txt\n";
        return;
    }
    string curLine;
    char nama[100], password[100];

    while (getline(file, curLine)) {
        if (trim(curLine).empty()) continue;

        sscanf(curLine.c_str(), "%[^|]| %s", nama, password);
        User* newU = new User(nama, password);
        add_user_to_table(newU);

        getline(file, curLine);
        if (trim(curLine).empty()) continue;

        string  movie_name = "", 
                rating_str = "";
        Movies* m = nullptr;
        for (int i = 0; i < curLine.size(); i++) {
            if (curLine[i] == '>') {
                i += 2; //skip space before rating
                while(curLine[i] != '|') {
                    rating_str += curLine[i];
                    i++;
                }
                m = search_movie_BST(BST, get_key(movie_name));
                if (m) link_userRate_to_movie(newU, m, stof(rating_str));
                
                movie_name = rating_str = "";
                i++; // skip space after |
            } else {
                movie_name += curLine[i];
            }
        }
    }
}




// ── Write ───────────────────────────
void save_movies_BST(ofstream& file, Movies* root) { //pre-order (avoid sorting alphabetically)
    if (!root) return;

    // ── movie line ────────────────────────────────
    file << root->nama            << "| "
         << root->studio          << "| "
         << root->jumlah_episode  << "| "
         << root->jumlah_season   << "| "
         << root->total_rating    << "\n";

    // ── genre line ───────────────────────────────
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* e = root->genres.bucket[i];
        while (e) {
            file << ((Genre*)e->value)->nama << "| ";
            e = e->next;
        }
    }
    file << "\n";

    // ── relation line ────────────────────────────
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* e = root->film_terkait.bucket[i];
        while (e) {
            file << ((Movies*)e->value)->nama << "| ";
            e = e->next;
        }
    }
    file << "\n\n";

    save_movies_BST(file, root->left);
    save_movies_BST(file, root->right);
}

void save_movies() { // rewrite
    ofstream file("../movies.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file movies.txt\n";
        return;
    }
    save_movies_BST(file, BST);
    file.close();
}


void write_user(ofstream& file, User* u) {
    file << u->nama << "| " 
         << u->password << '\n';

    for (int j = 0; j < TABLE_SIZE; j++) {
        Entry* e = u->rated.bucket[j];
        while (e) {
            RatingEntry* r = (RatingEntry*) e->value;
            file << r->movie->nama << "> "
                    << r->rate << "| ";
            e = e->next;
        }
    }
    file << "\n\n";
}

void append_user(User* u) { //app
    ofstream file("../users.txt", ios::app);
    if (!file.is_open()) {
        cout << "Error: cannot open file\n";
        return;
    }
    write_user(file, u);
    file.close();
}

void save_users() { //rewrite
    ofstream file("../users.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file users.txt\n";
        return;
    }
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* e = Users.bucket[i];
        while (e) {
            write_user(file, (User*)e->value);
            e = e->next;
        }
    }
    file.close();
}

#endif