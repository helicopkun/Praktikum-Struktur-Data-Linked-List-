#ifndef DISPLAY_H
#define DISPLAY_H

#include "Files_List.h"
#include <iostream>

bool validPilihan;
string namaInput;

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
        NewMontirFile(nama);
        cout << nama << " telah ditambahkan ke daftar montir" << endl << endl;
    } else {
        cout << nama << " sudah terdaftar sebagai montir" << endl << endl;
    }
}

void TampilMechanic() {
    Montir* cur = headMontir;

    while (cur != NULL) {
        cout << cur->index << ". " << cur->namaMontir << endl;
        cur = cur->next;
    }
}

int PilihMontir(string Title) {
    int index;
    system("cls");
    cout << "====== " << Title << " ======\n" <<
    "Pilih Montir!\n\n";
    TampilMechanic();                      
    cout << "\nPilihan: ";
    while (!(cin >> index)) { // Selama input BUKAN angka
        system("cls");
        cout << "====== " << Title << " ======\n" <<
        "Pilih Montir!\n\n";
        TampilMechanic();
        cout << "\nPilihan: ";
        cin.clear(); // 1. Bersihkan status error cin
        cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
    }
    cin.ignore(1000, '\n');
    cout << endl;
    validPilihan = true;
    return index;
}
//p.s sort by date before checking
string CheckDate(string new_date, Service* Servis = NULL, int count = 0){ // increment by 1 if not valid (check for entire due list)
    if (count >= 5) {                                                        // returns new date
        cout << "Tanggal sudah diundur sebanyak 5 kali, masukkan tanggal baru : ";
        cin >> new_date;
        cin.ignore();
        return CheckDate(new_date, Servis);
    }
                                      
    int hari_baru, bulan_baru, tahun_baru;
    sscanf(new_date.c_str(), "%d-%d-%d", &hari_baru, &bulan_baru, &tahun_baru);
    bool valid = true;
    int hari, bulan, tahun;
    
    if (Servis) {
        sscanf(Servis->tanggal.c_str(), "%d-%d-%d", &hari, &bulan, &tahun);
        
        if (tahun_baru < tahun) valid = false;
        else if (tahun_baru == tahun){

            if (bulan_baru < bulan) valid = false;
            else if (bulan_baru == bulan){

                if (hari_baru < hari) valid = false;
            }
        }
        if (!valid){
            hari_baru = hari + 1;
            if (hari_baru > 30){
                hari_baru = 1;
                bulan_baru = bulan + 1;
                if (bulan_baru > 12) {
                    tahun_baru = tahun + 1;
                }
            } else {
                bulan_baru = bulan;
                tahun_baru = tahun;
            }
            cout << "\nTidak bisa sebelum jadwal awal " << Servis->dataCustomer->nama << ".\n";
        }
    }
    Service* cur = headHistoryDue;
    while(cur != NULL){ // check all current 
        sscanf(cur->tanggal.c_str(), "%d-%d-%d", &hari, &bulan, &tahun);
        if (hari == hari_baru && bulan == bulan_baru && tahun == tahun_baru) {
            if (count == 0) cout << "Tanggal sudah dijadwalkan oleh servis lain.\n";
            valid = false;
            break;
        }
        cur = cur->allnext;
    }

    if (valid) {  //base case
        if (count > 0) cout << "\nTanggal ambil yang baru adalah " << new_date << endl << endl;
        return new_date;
    }
    else { 
        hari_baru++;
        if (hari_baru > 30) {
            bulan_baru++;
            hari_baru = 1;
            if (bulan_baru > 12) {
                tahun_baru++;
            }
        }
        char buffer [20];
        sprintf(buffer, "%02d-%02d-%02d", hari_baru, bulan_baru, tahun_baru);
        new_date = buffer;
        count++;
        cout << "\nTanggal diundur " << count << " hari setelah tanggal baru...";
        return CheckDate(new_date, Servis, count);
    }
}

