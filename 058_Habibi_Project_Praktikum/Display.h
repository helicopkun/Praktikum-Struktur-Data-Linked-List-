#ifndef DISPLAY_H
#define DISPLAY_H

#include "Files_List.h"

// ── Inputs ───────────────────────────
void press_enter() {
    string dummy;
    cout << "\nPress Enter to continue...";
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

// ── General ───────────────────────────
void tampil_barang(Barang* list_barang[], int list_jumlah[], int size) {
    if (size == 0) cout << "Empty...\n";
    for (int i = 0; i < size; i++) {
        cout << i+1 << ". " << list_barang[i]->nama;
        if (i+1 >= 10 && i+1 < 100) cout << " ";
        else if (i+1 < 10) cout << "  ";
        for (int j = list_barang[i]->nama.size(); j < 25; j++) (j % 2 == 0) ? cout << "-" : cout << " ";
        cout << "stok: " << list_jumlah[i] << "\n";
    }
    cout << "\n";
}

void tampil_detail(Barang* b) {
    system("cls");
    cout << "\t====" << b->nama << "====\n";
    cout << "Total stok: " << inventory.jumlah_barang.get_exact(b->nama);
    cout << "\nLokasi inventory: " << inventory.lokasi_inv->lokasi_awal;
    cout << "\n\nAda di: \n";
    Node<Gudang*>* cur = b->gudang;
    
    int lokasi_inv = lokasi_to_idx(inventory.lokasi_inv->lokasi_awal);
    while (cur) {
        cout << " " << cur->data->nama << ", " << cur->data->lokasi_gudang->lokasi_awal
        << "\n Sebanyak: " << cur->data->barang_gudang.get_exact(b->nama)
        << "\n Dengan waktu tempuh: "
        << cur->data->lokasi_gudang->waktu_ke[lokasi_inv]
        << " Hari"
        << "\n Rute (ke inventory): ";
        int cur_lokasi = lokasi_to_idx(cur->data->lokasi_gudang->lokasi_awal);
        while (cur_lokasi != -1) {
            cout << nama_semua_lokasi[cur_lokasi];
            cur_lokasi = inventory.lokasi_inv->lokasi_sebelum[cur_lokasi];
            if (cur_lokasi != -1) cout << " -> ";
        }
        cout << "\n\n";
        cur = cur->next;
    }
}

void tampil_gudang(Gudang* list[], int size, Jalur* lokasi_awal, Barang* b = nullptr, bool lokasi_awal_is_target = false) {
    for (int i = 0; i < size; i++) {
        cout << i + 1 << ". " << list[i]->nama << ", " << list[i]->lokasi_gudang->lokasi_awal;
        if (b) cout << "  Stok: " << list[i]->barang_gudang.get_exact(b->nama);
        cout << "\n";
        cout << "Waktu tempuh: " << lokasi_awal->waktu_ke[lokasi_to_idx(list[i]->lokasi_gudang->lokasi_awal)];
        cout << " Hari, Rute: ";
        int cur_lokasi;
        if (lokasi_awal_is_target) cur_lokasi = lokasi_to_idx(list[i]->lokasi_gudang->lokasi_awal);
        else cur_lokasi = lokasi_to_idx(lokasi_awal->lokasi_awal);
        while (cur_lokasi != -1) {
            cout << nama_semua_lokasi[cur_lokasi];
            if (lokasi_awal_is_target) cur_lokasi = lokasi_awal->lokasi_sebelum[cur_lokasi];
            else cur_lokasi = list[i]->lokasi_gudang->lokasi_sebelum[cur_lokasi];
            if (cur_lokasi != -1) cout << " -> ";
        }
        cout << "\n\n";
    }
}

void sort_closest_gudang(Gudang* result[], int size, Jalur* lokasi_awal) {
    Gudang* tempB;
    for (int i = 0; i < size; i++) {
        int min = i;
        for (int j = i; j < size; j++) {
            if (lokasi_awal->waktu_ke[lokasi_to_idx(result[j]->lokasi_gudang->lokasi_awal)] <
                lokasi_awal->waktu_ke[lokasi_to_idx(result[min]->lokasi_gudang->lokasi_awal)])
                min = j;
        }
        tempB = result[i];
        result[i] = result[min];
        result[min] = tempB;
    }
}

int get_gudang(Barang* b, Jalur* lokasi_awal, Gudang* result[], bool include_same = true) {
    Node<Gudang*>* cur = b->gudang;
    int size = 0;
    while (cur) {
        if (!include_same && cur->data->lokasi_gudang->lokasi_awal == lokasi_awal->lokasi_awal) {
            cur = cur->next;
            continue;
        }
        result[size++] = cur->data;
        cur = cur->next; 
    }
    sort_closest_gudang(result, size, lokasi_awal);
    return size;
}

void sort_list_amount(Barang* list_barang[], int list_jumlah[], int size) { //selection sort
    int temp;
    Barang* tempB;
    for (int i = 0; i < size; i++) {
        int min = i;
        for (int j = i; j < size; j++) {
            if (list_jumlah[j] < list_jumlah[min]) min = j;
        }
        temp = list_jumlah[i];
        tempB = list_barang[i];
        list_jumlah[i] = list_jumlah[min];
        list_barang[i] = list_barang[min];
        list_jumlah[min] = temp;
        list_barang[min] = tempB;
    }
}

void sort_list_name(Barang* list_barang [], int list_jumlah []) {
    int size = 0;
    get_all_BST(inventory.root, list_barang, size);
    for (int i = 0; i < size; i++) {
        list_jumlah[i] = inventory.jumlah_barang.get_exact(list_barang[i]->nama);
    }
}

void queue_messages(QueuedBarang* data) {
    if (data->tipe_proses == "withdraw") {
            cout << "  Process type: Withdrawal, ";
            cout << "Withdrawn " << data->jumlah 
            << " " << data->barang->nama 
            << " From " << data->gudang->nama;
    }
    else if (data->tipe_proses == "transferee") {
        cout << "  Process type: Tranfered, ";
        cout << "Awaiting transfer of " << data->jumlah 
        << " " << data->barang->nama 
        << " to " << data->gudang->nama;
    }
}

void dequeue_type (Node<QueuedBarang*>* head, string process_type) {
    Node<QueuedBarang*>* cur = head;
    while (cur) {
        if (cur->data->tipe_proses == "withdraw" && process_type == "withdraw") {
            do {
                cout << "Process type: Withdrawal, ";
                cout << "Withdrawal of " << cur->data->jumlah 
                << " " << cur->data->barang->nama 
                << " From " << cur->data->gudang->nama
                << " Successful ?";
                string input = input_name("\n  [Y][N] Pilihan: ");
                if (input == "N" || input == "n") return;
                if (input == "Y" || input == "y") {
                    QueuedBarang* q = inventory.dequeue_demand(process_type);
                    inventory.update_barang_global(q->barang, -(q->jumlah));
                    string prompt = "Successfully withdrawn "+to_string(q->jumlah)+" from "+q->gudang->nama;
                    cout << "\n\n" << prompt;
                    inventory.push_history(prompt);
                    save_inventory();
                    save_queued();
                    save_riwayat();
                    return;
                }
                system("cls");
            } while(true);
        }
        else if (cur->data->tipe_proses == "transferee" && process_type == "transferee") {
            do {
                cout << "Process type: Trasnfered, ";
                cout << "Receiving amount of " << cur->data->jumlah 
                << " " << cur->data->barang->nama 
                << " To " << cur->data->gudang->nama
                << " Successful ?";
                string input = input_name("\n  [Y][N] Pilihan: ");
                if (input == "N" || input == "n") return;
                if (input == "Y" || input == "y") {
                    QueuedBarang* q = inventory.dequeue_demand(process_type);
                    q->gudang->barang_gudang.insert(q->barang->nama, q->jumlah);
                    string prompt = q->gudang->nama+" has received "+to_string(q->jumlah)+" "+q->barang->nama;
                    cout << "\n\n" << prompt;
                    inventory.push_history(prompt);
                    save_gudang();
                    save_queued();
                    save_riwayat();
                    return;
                }
                system("cls");
            } while(true);
        }

        cur = cur->next;
        if (!cur) {
            cout << "No process type: ";
            (process_type == "withdraw") ? cout << " Withdrawal " : cout << " Transfered ";
            cout << "Available in the moment..\n";
        }
    }
    return;
}

// ── Menu ───────────────────────────
void menu_inventory_management() {
    Barang* list_barang [100];
    int list_jumlah [100];
    int size = inventory.daftar_barang.get_all(list_barang);
    for (int i = 0; i < size; i++) {
        list_jumlah[i] = inventory.jumlah_barang.get_exact(list_barang[i]->nama);
    }
    
    int pilihan;
    int subI;
    string subS;
    while (true) {
        system("cls");
        cout << "===Inventory Management===\nCurrent location: " << inventory.lokasi_inv->lokasi_awal << "\n\n";
        tampil_barang(list_barang, list_jumlah, size);
        pilihan = input_idx("  Item:\n[0]Exit, [1]Sort, [2]Detail, [3]Search\n[4]Withdraw, [5]Register, [6]Change location\nPilihan: ", 0, 6);
        cout << "\n\n";
        if (pilihan == 0) break;

        else if (pilihan == 1) {
            subI = input_idx("  Sort By:\n[0]Exit, [1]Nama, [2]Jumlah\nPilihan: ", 0, 2);
            if (subI == 1) sort_list_name(list_barang, list_jumlah);
            else if (subI == 2) sort_list_amount(list_barang, list_jumlah, size);
        }

        else if (pilihan == 2) {
            subI = input_idx("  Pick item:\n[0]Exit, [1-" + to_string(size) + "]Barang\nPilihan: ", 0, size);
            if (subI == 0) continue;
            tampil_detail(list_barang[subI - 1]);
            press_enter();
        }

        else if (pilihan == 3) {
            subS = input_name(" Search item:\n[0]Exit, [Keyword]Item\nPilihan: ");
            if (subS != "0") {
                Barang* match[100];
                int jumlah[100];
                int count = 0;
                search_match_BST(inventory.root, get_key(subS), match, count);
                if (count == 0) {
                    cout << "No matches found.\n";
                    press_enter();
                    continue;
                }

                for (int i = 0; i < count; i++) {
                    jumlah[i] = inventory.jumlah_barang.get_exact(match[i]->nama);
                }
                system("cls");
                tampil_barang(match, jumlah, count);
                subI = input_idx("  Pick item:\n[0]Exit, [1-" + to_string(count) + "]Barang\nPilihan: ", 0, count);
                if (subI == 0) continue;
                tampil_detail(match[subI - 1]);
                press_enter();
            }   
        }

        else if (pilihan == 4) {
            subI = input_idx("  Pick item:\n[0]Exit, [1-" + to_string(size) + "]Barang\nPilihan: ", 0, size);
            if (subI == 0) continue;
            // int amount = input_idx(" Amount: ", 1, 500);
            Gudang* sublist[100];
            subI--; //indexing 1 -> 0;
            cout << "\n\nPilihan anda: " << list_barang[subI]->nama 
                << "  Total stok: " << inventory.jumlah_barang.get_exact(list_barang[subI]->nama) 
                << "\n\n";
            int size = get_gudang(list_barang[subI], inventory.lokasi_inv, sublist);
            if (size == 0) {
                cout << "Tidak ada gudang dengan item " << list_barang[subI]->nama 
                << "\n\n";
                press_enter();
                continue;
            }
            tampil_gudang(sublist, size, inventory.lokasi_inv, list_barang[subI], true);
            int gudang = input_idx(" Pilih Gudang:\n[0]Exit, [1-" + to_string(size) + "]Gudang\nPilihan: ", 0, size);
            if (gudang == 0) continue;
            gudang--; //indexing 1 -> 0;
            int amount = input_idx(" Pick amount:\n[0]Exit, [N]Amount\nPilihan: ", 0, 
                sublist[gudang]->barang_gudang.get_exact(list_barang[subI]->nama));
            if (amount == 0) continue;
            inventory.enqueue_demand(list_barang[subI], sublist[gudang], amount, "withdraw");
            sublist[gudang]->update_barang_gudang(list_barang[subI], -amount);
            // inventory.update_barang_global(list_barang[subI], -amount); after withdraw if dequeued, otw to inventory
            list_jumlah[subI] -= amount;
            save_inventory();
            save_gudang();
            save_queued();
            press_enter();
        }

        else if (pilihan == 5) {
            subS = input_name("Register an item: ");
            if (inventory.daftar_barang.is_exist(subS)) {cout << "Item already existed"; press_enter(); continue;}
            Barang* newB = new Barang(trim(subS));
            inventory.add_barang_to_inv(newB);
            save_inventory();
            size = inventory.daftar_barang.get_all(list_barang); //reset list
            for (int i = 0; i < size; i++) { 
                list_jumlah[i] = inventory.jumlah_barang.get_exact(list_barang[i]->nama);
            }
            cout << "Successfully registered " << newB->nama;
            inventory.push_history(newB->nama+" has been Registered to Inventory");
            press_enter();
        }

        else if (pilihan == 6) {
            for (int i = 0; i < jumlah_lokasi; i++) {
                cout << i+1 << ". " << nama_semua_lokasi[i] << endl;
            }
            cout << "\n\n";
            subI = input_idx(" Change location:\n[0]Exit, [1-"+to_string(jumlah_lokasi)+"]Location\nPilihan: ", 0, jumlah_lokasi);
            if (subI == 0) continue;
            inventory.lokasi_inv = jalur_ke_semua_lokasi.get_exact(nama_semua_lokasi[subI-1]);
            save_inventory();
            cout << "\n\n Location saved, now changed to " << nama_semua_lokasi[subI-1];
            press_enter();
        }
        
    }
}

void menu_gudang_management() {
    Gudang* list_gudang[100];
    int jumlah_list[100];
    Barang* barang_list[100];
    string keys_barang[100];
    int size = inventory.list_gudang.get_all(list_gudang);
    int size_list;
    
    while (true) {
        system("cls");
        cout << "===Gudang Management===\n";
        for (int i = 0; i < size; i++) {
            cout << i + 1 << ". " << list_gudang[i]->nama 
            << ", " << list_gudang[i]->lokasi_gudang->lokasi_awal
            << "\n";
        }
        cout << "\n\n";
        int pilihan = input_idx("  Gudang:\n[0]Exit, [1]View Gudang, [2]Transfer item, [3]Buy item, [4]Add item, [5]Register Gudang\nPilihan: ", 0, 4);
        int gudang_idx;
        cout << "\n\n";
        if (pilihan == 0) break;

        if (pilihan != 5) {
            gudang_idx = input_idx("\n  Pilih Gudang:\n[0]Exit, [1-"+to_string(size)+"]Gudang\nPilihan: ", 0, size);
            if (gudang_idx == 0) continue;
            gudang_idx--;
            system("cls");
            cout << "=== " << list_gudang[gudang_idx]->nama << " ===\n"
            << "Lokasi: " << list_gudang[gudang_idx]->lokasi_gudang->lokasi_awal 
            << "\n\n";
            
            size_list = list_gudang[gudang_idx]->barang_gudang.get_all(jumlah_list);
            list_gudang[gudang_idx]->barang_gudang.get_all_keys(keys_barang);

            for (int i = 0; i < size_list; i++) {
                barang_list[i] = inventory.daftar_barang.get_exact(keys_barang[i]);
            }
            sort_list_amount(barang_list, jumlah_list, size_list);
            tampil_barang(barang_list, jumlah_list, size_list);
        }
        cout << "\n\n";

        if (pilihan == 1) press_enter();

        else if (pilihan == 2) {
            Gudang* listG [100];
            int item = input_idx("  Pick Barang:\n[0]Exit, [1-"+to_string(size_list)+"]Barang\nPilihan: ", 0, size_list);
            if (item == 0) continue;
            item--;
            int sizeT = get_gudang(barang_list[item], list_gudang[gudang_idx]->lokasi_gudang, listG, false);
            sort_closest_gudang(listG, sizeT, list_gudang[gudang_idx]->lokasi_gudang);
            
            // system("cls");
            tampil_gudang(listG, sizeT, list_gudang[gudang_idx]->lokasi_gudang, barang_list[item], true);
            int gudang = input_idx("  Take from:\n[0]Exit, [1-"+to_string(sizeT)+"]Gudang\nPilihan: ", 0, sizeT);
            if (gudang == 0) continue;
            gudang--;
            int amount = input_idx("  Amount:\n[0]Exit, [N]Amount\nPilihan: ", 0, listG[gudang]->barang_gudang.get_exact(barang_list[item]->nama));
            if (amount == 0) continue;
            listG[gudang]->update_barang_gudang(barang_list[item], -amount);

            inventory.push_history(listG[gudang]->nama+" Transfered "+to_string(amount)+" "+barang_list[item]->nama+" to "+list_gudang[gudang_idx]->nama);
            inventory.enqueue_demand(barang_list[item], list_gudang[gudang_idx], amount, "transferee");
            save_gudang();
            save_riwayat();
            save_queued();
            press_enter();
        }

        else if (pilihan == 3) {
            int item = input_idx("  Pick Barang:\n[0]Exit, [1-"+to_string(size_list)+"]Barang\nPilihan: ", 0, size_list);
            if (item == 0) continue;
            item--;
            int amount = input_idx("  Amount:\n[0]Exit, [N]Amount\nPilihan: ", 0, INF);
            if (amount == 0) continue;
            list_gudang[gudang_idx]->update_barang_gudang(barang_list[item], amount);
            inventory.update_barang_global(barang_list[item], amount);
            inventory.push_history(list_gudang[gudang_idx]->nama+" Bought "+to_string(amount)+" "+barang_list[item]->nama);
            save_gudang();
            save_inventory();
            save_riwayat();
            press_enter();
        }

        else if (pilihan == 4) {
            cout << "item not added yet: \n";
            Barang* listB[100];
            int size = inventory.daftar_barang.get_all(listB);
            int count = 0;
            for (int i = 0; i < size; i++) {
                if (list_gudang[gudang_idx]->barang_gudang.is_exist(listB[i]->nama)) continue;
                cout << count++ + 1 << ". " << listB[i]->nama;
                
                if (i+1 >= 10 && i+1 < 100) cout << " ";
                else if (i+1 < 10) cout << "  ";
                for (int j = listB[i]->nama.size(); j < 25; j++) (j % 2 == 0) ? cout << "-" : cout << " ";

                cout << "Jumlah stok: " << inventory.jumlah_barang.get_exact(listB[i]->nama) << endl;
            }
            int add = input_idx("  Add Barang:\n[0]Exit, [1-"+to_string(count)+"]Barang\nPilihan: ", 0, count);
            if (add == 0) continue;
            add--;
            link_gudang_to_barang(list_gudang[gudang_idx], listB[add], 0);
            inventory.push_history(list_gudang[gudang_idx]->nama+" Registered item: "+listB[add]->nama);
            save_gudang();
            save_riwayat();
            press_enter();
        }

        else if (pilihan == 5) {
            system("cls");
            cout << "=== Registrasi Gudang ===\n";
            string g_nama = input_name("Nama gudang: ");
            if (inventory.list_gudang.is_exist(g_nama)) {
                cout << g_nama << " Already existed!\n";
                press_enter();
                continue;
            }
            cout << "Lokasi: \n";
            for (int i = 0; i < jumlah_lokasi; i++) {
                cout << "  " << i+1 << ".";
                (i+1 < 10) ? cout << "  " : cout << " ";
                cout << nama_semua_lokasi[i] << endl;
            }
            int g_lokasi = input_idx("  Pick Lokasi:\n[0]Exit, [1-"+to_string(jumlah_lokasi)+"]Lokasi\nPilihan: ", 0, jumlah_lokasi);
            if (g_lokasi == 0) continue;
            g_lokasi--;
            Jalur* lokasi = jalur_ke_semua_lokasi.get_exact(nama_semua_lokasi[g_lokasi]);
            Gudang* newG = new Gudang(g_nama, lokasi);
            inventory.add_gudang_to_inventory(newG);
            inventory.push_history(newG->nama+" Has been registered");
            save_gudang();
            save_riwayat();
            press_enter();
        }
    }
}

void menu_queue_demand() {
    while (true) {
        if (!inventory.queue_head) {
            cout << "No demands yet..!\n";
            press_enter();
            return;
        }
        cout << "Demands: \n";
        Node<QueuedBarang*>* cur = inventory.queue_head;
        while (cur) {
            queue_messages(cur->data);
            cout << "\n\n";
            cur = cur->next;
        }
        cout << "\n\n";

        int pilihan = input_idx("  Dequeue:\n[0]Exit, [1]Withdrawal, [2]Transfered\nPilihan: ", 0, 2);
        if (pilihan == 0) break;

        else if (pilihan == 1) dequeue_type(inventory.queue_head, "withdraw");

        else if (pilihan == 2) dequeue_type(inventory.queue_head, "transferee");

        press_enter();
        system("cls");      
    }

}

void menu_riwayat() {
    while (true) {
        Node<string>* cur = inventory.riwayat_head;
        while (cur) {
            cout << cur->data << "\n\n";
            cur = cur->next;
        }

        int pilihan = input_idx("  Riwayat:\n[0]Exit, [1]Delete recent history\nPilihan: ", 0, 1);
        if (pilihan == 0) break;

        else if (pilihan == 1) {
            inventory.pop_history();
            cout << "\n\nRemoved recent history..";
            save_riwayat();
            press_enter();
        }
        system("cls");
    }

}


#endif