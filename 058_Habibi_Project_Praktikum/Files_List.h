#ifndef FILES_H
#define FILES_H

#include "Linked_List.h"
#include <iostream>
#include <fstream>
using std::cout;

// ── Read ───────────────────────────
void _load_lokasi() { 
    ifstream file("../lokasi.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file lokasi.txt\n";
        return;
    }
    string curLine;
    string lokasi = "";
    int idx = 0;
    int i = 0;
    string str_waktu;
    int waktu;

    while(getline(file, curLine) && idx < MAX_LOKASI) { //get all location name + idx
        if (curLine.empty()) break;

        while (curLine[i] != '|') 
            lokasi += curLine[i++];
        
        lokasi = trim(lokasi);
        // cout << "Lokasi awal init: " << lokasi << endl;
        assign_lokasi_to_idx(lokasi, idx);
        nama_semua_lokasi[idx] = lokasi;

        Jalur* newJ = new Jalur(lokasi);
        jalur_ke_semua_lokasi.insert(lokasi, newJ);

        lokasi = "";
        i = 0;
        idx++;
    }

    jumlah_lokasi = idx;
    // cout << jumlah_lokasi << endl;
    init_graph_lokasi();

    file.clear();
    file.seekg(0, std::ios::beg); //start from beg

    idx = 0;
    i = 0;
    lokasi = "";
    while(getline(file, curLine) && idx < jumlah_lokasi) { //get all location route + weight
        // cout << "idx: " << idx << endl;
        if (trim(curLine).empty()) break;
        // cout << curLine << endl;
        while (curLine[i] != '|') i++;
        i++; //skip |
        while (i < curLine.size()) {
            while (curLine[i] != ':') lokasi += curLine[i++];
            i++; //skip :
            while (curLine[i] != '|') str_waktu += curLine[i++];
            i++; //skip |
            lokasi = trim(lokasi);
            str_waktu = trim(str_waktu);
            waktu = stoi(str_waktu);

            // cout << "rute: " << nama_semua_lokasi[idx] << " ke " << lokasi << " dengan " << waktu << endl;
            tambah_rute(nama_semua_lokasi[idx], lokasi, waktu);
            lokasi = "";
            str_waktu = "";
        }
        i = 0;
        idx++;
        // cout << "\n\n";
    }

    Jalur* list_jalur[jumlah_lokasi];
    jalur_ke_semua_lokasi.get_all(list_jalur);

    for (Jalur* j : list_jalur) {
        j->hitung_waktu_tempuh();
        // cout << "hitung jarak berhasil";
    }
    file.close();
}

void _load_inventory() {
    ifstream file("../inventory.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file inventory.txt\n";
        return;
    }
    string curLine;
    char barang [100];
    int jumlah;

    // ── add location ─────────────────────────-
    getline(file, curLine);
    inventory.lokasi_inv = jalur_ke_semua_lokasi.get_exact(curLine);

    // ── add new barang ──────────────────────────
    while (getline(file, curLine)) {
        if (trim(curLine).empty()) continue;

        sscanf(curLine.c_str(), "%[^|]|%d", barang, &jumlah);
        Barang* newB;

        if (inventory.daftar_barang.is_exist(barang)){ //in-case duplicated
            newB = inventory.daftar_barang.get_exact(barang);
        } else {
            newB = new Barang(trim(barang));
        }
        
        inventory.add_barang_to_inv(newB);
        inventory.update_barang_global(newB, jumlah);
    }

    file.close();
}

void _load_gudang() {
    ifstream file("../gudang.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file gudang.txt\n";
        return;
    }
    string curLine;
    char barang [100];
    int jumlah;
    
    while (getline(file, curLine)) {
        if (trim(curLine).empty()) continue;

        char nama[100] = "";
        char lokasi[100] = "";

        sscanf(curLine.c_str(), "%[^,],%[^\n]", nama, lokasi);

        Gudang* newG = new Gudang(trim(nama), jalur_ke_semua_lokasi.get_exact(lokasi));
        inventory.add_gudang_to_inventory(newG);

        while (getline(file, curLine)) {//barang"
            if (trim(curLine) == "eol") break;
            
            sscanf(curLine.c_str(), "%[^|]|%d", barang, &jumlah);
            if (!inventory.daftar_barang.is_exist(barang)) { //unexpected item from inventory, very unlikely might remove
                cout << "Warning, " << trim(barang) << " doesnt exist in inventory.\n";
                continue;
            }
            Barang* dari_inv = inventory.daftar_barang.get_exact(barang);
            link_gudang_to_barang(newG, dari_inv, jumlah);
        }
    }
    file.close();
}

