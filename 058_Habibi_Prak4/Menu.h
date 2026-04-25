#ifndef MENU_H
#define MENU

#include "Display.h"

bool keluarAdmin = false, 
     keluar = false;

//------------------------------------------ Menus ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
void MenuServisAdmin() {
    string pilihan;
            cout << R"(====== Services ======
Pilih menu?
1. Semua Servis Singkat
2. Servis Baru
3. Selesaikan Pekerjaan
4. Riwayat Kerja Montir

Pilihan: )";
    cin >> pilihan;
    cin.ignore();
    system("cls");
    if (pilihan == "1") {
        cout << "====== All Services ======\n";
        if (headHistoryDue == NULL) {
            cout << "Empti :c\n\n";
            return;
        }
        DisplayServices(headHistoryDue, "All", false);
    }
    else if (pilihan == "2") {
        NewService(false);
    }
    else if (pilihan == "3") {
        if (headHistoryDue == NULL) {
            cout << "====== Jobs Done======1\n" 
                 << "Empti :c\n\n";
            return;
        }
        
        ResetMontir();
        UpdateMontir(headHistoryDue, true, false);

        int index;
        do {
            system("cls");
            cout << "====== Jobs Done======2\n" <<
            "Pilih Montir!\n\n";
            TampilMechanic();
            cout << "\nPilihan: ";
            while (!(cin >> index)) { // Selama input BUKAN angka
                system("cls");
                cout << "====== Jobs Done======3\n" <<
                "Pilih Montir!\n\n";
                TampilMechanic();
                cout << "\nPilihan: ";
                cin.clear(); // 1. Bersihkan status error cin
                cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
            }
            cin.ignore(1000, '\n');
            cout << endl;
            validPilihan = true;
            MechanicFinishJob(FindMechanicName(index));
        } while(!validPilihan);
    } 
    else if (pilihan == "4") {

        ResetMontir();
        UpdateMontir(tailHistoryDone, false, true); //dari terlama (Done = Stack, jadi tail = terlama)
        UpdateMontir(headHistoryDue, false, false); //dari terlama (Due = Queue, jadi head =  terlama)
                                                    //dari terlama (Done lebih dahulu ada sebelum Due, harusnya)
        int index;
        do {
            system("cls");
            cout << "====== Riwayat Kerja Montir ======\n" <<
            "Pilih Montir!\n\n";
            TampilMechanic();                      
            cout << "\nPilihan: ";
            while (!(cin >> index)) { // Selama input BUKAN angka
                system("cls");
                cout << "====== Riwayat Kerja Montir ======\n" <<
                "Pilih Montir!\n\n";
                TampilMechanic();
                cout << "\nPilihan: ";
                cin.clear(); // 1. Bersihkan status error cin
                cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
            }
            cin.ignore(1000, '\n');
            cout << endl;
            validPilihan = true;
            MechanicJobsHistory(FindMechanicName(index));
        } while(!validPilihan);
    } 
    else cout << "Pilihan tidak valid!\n\n";
}

void MenuAdmin() {
    system("cls");
    string pilihan;
    cout << R"(====== Welcome To Lognuts ======
Pilih Menu?

1. Servis
2. Pelanggan Baru
3. Montir Baru
4. Keluar

Pilihan : )";
    cin >> pilihan;
    cin.ignore();
    system("cls");
    if (pilihan == "1") MenuServisAdmin();
    else if (pilihan == "2") {
        if (!NewCustomer())
            cout << "\nPelanggan telah terdaftar\n\n";
    }
    else if (pilihan == "3") NewMontir();
    else if (pilihan == "4") {
        keluarAdmin = true;
        return;
    }
    else cout << "Pilihan tidak valid!\n\n";
    
    cout << "Press enter to go back...";
    cin.ignore();
}

void MenuCustomer(string namaCustomer) {
    Customer* curCustomer = headCustomer;
    namaInput = namaCustomer;
    while(curCustomer != NULL) { //cek customer yang mana
        if (curCustomer->nama == namaCustomer) break;
        curCustomer = curCustomer->next;
    }

    system("cls");
    string pilihan;
    cout << "====== Welcome " << namaCustomer << " ======";
    cout << R"(
Pilih menu!

1. Antrian Anda
2. Booking Servis
3. Batalkan Servis
4. Undo Pembatalan
5. Riwayat Servis
6. Keluar

Pilihan: )";
    cin >> pilihan;
    cin.ignore();
    cout << endl;
    
    if (pilihan == "1") Antrian(true, curCustomer);
    else if (pilihan == "2") NewService(true, curCustomer);
    else if (pilihan == "3") {
        CancelService(curCustomer);
        return;
    }
    else if (pilihan == "4") UndoCancelService(curCustomer);
    else if (pilihan == "5") RiwayatCustomer(curCustomer);
    else if (pilihan == "6") {
        while (headUndo != NULL) { //reset list Undo
        Service* temp = headUndo;
        headUndo = headUndo->next;
        delete temp;
    }
        keluar = true;
        return;
    }
    else cout << "Pilihan tidak valid!\n\n";

    cout << "Press enter to go back...";
    cin.get();
}



#endif