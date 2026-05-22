#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Nodes.h"

// ── Inventory ───────────────────────────


// ── Gudang ───────────────────────────
void link_gudang_to_barang (Gudang* g, Barang* b, int jumlah) {
    g->barang_gudang.insert(b->nama, jumlah);
    b->add_gudang(g);
}

// ── Barang ───────────────────────────
void add_to_BST(Tree<Barang*>*& root, Barang* b, const string& key_nama) {
    if (root == nullptr) {
        Tree<Barang*>* newT = new Tree<Barang*>(b);
        root = newT;
        return;
    }
    // string nama = get_key(b->nama);
    string key = get_key(root->data->nama);
    if (key_nama < key) add_to_BST(root->left, b, key_nama);
    else add_to_BST(root->right, b, key_nama); 
}

Barang* search_BST(Tree<Barang*>* root, const string& key_nama) { // BST
    if (!root) return nullptr;
    // string nama = get_key(nama_input);
    string key = get_key(root->data->nama);

    if (key_nama == key) return root->data;
    if (key_nama < key) return search_BST(root->left, key_nama);
    else return search_BST(root->right, key_nama);
}

void search_match_BST(Tree<Barang*>* root, const string& key_word, Barang* result[], int& count) { //in-order
    if (!root) return;
    search_match_BST(root->left, key_word, result, count);
    if (get_key(root->data->nama).find(key_word) != string::npos) result[count++] = root->data;
    search_match_BST(root->right, key_word, result, count);
}

void get_all_BST(Tree<Barang*>* root, Barang* result[], int& count) {
    if (!root) return;
    get_all_BST(root->left,  result, count);
    result[count++] = root->data;
    get_all_BST(root->right, result, count);
}


#endif