void _load_riwayat() {
    ifstream file("../riwayat.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file riwayat.txt\n";
        return;
    }
    string curLine;
    string lines[1000];
    int count = 0;
    while(getline(file, curLine)) {
        if (trim(curLine).empty()) continue;
        lines[count++] = trim(curLine);
    }
    for (int i = count - 1; i >= 0; i--) {
        // cout << lines[i] << endl;
        inventory.push_history(lines[i]);
    }
    
    file.close();
}

void _load_queued() {
    ifstream file("../queued.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file queued.txt\n";
        return;
    }

    string curLine;
    char proses[100];
    char barang[100];
    char gudang[100];
    int jumlah;

    while (getline(file, curLine)) {
        if (trim(curLine).empty()) continue;

        sscanf(curLine.c_str(), "%[^|]|%[^|]|%[^|]|%d", proses, barang, gudang, &jumlah);
        if (inventory.daftar_barang.is_exist(barang) && inventory.list_gudang.is_exist(gudang))
            inventory.enqueue_demand(inventory.daftar_barang.get_exact(barang), 
                                    inventory.list_gudang.get_exact(gudang),
                                    jumlah, proses);

    }

    file.close();
}

void load_files() {
    
    // cout << "hitung\n";
    _load_lokasi();
    // cout << "hitung berhasil\n";
    
    _load_inventory();
    _load_gudang();
    _load_riwayat();
    _load_queued();
}




// ── Write ───────────────────────────
void _save_inventory_BST(ofstream& file, Tree<Barang*>* root) { //pre-order (avoid sorting alphabetically)
    if (!root) return;

    file << root->data->nama << "|" << inventory.jumlah_barang.get_exact(root->data->nama) << endl;

    _save_inventory_BST(file, root->left);
    _save_inventory_BST(file, root->right);
}

void save_inventory() { // rewrite
    ofstream file("../inventory.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file inventory.txt\n";
        return;
    }

    file << inventory.lokasi_inv->lokasi_awal << endl;
    _save_inventory_BST(file, inventory.root);

    file.close();
}

void save_gudang() {
    ofstream file("../gudang.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file gudang.txt\n";
        return;
    }

    Gudang* list_gudang[100];
    int size = inventory.list_gudang.get_all(list_gudang);
    int countSize = 0;
    for (Gudang* g : list_gudang) {
        if (countSize++ >= size) break;
        string nama_gudang = g->nama;
        string lokasi_gudang = g->lokasi_gudang->lokasi_awal;

        file << nama_gudang << "," << lokasi_gudang << endl;

        string nama_barang;
        int jumlah_barang;
        string list_keys[100];
        int count = g->barang_gudang.get_all_keys(list_keys);
        for (int i = 0; i < count; i++) {
            nama_barang = inventory.daftar_barang.get_exact(list_keys[i])->nama;
            jumlah_barang = g->barang_gudang.get_exact(list_keys[i]);

            file << nama_barang << "|" << jumlah_barang << endl;
        }

        file << "eol" << "\n\n";
    }


    file.close();
}

void save_lokasi() {
    ofstream file("../lokasi.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file lokasi.txt\n";
        return;
    }

    Jalur* list_jalur[100];
    jalur_ke_semua_lokasi.get_all(list_jalur);
    for (Jalur* j: list_jalur) {
        file << j->lokasi_awal << "|";

        for (int i = 0; i < jumlah_lokasi; i++) {
            if (waktu_tempuh[lokasi_to_idx(j->lokasi_awal)][i] != INF //tidak ada jalur langsung
             && waktu_tempuh[lokasi_to_idx(j->lokasi_awal)][i] != 0) //lokasi yang sama
                file << nama_semua_lokasi[i] << ":" 
                     << waktu_tempuh[lokasi_to_idx(j->lokasi_awal)][i] << "|";
        }

        file << endl;
    }

    file.close();
}

void save_queued() {
    ofstream file("../queued.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file queued.txt\n";
        return;
    }
    int count = 0;
    Node<QueuedBarang*>* cur = inventory.queue_head;
    while (cur) {
        file << cur->data->tipe_proses << "|"
             << cur->data->barang->nama << "|" 
             << cur->data->gudang->nama << "|" 
             << cur->data->jumlah
             << "\n\n";
        cur = cur->next;
        count++;
    }
    file.close();
}

void save_riwayat() {
    ofstream file("../riwayat.txt");
    if (!file.is_open()) {
        cout << "Error: cannot open file riwayat.txt\n";
        return;
    }

    Node<string>* cur = inventory.riwayat_head;
    while (cur) {
        file << cur->data << "\n\n";
        cur = cur->next;
    }

    file.close();
}

#endif