#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Hashing.h"

template <typename T>
struct Entry {
    string key;
    T value;
    Entry* next = nullptr;

    Entry(string k, T v) : key(k), value(v) {}
};

template <typename T>
struct HashTable {
    Entry<T>* bucket[TABLE_SIZE] = {};
    
    // ── Insert / Update ───────────────────────────
    void insert(const string& input_key, T value) {
        string key = get_key(input_key);
        unsigned int idx = hashing(key);
        Entry<T>* cur = bucket[idx];

        // update if bucket exists
        while (cur) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
            cur = cur->next;
        }

        // add new entry to front
        Entry<T>* entry = new Entry<T>(key, value);
        entry->next = bucket[idx];
        bucket[idx] = entry;
    }

    // ── Search ────────────────────────────────────
    bool is_exist(const string& input_key) {
        string key = get_key(input_key);
        unsigned int idx = hashing(key);
        Entry<T>* cur = bucket[idx];

        while (cur) {
            if (cur->key == key) {
                return true;
            }
            cur = cur->next;
        }
        return false;
    }

    T get_exact(const string& input_key) {
        string key = get_key(input_key);
        unsigned int idx = hashing(key);
        Entry<T>* cur = bucket[idx];

        while (cur) {
            if (cur->key == key)
                return cur->value;
            cur = cur->next;
        }
        return T();
    }

    int get_match(const string& input_keyword, T result[]) { //get matching + return count
        int count = 0;
        string keyword = get_key(input_keyword);
        for (int i = 0; i < TABLE_SIZE; i++) {
            Entry<T>* cur = bucket[i];
            while (cur) {
                if (cur->key.find(keyword) != string::npos) {
                    result[count] = cur->value;
                    count++;
                }
                cur = cur->next;
            }
        }
        return count;
    }

    int get_all(T result[]) { //get all + return count
        int count = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            Entry<T>* cur = bucket[i];
            while (cur) {
                result[count] = cur->value;
                count++;
                cur = cur->next;
            }
        }
        return count;
    }

    int get_all_keys(string keys[]) {
        int count = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            Entry<T>* cur = bucket[i];
            while (cur) {
                keys[count] = cur->key;
                count++;
                cur = cur->next;
            }
        } 
        return count;
    }

    void remove_entry(const string& input_key) {
        string key = get_key(input_key);
        unsigned int idx = hashing(key);
        Entry<T>* cur = bucket[idx];
        Entry<T>* prev = nullptr;
        while (cur) {
            prev = cur;
            if (cur->key == key) {
                prev->next = cur->next->next;
            }
            cur = cur->next;

        }

    }
};

#endif


/*
~ x.y, x = Bucket, y = chain
~ newest entry - top
~ oldest entry - bottom

            Entry0.0        
            Entry0.1                Entry24.0
            Entry0.2  Entry1.0      Entry24.1
Hashtable : Entry0.3, Entry1.1, ... Entry24.2

read entry bucket from top to bottom

*/