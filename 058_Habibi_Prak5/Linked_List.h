#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Nodes.h"

// ── Genre ───────────────────────────
void link_genre_to_movie (Genre* g, Movies* m) {
    g->movies.insert(m->nama, m);
    m->genres.insert(g->nama, g);
}

void add_genre_to_table(Genre* g) {
    Genres.insert(g->nama, g);
}


// ── Movie ───────────────────────────
void link_movie_to_universe(Movies* m, Movies* U) {
    m->film_terkait.insert(U->nama, U);
    U->film_terkait.insert(m->nama, m);
}

void add_movie_to_BST(Movies*& root, Movies* m) {
    if (root == nullptr) {
        root = m;
        return;
    }
    string nama = get_key(m->nama);
    string key = get_key(root->nama);
    if (nama < key) add_movie_to_BST(root->left, m);
    else            add_movie_to_BST(root->right, m); 
}

Movies* search_movie_BST(Movies* root, const string& nama) { // BST
    if (!root) return nullptr;
    // string nama = get_key(nama_input);
    string key = get_key(root->nama);

    if (nama == key) return root;
    if (nama < key) return search_movie_BST(root->left, nama);
    else            return search_movie_BST(root->right, nama);
}

void search_match_movie_BST(Movies* root, const string& prefix, Movies* result[], int& count) { //in-order
    if (!root) return;
    // string prefix = get_key(prefix_input);
    // string key = get_key(root->nama);
    
    search_match_movie_BST(root->left, prefix, result, count);
    if (get_key(root->nama).find(prefix) != string::npos) result[count++] = root;
    search_match_movie_BST(root->right, prefix, result, count);
}

void get_all_movie_BST(Movies* root, Movies* result[], int& count, string option) { //in-order, option = ["Film / Series", "Film", "Series"]
    if (!root) return;
    get_all_movie_BST(root->left,  result, count, option);

    if (option == "Film / Series") result[count++] = root;

    else if (option == "Film") 
        if (root->is_film())
            result[count++] = root;

    else if (option == "Series") 
        if (!root->is_film())
            result[count++] = root;

    get_all_movie_BST(root->right, result, count, option);
}


// ── User ───────────────────────────
void link_userRate_to_movie(User* u, Movies* m, float rate) {
    if (u->rated.is_exist(m->nama)) {
        RatingEntry* cur = (RatingEntry*) u->rated.get_exact(m->nama);
        cur->rate = rate;
        return;
    }
    RatingEntry* cur = new RatingEntry(rate, m);
    m->rated_by.insert(u->nama, cur);
    u->rated.insert(m->nama, cur);
}

void add_user_to_table(User* u) {
    Users.insert(u->nama, u);
}




#endif