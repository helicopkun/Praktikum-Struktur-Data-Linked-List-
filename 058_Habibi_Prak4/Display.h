#ifndef DISPLAY_H
#define DISPLAY_H

#include "LinkedList.h"

bool validPilihan;
string namaInput, noTelp;

bool NewCustomer(bool askName = true) { //tambah di belakang list - queue 
    if (askName) {
        cout << "Nama Pelanggan: ";
        getline(cin, namaInput);
    }

    Customer* cur = headCustomer; //cur = cursor customer
    while(cur != NULL) {
        if (cur->nama == namaInput) break;
        cur = cur->next;
    }
    if (cur != NULL) return false; //kalo ada, bukan customer baru
    
    Customer *baruCustomer = new Customer;
    
    cout << "No Telp Pelanggan: ";
    getline(cin, baruCustomer->nomor_telepon);
    cout << "Alamat: ";
    getline(cin, baruCustomer->alamat);
    cout << "Umur: ";
    getline(cin, baruCustomer->umur);
    cout << "Gender: ";
    getline(cin, baruCustomer->gender);
    
    baruCustomer->nama = namaInput;
    AddCustomer(baruCustomer);

    return true;
}

void NewService(bool customer, Customer* Pelanggan = NULL) { //customer = dari customer atau tidak
    int index, antrian;
    string model, merek, kendala;

    cout << "====== New Service ======" << endl;
    cout << "Model Mobil: ";
    getline(cin, model);
    cout << "Merek Mobil: ";
    getline(cin, merek);
    cout << "Kendala: ";
    getline(cin, kendala);

    //cek data servis due, kalo SAMA update
    bool sudahAda = false, askName = true;
    Service* curService = headHistoryDue;
    if (curService != NULL) { //cek mobil sama dari semua semua servis (menu admin)
        antrian = 1;
        while (curService != NULL && !sudahAda) {
            if (model == curService->model_mobil && merek == curService->merek_mobil) { //cek semua mobil, kalau ada yang sama
                
                if (!Pelanggan) { //input nama misal bukan dari menu customer (update namaInput)
                    askName = false;
                    cout << "Nama Pelanggan: ";
                    getline(cin, namaInput);
                }
                while (curService != NULL) { //cek lagi 
                    if (namaInput == curService->dataCustomer->nama && 
                        model == curService->model_mobil && 
                        merek == curService->merek_mobil) {
                            sudahAda = true;
                            break;   
                    }
                    curService = curService->allnext; // geser mencari semua data yang sama
                    if (curService != NULL) antrian++; //antrian++
                }
                if (curService == NULL || sudahAda) break; //lanjut seperti normal
            }
            curService = curService->allnext; //geser mencari data dengan mobil yang sama
            if (curService != NULL) antrian++; //antrian++
        }
        if (sudahAda) { //sudah ada mobil yg sama dengan customer yang sama
            string pilihan;
            do {
                system("cls");
                    cout << "====== New Service ======" << endl
                        << "Model Mobil: " << model << endl
                        << "Merek Mobil: " << merek << endl
                        << "Kendala: " << kendala << endl;
                    if (!Pelanggan) cout << "Nama Pelanggan: " << namaInput << endl;
                cout << "\nMobil ini sudah dibengkel anda ingin menambahkan kendala ini kepada servis" << endl <<
                        "tersebut (yes/no): ";
                cin >> pilihan;
                cin.ignore();
            } while (pilihan != "yes" && pilihan != "no");

            if (pilihan == "no") return;
            if (pilihan == "yes") {
                curService->deskripsi_kendala = curService->deskripsi_kendala + " + " + kendala;
                RewriteDue(); 
                if (Pelanggan) cout << "\nServis sudah tercatat, nomor antrian anda adalah: " << antrian << endl << endl;
                else cout << "\nServis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << endl << endl;
                return; //tidak perlu buat node baru, pilih montir, bla-bla-bla
            }
        }
    } 
    
    antrian++; //karena berada di last of list (curService = NULL), tambah karena membuat antrian baru
    Service *baruService = new Service;
    baruService->model_mobil = model;
    baruService->merek_mobil = merek;
    baruService->deskripsi_kendala = kendala;
    cout << "Tanggal Masuk: ";
    getline(cin, baruService->tanggal);

    ReadAllMontir();

    cout << "Pilih Montir: " << endl;
    TampilMechanic();
    cout << "Pilihan: ";
    cin >> index;
    cin.ignore(); // ts is the reason i wasted 2 hours of my life

    baruService->nama_montir = FindMechanicName(index);
    
    if (headHistoryDue == NULL) headHistoryDue = tailHistoryDue = baruService; //first in history
    else {
        tailHistoryDue->allnext = baruService; //save ke history - di akhir list (queue)
        baruService->allprev = tailHistoryDue;
        tailHistoryDue = baruService;
    }

    if (Pelanggan) { //kalo dari "menu booking customer" tidak perlu cek / daftar ulang
        AddService(Pelanggan, baruService);
        cout << "Servis sudah tercatat, nomor antrian anda adalah: " << antrian << "\n\n\n";
    
    } else {
        if (NewCustomer(askName)) { //customer baru :D  (dari menu admin servis)
            AddService(tailCustomer, baruService);
            cout << "\n\nPelanggan baru dan servis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << "\n\n\n";
        
        } else { //customer lama
            Customer* cur = headCustomer; //cur = cursor customer
            while(cur != NULL) {
                if (cur->nama == namaInput) break;
                cur = cur->next;
            } //save per customer - di akhir list (queue)
            
            AddService(cur, baruService);
            cout << "\n\nServis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << "\n\n\n";
        }
    }
}

