#ifndef NODES_H
#define NODES_H

#include "HashTable.h"

struct Jalur;
struct Inventory;
struct Gudang;
struct Barang;
struct QueuedBarang;

template <typename T>
struct Node {
    T data;
    Node* next = nullptr;
    Node(T d): data(d) {}
};

template <typename T>
struct Tree {
    T data;
    Tree* left = nullptr;
    Tree* right = nullptr;
    Tree(T d): data(d) {}
};


struct Barang {
    string nama;
    Node<Gudang*>* gudang = nullptr; //available in gudang

    bool is_gudang_exist(Gudang* g) {
        Node<Gudang*>* cur = gudang;
        while (cur) {
            if (cur->data == g) return true;
            cur = cur->next;
        }
        return false;
    }

    void add_gudang(Gudang* g) {
        if (!is_gudang_exist(g)) {
            Node<Gudang*>* newG = new Node<Gudang*>(g);
            if (!gudang) {
                gudang = newG;
                return;
            }
            newG->next = gudang;
            gudang = newG;
        }
    }


    Barang(string n): nama(n) {}
};

struct Gudang {
    string nama;
    Jalur* lokasi_gudang = nullptr;

    HashTable<int> barang_gudang; // key: barang->nama, value: jumlah barang dalam gudang

    void update_barang_gudang(Barang* b, int jumlah) {
        jumlah += barang_gudang.get_exact(b->nama);
        barang_gudang.insert(b->nama, jumlah);
    }

    Gudang(string n, Jalur* l): nama(n), lokasi_gudang(l) {}
};

struct QueuedBarang {
    Barang* barang = nullptr;
    Gudang* gudang = nullptr;
    int jumlah;
    string tipe_proses;
    QueuedBarang(Barang* b, Gudang* g, int j, string t): barang(b), gudang(g), jumlah(j), tipe_proses(t) {}
};

void add_to_BST(Tree<Barang*>*& root, Barang* b, const string& key_nama);
struct Inventory {
    Jalur* lokasi_inv = nullptr;

    Tree<Barang*>* root = nullptr;
    HashTable<Barang*> daftar_barang; //key: nama barang, value = Barang*
    HashTable<int> jumlah_barang; //key: nama barang, value = jumlah Barang dari semua Gudang

    HashTable<Gudang*> list_gudang; //key: nama gudang, value = Gudang*

    Node<string>* riwayat_head = nullptr; //aktivitas, Head (stack) add to front
    Node<QueuedBarang*>* queue_head = nullptr; //barang jumlah terendah di inv
    Node<QueuedBarang*>* queue_tail = nullptr;


    void add_barang_to_inv (Barang* b) {
        daftar_barang.insert(b->nama, b);
        jumlah_barang.insert(b->nama, 0);
        add_to_BST(root, b, get_key(b->nama));
    }

    void update_barang_global(Barang* b, int jumlah) {
        jumlah += jumlah_barang.get_exact(b->nama);
        jumlah_barang.insert(b->nama, jumlah);
    }

    void add_gudang_to_inventory(Gudang* g) {
        list_gudang.insert(g->nama, g);
    }
    
    void enqueue_demand(Barang* b, Gudang* g, int j, string t) {
        QueuedBarang* newQ = new QueuedBarang(b, g, j, t);
        Node<QueuedBarang*>* new_node = new Node<QueuedBarang*>(newQ);
        if (!queue_head) {
            queue_head = queue_tail = new_node;
            return;
        }
        queue_tail->next = new_node;
        queue_tail = new_node;
    }

    QueuedBarang* dequeue_demand(string tipe_proses) {
        if (!queue_head) return nullptr;
        Node<QueuedBarang*>* cur = queue_head, *prev = queue_head;
        
        while (cur && cur->data->tipe_proses != tipe_proses) {
            prev = cur;
            cur = cur->next;
        }
        
        if (!cur) return nullptr;

        if (!prev) queue_head = cur->next; //if head
        else prev->next = cur->next;

        if (cur == queue_tail) queue_tail = prev; //if tail

        QueuedBarang* saved = cur->data;

        delete cur;
        return saved;
    }