bool NewCustomer(bool AskName) { //tambah di belakang list - queue
    if (AskName) {
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
    AddCustomerFile(baruCustomer);

    return true;
}

void NewService(Customer* Pelanggan = NULL) {
    namaInput = "";
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
    SortByQueue();
    Service* curService = headHistoryDue;
    if (curService != NULL) { //cek mobil sama dari semua semua servis (menu admin)
        antrian = 1;
        while (curService != NULL && !sudahAda) {
            if (model == curService->model_mobil && merek == curService->merek_mobil) { //cek semua mobil, kalau ada yang sama
                
                if (!Pelanggan) { //input nama misal bukan dari menu customer (update namaInput)
                    askName = false;
                    cout << "Nama Pelanggan: ";
                    getline(cin, namaInput);
                } else namaInput = Pelanggan->nama;

                while (curService != NULL) { //cek lagi (lanjut khusus ketika tau namaInput)
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

    ReadAllMontir();
    cout << "Pilih Montir: " << endl;
    TampilMechanic();
    cout << "Pilihan: ";
    cin >> index;
    cin.ignore(); // ts is the reason i wasted 2 hours of my life
    baruService->nama_montir = FindMechanicName(index);

    cout << "Kepentingan: ";
    getline(cin, baruService->kepentingan);
    
    string tanggal;
    cout << "Tanggal Ambil: ";
    getline(cin, tanggal);
    SortByDate();

    if (Pelanggan)                                                            //gatau penting atau enggak
        baruService->tanggal = CheckDate(tanggal, Pelanggan->HeadServiceDue); //customer tidak bisa menambah lebih awal dari tanggal paling awal
    else
        baruService->tanggal = CheckDate(tanggal); // admin bisa menambah sebelum tanggal awal

    SortByQueue();
    if (Pelanggan) { //kalo dari "menu booking customer" tidak perlu cek / daftar ulang
        AddDueFile(baruService, Pelanggan);
        cout << "Servis sudah tercatat, nomor antrian anda adalah: " << antrian << "\n\n\n";
    
    } else {
        if (NewCustomer(askName)) { //customer baru :D  (dari menu admin servis)
            AddDueFile(baruService, tailCustomer);
            cout << "\n\nPelanggan baru dan servis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << "\n\n\n";
        
        } else { //customer lama
            Customer* cur = headCustomer; //cur = cursor customer
            while(cur != NULL) {
                if (cur->nama == namaInput) break;
                cur = cur->next;
            } //save per customer - di akhir list (queue)
            
            AddDueFile(baruService, cur);
            cout << "\n\nServis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << "\n\n\n";
        }
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
        "Montir: " << bantu->nama_montir << endl;
        if (names) { 
            cout << endl <<
            "Nama Pelanggan: " << bantu->dataCustomer->nama << endl <<
            //"No Telp Pelanggan: " << bantu->dataCustomer->nomor_telepon << endl;
            "Kepentingan: " << bantu->kepentingan << endl;
        }
        cout << "Tanggal Ambil: " << bantu->tanggal << endl;

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

void Antrian(Customer* Pelanggan = NULL) {
    cout << "====== All Services ======\n";
        if (headHistoryDue == NULL) {
            cout << "Empti :C\n\n";
            return;
        }
        DisplayServices(headHistoryDue, "All", false);
        cout << endl;

        if (Pelanggan) {
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

void AntrianDisplay(Customer* Pelanggan = NULL){ //full display
    char sort;
    SortByQueue();
    Antrian(Pelanggan);
    cout << "[D]ate [U]rgency [Q]ueue [E]xit" << endl <<
            "Sort by:";
    while (cin >> sort){
        sort = (char)toupper(sort);
        if (sort == 'E') break;
        system("cls");
        if (sort == 'Q') SortByQueue();
        else if (sort == 'D') SortByDate();
        else if (sort == 'U') SortByUrgency();
        Antrian(Pelanggan);
        cout << "[D]ate [U]rgency [Q]ueue [E]xit" << endl <<
                "Sort by:";
    }
    
}

void RescheduleService(Service* Servis){
    string new_date;
    cout << "Masukkan Tanggal baru: ";
    getline(cin, new_date);
    new_date = CheckDate(new_date, Servis); // parameter Servis for blocking earlier dates
    string nama = Servis->dataCustomer->nama;
    string model = Servis->model_mobil;
    string merek = Servis->merek_mobil;

    SortByQueue();

    Service* cur = headHistoryDue;
    while (cur != NULL) {
        if (cur->dataCustomer->nama == nama &&
            cur->model_mobil == model &&
            cur->merek_mobil == merek) {
            cur->tanggal = new_date;
            break;
        }
        cur = cur->allnext;
    }
    RewriteDue();
}

void DisplayReschedule(){
    bool valid;
    int index, pilihan;
    Service* cur = headHistoryDue;
    if (cur == NULL) {
        cout << "No services :c\n\n";
        return;
    }
    SortByDate();

    ResetMontir();
    UpdateMontir(headHistoryDue);

    string nama_montir = FindMechanicName(PilihMontir("Ganti Tanggal Ambil"));

    do {
        index = 0;
        system("cls");
        cout << "====== Ganti Tanggal Ambil ======\n";
        cur = headHistoryDue;

        while (cur != NULL) {
            if (cur->nama_montir == nama_montir) {
                cout << ++index << ". Servis ke-" << index  << ":" << endl;
                DisplayServices(cur, "All", false, false, false);
            }
            cur = cur->allnext;
        }
        cout << "0. Keluar" << endl << endl;
        cout << "Pilih Mobil yang ingin diundurkan: ";

        while (!(cin >> pilihan)) { //false input, ulang
            index = 0;
            system("cls");
            cout << "====== Ganti Tanggal Ambil ======\n";
            cur = headHistoryDue;

            while (cur != NULL) {
                if (cur->nama_montir == nama_montir) {
                    cout << ++index << ". Servis ke-" << index << ":" << endl;
                    DisplayServices(cur, "All", false, false, false);
                }
                cur = cur->allnext;
            }
            cout << "0. Keluar" << endl << endl;
            cout << "Pilih Mobil yang ingin diundurkan: ";
            cin.clear(); // 1. Bersihkan status error cin
            cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
        }
        cin.ignore(1000, '\n');
        if (pilihan == 0) return;
        else if (pilihan < 0 || pilihan > index) valid = false; //false pilihan, ulang
        else valid = true;
    } while (!valid);

    cur = headHistoryDue; //reset posisi cur
    
    int i = 0;
    while (i < pilihan){
        if (cur->nama_montir == nama_montir) i++;
        if (i >= pilihan) break;
        cur = cur->allnext;
    }

    string model = cur->model_mobil;
    RescheduleService(cur);
    cout << "Servis " << model << " berhasil di undur!!" << endl;

}

void CancelService(Customer* Pelanggan) {
    bool valid;
    int index, pilihan;
    Service* cur = Pelanggan->HeadServiceDue;
    if (cur == NULL) {
        cout << "No services :c\n\n";
        return;
    }
    //SortByQueue(); //queue
    SortByDate();

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

    RemoveDue(cur);

    Service* newUndo = cur;
    if (headUndo == NULL) { //new list (Undo)
        headUndo = newUndo;
    } else { //LIFO Undo
        newUndo->next = headUndo;
        headUndo->prev = newUndo;
        headUndo = newUndo;
    }
    
    cout << "\nServis " << cur->model_mobil << " telah dibatalkan" << endl << endl;
}

void UndoCancelService(Customer* Pelanggan) {
    string pilihan, tanggal;
    cout << "====== Booking Kembali Service ======" << endl << endl;
    if (headUndo == NULL) {
        cout << "No mistakes were made yet :D\n\n";
        return;
    }

    DisplayServices(headUndo, "Customer", false, false, false);

    do {
        cout << "Apakah anda ingin membooking kembali servis ini? (yes/no): ";
        cin >> pilihan;
    } while (pilihan != "yes" && pilihan != "no");

    if (pilihan == "no") return;

    cout << "Apakah ingin di reschedule? input (-) jika tidak" << endl 
        << "Tanggal Ambil Lama: " << headUndo->tanggal << endl 
        << "Tanggal Ambil Baru: ";
    cin >> tanggal;
    cin.ignore();

    if (tanggal != "-") headUndo->tanggal = tanggal;
    
    AddDueFile(headUndo, Pelanggan);

    cout << "\n\nServis " << headUndo->model_mobil << " telah dibooking kembali" << endl << endl;

    Service* temp = headUndo; // hapus node lama
    headUndo = headUndo->next;
    if (headUndo != NULL) headUndo->prev = NULL;
    delete temp;

}

void RiwayatCustomer(Customer* Pelanggan) { //riwayat servis anda
    cout << "====== Riwayat Servis Anda ======" << endl << endl;

    cout << "====== Services =======" << endl;
    if (Pelanggan->TailServiceDone == NULL && Pelanggan->HeadServiceDone == NULL)
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
    Service* bantu = cur->kerjaTerpentingDue;
    DisplayServices(bantu, "All", false, false);
    cout << endl << "Apakah servis ini sudah selesai? (yes/no) : ";
    cin >> pilihan;
    if (pilihan == "yes") {
        AddDoneFile(bantu);

        string nama = bantu->dataCustomer->nama;
        string model = bantu->model_mobil;
        string merek = bantu->merek_mobil;

        SortByQueue();

        Service* cur = headHistoryDue;
        while (cur != NULL) {
            if (cur->dataCustomer->nama == nama &&
                cur->model_mobil == model &&
                cur->merek_mobil == merek) {
                RemoveDue(cur);
                break;
            }
            cur = cur->allnext;
        }
    }
    else if (pilihan == "no") return;
    else {
        validPilihan = false;
        return;
    }
}

void DisplayFinishJob() {
    if (headHistoryDue == NULL) {
        cout << "====== Jobs Due ======\n" 
                << "Empti :c\n\n";
        return;
    }
    SortByUrgency();

    ResetMontir();
    UpdateMontir(headHistoryDue, true);

    int index;
    do {
        index = PilihMontir("Finish Job");
        MechanicFinishJob(FindMechanicName(index));
    } while(!validPilihan);
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

void DisplayJobsHistory() {
    if (headHistoryDue == NULL && headHistoryDone == NULL) {
        cout << "====== Jobs Done ======\n" 
                << "Empti :c\n\n";
        return;
    }
    SortByDate();

    ResetMontir();
    UpdateMontir(tailHistoryDone, false, true); //dari terlama (Done = Stack, jadi tail = terlama)
    UpdateMontir(headHistoryDue, false, false); //dari terlama (Due = Queue, jadi head =  terlama)
                                                //dari terlama (Done lebih dahulu ada sebelum Due, harusnya)
    int index;
    do {
        index = PilihMontir("Riwayat Kerja Montir");
        MechanicJobsHistory(FindMechanicName(index));
    } while(!validPilihan);
} 

#endif