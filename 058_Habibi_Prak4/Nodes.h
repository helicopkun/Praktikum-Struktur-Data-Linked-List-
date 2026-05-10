#ifndef NODES_H
#define NODES_H

#include "HashTable.h"

struct Genre;
struct Movies;
struct RatingEntry;
struct User;

struct Genre {
    string nama;
    HashTable movies; // key: Movies->nama,  value: Movies*

    Genre(string n): nama(n) {}
};

struct Movies {
    string nama, studio;
    int jumlah_episode = 0, jumlah_season = 0;
    float total_rating = 0.0f;

    HashTable film_terkait; // key: Movies->nama,  value: Movies*
    HashTable genres;       // key: Genre->nama,   value: Genre*
    HashTable rated_by;     // key: User->nama,  value: RatingEntry*

    Movies* left = nullptr; 
    Movies* right = nullptr;
    
    bool is_film() {
        return jumlah_episode == 0 && jumlah_season == 0;
    }
    Movies(string n, string st, int e, int ss, float r): nama(n), studio(st), jumlah_episode(e), jumlah_season(ss), total_rating(r) {}
};

struct RatingEntry {
    float rate = 0.0f;
    Movies* movie = nullptr;

    RatingEntry(float r, Movies* m): rate(r), movie(m) {}
};

struct User {
    string  nama, password;
    HashTable rated; // key: Movies->nama, value: RatingEntry*
    bool isAdmin = false;
    User(string n, string p): nama(n), password(p) {}
};

Movies* BST = nullptr; // global movie, sorted via names (Binary Search Tree)
HashTable Genres;      // global genre hashtable, key: Genre->nama, value: Genre*
HashTable Users;       // global user hashtable, key: User->nama, value: User*

#endif