    void push_history(string prompt) {
        Node<string>* new_node = new Node<string>(prompt);
        if (!riwayat_head) {
            riwayat_head = new_node;
            return;
        }
        new_node->next = riwayat_head;
        riwayat_head = new_node;
    }

    string pop_history() {
        if (!riwayat_head) return "";
        string saved = riwayat_head->data;
        Node<string>* del = riwayat_head;
        riwayat_head = riwayat_head->next;

        delete del;
        return saved;
    }
};
Inventory inventory;



const int MAX_LOKASI = 15;
const int INF = 99999;
int jumlah_lokasi = 0;
string nama_semua_lokasi[MAX_LOKASI]; //idx to lokasi
int waktu_tempuh[MAX_LOKASI][MAX_LOKASI]; //waktu tempuh dari a ke b
HashTable<int> nama_lokasi_to_idx; // key: nama lokasi, value: idx
HashTable<Jalur*> jalur_ke_semua_lokasi;  // key: nama lokasi, value: lokasi dengan data" jalur

void init_graph_lokasi() {
    for (int i = 0; i < jumlah_lokasi; i++)
        for (int j = 0; j < jumlah_lokasi; j++)
            waktu_tempuh[i][j] = (i == j) ? 0 : INF;
}

void assign_lokasi_to_idx(const string& lokasi, int idx) {
    nama_lokasi_to_idx.insert(get_key(lokasi), idx);
}

int lokasi_to_idx(const string& lokasi) {
    return nama_lokasi_to_idx.get_exact(get_key(lokasi));
}

void tambah_rute(const string& dari, const string& ke, int waktu) {
    int dari_idx = lokasi_to_idx(dari);
    int ke_idx = lokasi_to_idx(ke);
    // cout << "dari " << dari_idx << " ke " << ke_idx << endl;
    waktu_tempuh[dari_idx][ke_idx] = waktu;
    waktu_tempuh[ke_idx][dari_idx] = waktu;
}


struct Jalur {
    string lokasi_awal;
    int waktu_ke[MAX_LOKASI];       // jarak ke idx lokasi
    int lokasi_sebelum[MAX_LOKASI]; // lokasi sebelum dari tujuan lokasi

    void hitung_waktu_tempuh() { //dijkstra algorithm
        int asal = lokasi_to_idx(lokasi_awal);
        bool visited[MAX_LOKASI];

        for (int i = 0; i < jumlah_lokasi; i++) { //init value awal
            waktu_ke[i] = INF;
            visited[i] = false;
            lokasi_sebelum[i] = -1; //flag belum ada jalur lokasi sebelum
        }

        waktu_ke[asal] = 0;

        for (int iter = 0; iter < jumlah_lokasi; iter++) {
            // Cari node belum dikunjungi dengan waktu terkecil
            int cur = -1; //flag kosong
            for (int i = 0; i < jumlah_lokasi; i++) {
                if (visited[i]) 
                    continue;
                
                if (cur == -1 || waktu_ke[i] < waktu_ke[cur])
                    cur = i; //tandai cur (sedang ngecek tetangga siapa)
            }
            if (waktu_ke[cur] == INF) break;
            visited[cur] = true;

            // Update waktu tetangga (adjacent)
            for (int adj = 0; adj < jumlah_lokasi; adj++) {
                if (waktu_tempuh[cur][adj] == INF)
                    continue;

                if (waktu_ke[cur] + waktu_tempuh[cur][adj] < waktu_ke[adj]) {
                    waktu_ke[adj] = waktu_ke[cur] + waktu_tempuh[cur][adj];
                    lokasi_sebelum[adj] = cur;
                }
            }
        }
    }

    Jalur(string l): lokasi_awal(l) {}
};

#endif