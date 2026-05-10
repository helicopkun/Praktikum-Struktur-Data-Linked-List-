#ifndef HASH_H
#define HASH_H

using namespace std;
#include <string>

string trim(const string& s) {
    int start = s.find_first_not_of(" \t\n\r");
    int end   = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string get_key(const string& text) {
    string newtext = "";
    for (char c : text) {
        if (c == ' ') continue;
        newtext += tolower(static_cast<unsigned char>(c));
    }
    return newtext;
}

#define TABLE_SIZE 25
unsigned int hashing(const string& key) {
    unsigned int h = 0;
    for (char c : key)
        h = h * 31 + c;
    return h % TABLE_SIZE;
}

#endif