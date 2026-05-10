#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Hashing.h"

struct Entry {
    string  key;
    void*   value;
    Entry*  next = nullptr;

    Entry(string k, void* v) : key(k), value(v) {}
};

struct HashTable {
    Entry* bucket[TABLE_SIZE] = {};
    
    // ── Insert / Update ───────────────────────────
    void insert(const string& input_key, void* value) {
        string key = get_key(input_key);
        unsigned int idx  = hashing(key);
        Entry*       cur = bucket[idx];

        // update if bucket exists
        while (cur) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
            cur = cur->next;
        }

        // add new entry to front
        Entry* entry    = new Entry(key, value);
        entry->next     = bucket[idx];
        bucket[idx]    = entry;
    }

    // ── Search ────────────────────────────────────
    bool is_exist(const string& input_key) {
        string key = get_key(input_key);
        unsigned int idx  = hashing(key);
        Entry*       cur = bucket[idx];

        while (cur) {
            if (cur->key == key) {
                return true;
            }
            cur = cur->next;
        }
        return false;
    }

    void* get_exact(const string& input_key) {
        string key = get_key(input_key);
        unsigned int idx  = hashing(key);
        Entry*       cur = bucket[idx];

        while (cur) {
            if (cur->key == key)
                return cur->value;
            cur = cur->next;
        }
        return nullptr;
    }

    void get_match(const string& input_prefix, void* result[], int& count) {
        count = 0;
        string prefix = get_key(input_prefix);
        for (int i = 0; i < TABLE_SIZE && count < 5; i++) {
            Entry* cur = bucket[i];
            while (cur && count < 5) {
                if (cur->key.find(prefix) != string::npos) {
                    result[count] = cur->value;
                    count++;
                }
                cur = cur->next;
            }
        }
    }

    template <typename T>
    int get_all(T* result[], int maxSize) {
        int count = 0;
        for (int i = 0; i < TABLE_SIZE && count < maxSize; i++) {
            Entry* e = bucket[i];
            while (e && count < maxSize) {
                result[count] = (T*) e->value;
                count++;
                e = e->next;
            }
        }
        return count;
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