void NewMontir() {
    string nama;
    ReadAllMontir();
    cout << "====== New Mechanic ======" << endl
        << "Masukkan Nama montir baru" << endl
        << "Nama: ";
    cin >> nama;
    cin.ignore();
    cout << endl;
    
    if (NewMontirToList(nama)) {
        ofstream nama_montir ("../Nama_Montir.txt", ios::app);
        nama_montir << nama << endl;
        nama_montir.close();
        cout << nama << " telah ditambahkan ke daftar montir" << endl << endl;
    } else {
        cout << nama << " sudah terdaftar sebagai montir" << endl << endl;
    }
}

void DisplayServices(Service* Data, string Customer_or_All, bool Reverse, bool loop = true, bool names = true) { //Tampilkan semua servis berdasarkan data
    Service* bantu = Data;                                  // Data = akses linkedlist mulai dari?
    if (bantu == NULL) {                                    // Customer_or_All = self-explanatory
        return;                                             // Due = data Due(true), All(false)
    }                                                       // loop = tampil sekali(false), semua(true)                              
    do {                                                    // names = data private data customer tersebut
        cout << 
        "-----------------------\n"
        "Model Mobil: " << bantu->model_mobil << endl << 
        "Merek Mobil: " << bantu->merek_mobil << endl <<
        "Kendala: " << bantu->deskripsi_kendala << endl << 
        "Montir: " << bantu->nama_montir << endl <<
        "Tanggal masuk: " << bantu->tanggal << endl;
        if (names) { 
            cout <<
            "Nama Pelanggan: " << bantu->dataCustomer->nama << endl << 
            "No Telp Pelanggan: " << bantu->dataCustomer->nomor_telepon << endl;
        }

        cout <<
        "-----------------------\n\n";

        if (Customer_or_All == "Customer") {
            if (Reverse) bantu = bantu->prev; //karena data terbaru di akhir list
            else bantu = bantu->next;
        }
        if (Customer_or_All == "All") {
            if (Reverse) bantu = bantu->allprev;
            else bantu = bantu->allnext;
        };
    } while (bantu != NULL && loop);
}

void Antrian(bool customer, Customer* Pelanggan = NULL) {
    cout << "====== All Services ======\n";
        if (headHistoryDue == NULL) {
            cout << "Empti :C\n\n";
            return;
        }
        DisplayServices(headHistoryDue, "All", false);
        cout << endl;

        if (customer) {
            if (Pelanggan->HeadServiceDue == NULL) {
                cout << "Anda tidak memiliki antrian" << endl << endl;
                return;
            }

            int antrian = 1;
            Service* cur = headHistoryDue;

            while (cur != NULL) {
                if (cur->dataCustomer == Pelanggan) 
                    cout << "Servis " << cur->model_mobil << " Anda berada di antrian ke-" << antrian << endl << endl;
                cur = cur->allnext;
                antrian++;
            }
        }
}

void CancelService(Customer* Pelanggan) {
    bool valid;
    int index, pilihan;
    Service* cur = Pelanggan->HeadServiceDue;
    if (cur == NULL) {
        cout << "No services :c\n\n";
        return;
    }
    do {
        index = 1; //reset jika ga valid
        system("cls");
        cout << "====== Cancel Service ======\n";
        cur = Pelanggan->HeadServiceDue;
        
        while (cur != NULL) { //tampilan servis
            cout << index << ". Servis ke-" << index  << ":" << endl;
            DisplayServices(cur, "Customer", false, false, false);
            cur = cur->next;
            if (cur != NULL) index++; //kalau ga di ujung, index++
        }
        cout << "0. Keluar" << endl << endl;
        cout << "Pilih Mobil yang ingin dibatalkan: ";

        while (!(cin >> pilihan)) { //false input, ulang
            index = 1;
            system("cls");
            cout << "====== Cancel Service ======\n";
            cur = Pelanggan->HeadServiceDue;

            while (cur != NULL) {
                cout << index << ". Servis ke-" << index  << ":" << endl;
                DisplayServices(cur, "Customer", false, false, false);
                cur = cur->next;
                if (cur != NULL) index++; //kalau ga di ujung, index++
            }
            cout << "0. Keluar" << endl << endl;
            cout << "Pilih Mobil yang ingin dibatalkan: ";
            cin.clear(); // 1. Bersihkan status error cin
            cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
        }
        cin.ignore(1000, '\n');
        if (pilihan == 0) return;
        else if (pilihan < 0 || pilihan > index) valid = false; //false pilihan, ulang
        else valid = true;
    } while (!valid);
    
    cur = Pelanggan->HeadServiceDue; //reset posisi cur
    for (int i = 1; i < pilihan; i++) { //cari posisi dengan looping list sebanyak index pilihan - 1
        cur = cur->next;
    }   
    ReLinkDue(cur);
    RewriteDue();

    Service* newUndo = cur;
    if (headUndo == NULL) { //new list (Undo)
        headUndo = newUndo;
    } else { //LIFO Undo
        newUndo->next = headUndo;
        headUndo->prev = newUndo;
        headUndo = newUndo;
    }
    
    cout << "\nServis " << cur->model_mobil << " telah dibatalkan" << endl << endl
        << "Press enter to go back...";
    cin.get();
}

void UndoCancelService(Customer* Pelanggan) {
    string pilihan, tanggal;
    cout << "====== Booking Kembali Service ======" << endl << endl;
    if (headUndo == NULL) {
        cout << "No mistakes were made yet :D\n\n";
        return;
    }

    DisplayServices(headUndo, "Customer", false, false, false);

    cout << "Apakah anda ingin membooking kembali servis ini? (yes/no): ";
    cin >> pilihan;
    if (pilihan == "") return;
    else if (pilihan == "yenos") {
        cout << "Apakah ingin di reschedule? input (-) jika tidak" << endl 
            << "Tanggal Lama: " << headUndo->tanggal << endl 
            << "Tanggal Baru: ";
        cin >> tanggal;
        cin.ignore();
                // copy data
        Service* copyService = new Service;
        copyService->model_mobil = headUndo->model_mobil;
        copyService->merek_mobil = headUndo->merek_mobil;
        copyService->deskripsi_kendala = headUndo->deskripsi_kendala;
        copyService->nama_montir = headUndo->nama_montir;

        if (tanggal != "-") copyService->tanggal = tanggal;
        else copyService->tanggal = headUndo->tanggal;

        AddDueList(copyService, Pelanggan);
        RewriteDue();

        cout << "\n\nServis " << copyService->model_mobil << " telah dibooking kembali" << endl << endl;

        Service* temp = headUndo; // hapus node lama
        headUndo = headUndo->next;
        if (headUndo != NULL) headUndo->prev = NULL;
        delete temp;

    } else UndoCancelService(Pelanggan);
}

void RiwayatCustomer(Customer* Pelanggan) { //riwayat servis anda
    cout << "====== Riwayat Servis Anda ======" << endl << endl;

    cout << "====== Services =======" << endl;
    if (Pelanggan->TailServiceDue == NULL && Pelanggan->HeadServiceDone == NULL)
        cout << "Empti servis :C";
    else {
        DisplayServices(Pelanggan->HeadServiceDone, "Customer", false); //update hanya yg sudah
    }
    cout << endl << endl;
}

void MechanicFinishJob(string nama) {
    string pilihan;
    if (nama == "Tidak ada") {
        validPilihan = false;
        return;
    }
    Montir* cur = headMontir;
    while (cur != NULL) {
        if (cur->namaMontir == nama) break;
        cur = cur->next;
    }
    Service* bantu = cur->kerjaTerlamaDue;
    DisplayServices(bantu, "All", false, false);
    cout << endl << "Apakah servis ini sudah selesai? (yes/no) : ";
    cin >> pilihan;
    if (pilihan == "yes") {
        ofstream data_DoneService ("../data_DoneService.txt", ios::app); //terbaru di akhir data
        data_DoneService << 
        bantu->dataCustomer->nama << endl <<
        bantu->model_mobil << endl <<
        bantu->merek_mobil << endl <<
        bantu->deskripsi_kendala << endl <<
        bantu->nama_montir << endl <<
        bantu->tanggal << endl << endl;

        ReLinkDue(bantu);
        RewriteDue();

        AddDoneList(bantu, bantu->dataCustomer);
    } 
    else if (pilihan == "no") return;
    else {
        validPilihan = false;
        return;
    }
}

void MechanicJobsHistory(string nama) {
    if (nama == "Tidak ada") {
        validPilihan = false;
        return;
    }
    cout << "====== " << nama << "'s Jobs ======\n\n";
    Service* curDue = tailHistoryDue;
    Service* curDone = headHistoryDone;
    while (curDue != NULL) {
        if (curDue->nama_montir == nama) DisplayServices(curDue, "All", true, false);
        curDue = curDue->allprev;
    }
    while (curDone != NULL) {
        if (curDone->nama_montir == nama) DisplayServices(curDone, "All", false, false);
        curDone = curDone->allnext;
    }
}

#endif