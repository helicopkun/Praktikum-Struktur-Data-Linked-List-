#include <iostream>
#include <fstream>
using namespace std;

struct Service;
struct Customer;

struct Customer {
    string nama, umur, gender, nomor_telepon, alamat;
    Service* HeadServiceDone = NULL; //last in - first out (data terakhir = riwayat pertama)
    Service* TailServiceDone = NULL;
    Service* HeadServiceDue = NULL; //first in - first out (data pertama = servis pertama montir)
    Service* TailServiceDue = NULL;
    Customer* next = NULL;
    Customer* prev = NULL;
}; //rupanya bisa deklarasi NULL dari awal bruh

struct Service {
    string model_mobil, merek_mobil, deskripsi_kendala, nama_montir, tanggal;
    Customer* dataCustomer = NULL;
    Service* next = NULL; //servis per customer
    Service* prev = NULL;
    Service* allnext = NULL; //semua servis
    Service* allprev = NULL; 
};

struct Montir {
    string namaMontir;
    int index;
    Montir* next = NULL;
    Service* kerjaTerbaru = NULL;
};

Customer* headCustomer = NULL; //first in - first out (data terakhir = customer terakhir)
Customer* tailCustomer = NULL;

Service *headHistoryDue = NULL; //first in - first out (data pertama = servis pertama montir)
Service *tailHistoryDue = NULL;

Service *headHistoryDone = NULL; //last in - first out (data terakhir = riwayat pertama)
Service *tailHistoryDone = NULL;

Montir* headMontirDue = NULL; //first in - first out
Montir* headMontirAll = NULL;
string namaInput;
bool validPilihan, 
     keluarAdmin, 
     keluar = false;
int jumlahMontir = 0;

bool NewCustomer() { //tambah di belakang list - queue
    Customer *baruCustomer = new Customer;
    
    cout << "Nama Pelanggan: ";
    getline(cin, namaInput);

    Customer* cur = headCustomer; //cur = cursor customer
    while(cur != NULL) {
        if (cur->nama == namaInput) break;
        cur = cur->next;
    }
    if (cur != NULL) return false; //kalo ada, bukan customer baru
    
    cout << "No Telp Pelanggan: ";
    getline(cin, baruCustomer->nomor_telepon);
    cout << "Alamat: ";
    getline(cin, baruCustomer->alamat);
    cout << "Umur: ";
    getline(cin, baruCustomer->umur);
    cout << "Gender: ";
    getline(cin, baruCustomer->gender);
    
    baruCustomer->nama = namaInput;

    if (headCustomer == NULL) {
        headCustomer = tailCustomer = baruCustomer;
    } else {
        baruCustomer->prev = tailCustomer;
        tailCustomer->next = baruCustomer;
        tailCustomer = baruCustomer;
    }
    ofstream data_customer2("data_customer2.txt", ios::app);
    data_customer2 << namaInput << endl 
    << baruCustomer->umur << endl 
    << baruCustomer->gender << endl 
    << baruCustomer->nomor_telepon << endl 
    << baruCustomer->alamat << endl << endl;
    data_customer2.close();
    return true;
}

void NewService() { //tambah di awal list - stack
    Service *baruService = new Service;

    cout << "Model Mobil: ";
    getline(cin, baruService->model_mobil);
    cout << "Merek Mobil: ";
    getline(cin, baruService->merek_mobil);
    cout << "Kendala: ";
    getline(cin, baruService->deskripsi_kendala);
    cout << "Montir: ";
    getline(cin, baruService->nama_montir);
    cout << "Tanggal Masuk: ";
    getline(cin, baruService->tanggal);

    if (NewCustomer()) { //customer baru :D
        tailCustomer->HeadServiceDue = tailCustomer->TailServiceDue = baruService;
        baruService->dataCustomer = tailCustomer;
        cout << "\n\nPelanggan baru telah terdaftar dan servis berhasil dicatat\n\n\n";
    } else { //customer lama
        Customer* cur = headCustomer; //cur = cursor customer
        while(cur != NULL) {
            if (cur->nama == namaInput) break;
            cur = cur->next;
        } //save per customer - di akhir list (queue)
        if (cur->TailServiceDue == NULL) { //cek ada servis atau tidak
            cur->HeadServiceDue = cur->TailServiceDue = baruService;
        } else {
            cur->TailServiceDue->next = baruService;
            baruService->prev = cur->TailServiceDue;
            cur->TailServiceDue = baruService;
        }
        baruService->dataCustomer = cur;

        cout << "\n\nPelanggan sudah terdaftar dan servis berhasil dicatat\n\n\n";
    }
    if (headHistoryDue == NULL) headHistoryDue = tailHistoryDue = baruService; //first in history
    else {
        tailHistoryDue->allnext = baruService; //save ke history - di akhir list (queue)
        baruService->allprev = tailHistoryDue;
        tailHistoryDue = baruService;
    }

    ofstream data_DueService2("data_DueService2.txt", ios::app);
    data_DueService2 << namaInput << endl 
    << baruService->model_mobil << endl 
    << baruService->merek_mobil << endl 
    << baruService->deskripsi_kendala << endl 
    << baruService->nama_montir << endl 
    << baruService->tanggal << endl << endl;
    data_DueService2.close();
}

void Services(Service* Data, string Customer_or_All, bool Reverse, bool loop) { //Tampilkan semua servis berdasarkan data
    Service* bantu = Data;                                  // Data = akses linkedlist mulai dari?
    if (bantu == NULL) {                                    // Customer_or_All = self-explanatory
        return;                                             // Due = data Due(true), All(false)
    }                                                       // loop = tampil sekali(false), semua(true)                              
    do {
        cout << 
        "Model Mobil: " << bantu->model_mobil << endl << 
        "Merek Mobil: " << bantu->merek_mobil << endl <<
        "Kendala: " << bantu->deskripsi_kendala << endl << 
        "Montir: " << bantu->nama_montir << endl <<
        "Tanggal masuk: " << bantu->tanggal << endl << 
        "Nama Pelanggan: " << bantu->dataCustomer->nama << endl << 
        "No Telp Pelanggan: " << bantu->dataCustomer->nomor_telepon << endl <<
        "-----------------------\n";

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

void ServisAnda() {
    string nama;
    cout << 
    "====== Riwayat Servis Anda ======" << endl <<
    "Masukkan Nama: ";
    getline(cin, nama);
    cout << endl;

    Customer* cur = headCustomer;
    while (cur != NULL) {
        if (cur->nama == nama) break;
        cur = cur->next;
    }
    if (cur == NULL || cur->nama != nama) {
        cout << "Tidak ada nama customer tersebut :p\n\n";
        return;
    }

    cout << "====== Services =======" << endl;
    if (cur->TailServiceDue == NULL && cur->HeadServiceDone == NULL)
        cout << "Empti servis :C";
    else {
        Services(cur->TailServiceDue, "Customer", true, true);
        Services(cur->HeadServiceDone, "Customer", false, true);
    }
    cout << endl << endl;
}

void UpdateMontir(Service* Data, bool Due_and_Singular, bool Reverse) { //counter montir (tergantung All or Due) + assign pekerjaan terlama yg harus dikerjakan terlebih dahulu + index tergantung list
    Service* bantuServis = Data;
    if (bantuServis == NULL) return;

    Montir *bantuMontir , *newMontir;
    while (bantuServis != NULL) {
        if (Due_and_Singular)
         bantuMontir = headMontirDue;
        else
         bantuMontir = headMontirAll;
        bool sudahAda = false;

        while (bantuMontir != NULL) {
            if (bantuMontir->namaMontir == bantuServis->nama_montir) {
                sudahAda = true;
                break;
            }
            bantuMontir = bantuMontir->next;
        }
        if (!sudahAda) {
            Montir* newMontir = new Montir; //new node
            newMontir->namaMontir = bantuServis->nama_montir;
            newMontir->index = ++jumlahMontir; // ++Index
            
            if (Due_and_Singular) newMontir->kerjaTerbaru = bantuServis;

            if (Due_and_Singular) {
                if (headMontirDue == NULL) { 
                    headMontirDue = newMontir; //awal list montir Due
                } else { 
                    Montir* tailM = headMontirDue;
                    while (tailM->next != NULL) tailM = tailM->next;
                    tailM->next = newMontir; //ujung list montir
                }
            } else {
                if (headMontirAll == NULL) {
                    headMontirAll = newMontir; // awal list montir All
                } else {
                    Montir* tail = headMontirAll;
                    while (tail->next != NULL) tail = tail->next;
                    tail->next = newMontir; //ujung list montir All
                }
            }
        }
        if (Reverse) bantuServis = bantuServis->allprev;
        else bantuServis = bantuServis->allnext;
    }
}

void TampilMechanic(bool Due) { //true = Due aja, false = semua
    Montir* cur;
    if (Due) cur = headMontirDue;
    else cur = headMontirAll;
    while (cur != NULL) {
        cout << cur->index << ". " << cur->namaMontir << endl;
        cur = cur->next;
    }
}

string FindMechanicName(int indexInput, bool Due) {
    Montir* cur;
    if (Due) cur = headMontirDue;
    else cur = headMontirAll;
    while (cur != NULL) {
        if (cur->index == indexInput) return cur->namaMontir;
        cur = cur->next;
    }
    return "Tidak ada";
}

void MechanicFinishJob(string nama) {
    string pilihan;
    if (nama == "Tidak ada") {
        validPilihan = false;
        return;
    }
    Montir* cur = headMontirDue;
    while (cur != NULL) {
        if (cur->namaMontir == nama) break;
        cur = cur->next;
    }
    Service* bantu = cur->kerjaTerbaru;
    Services(bantu, "All", false, false);
    cout << endl << "Apakah servis ini sudah selesai? (yes/no) : ";
    cin >> pilihan;
    if (pilihan == "yes") {
        ofstream data_DoneService ("data_DoneService2.txt", ios::app); //terbaru di akhir data
        data_DoneService << 
        bantu->dataCustomer->nama << endl <<
        bantu->model_mobil << endl <<
        bantu->merek_mobil << endl <<
        bantu->deskripsi_kendala << endl <<
        bantu->nama_montir << endl <<
        bantu->tanggal << endl << endl;

        if (bantu->allprev != NULL) bantu->allprev->allnext = bantu->allnext;//History
        else headHistoryDue = bantu->allnext;
        
        if (bantu->allnext != NULL) bantu->allnext->allprev = bantu->allprev;
        else tailHistoryDue = bantu->allprev;

        if (bantu->prev != NULL) bantu->prev->next = bantu->next;//Customer
        else bantu->dataCustomer->HeadServiceDue = bantu->next;

        if (bantu->next != NULL) bantu->next->prev = bantu->prev;
        else bantu->dataCustomer->TailServiceDue = bantu->prev;

        bantu->next = bantu->prev = bantu->allnext = bantu->allprev = NULL;//reset pointer

        if (headHistoryDone == NULL) { //new list (History)
            headHistoryDone = tailHistoryDone = bantu;
        } else { //LIFO
            bantu->allnext = headHistoryDone;
            headHistoryDone->allprev = bantu;
            headHistoryDone = bantu;
        
            Customer* curC = bantu->dataCustomer;
            if (curC->HeadServiceDone == NULL) { //new list (Customer)
                curC->HeadServiceDone = curC->TailServiceDone = bantu;
            } else { //LIFO
                bantu->next = curC->HeadServiceDone;
                curC->HeadServiceDone->prev = bantu;
                curC->HeadServiceDone = bantu;
            }
        }

        ofstream data_DueService("data_DueService2.txt", ios::trunc); //rewrite due
        Service* cur = headHistoryDue;
        while (cur != NULL) {
            data_DueService << cur->dataCustomer->nama << endl
                << cur->model_mobil << endl
                << cur->merek_mobil << endl
                << cur->deskripsi_kendala << endl
                << cur->nama_montir << endl
                << cur->tanggal << endl << endl;
            cur = cur->allnext; // Jalan ke servis berikutnya di antrean
        }
        data_DueService.close();

    } 
    else if (pilihan == "no") return;
    else {
        validPilihan = false;
        return;
    }
}

void MechanicJobs(string nama) {
    if (nama == "Tidak ada") {
        validPilihan = false;
        return;
    }
    cout << "====== " << nama << "'s Jobs ======\n\n";
    Service* curDue = tailHistoryDue;
    Service* curDone = headHistoryDone;
    while (curDue != NULL) {
        if (curDue->nama_montir == nama) Services(curDue, "All", true, false);
        curDue = curDue->allprev;
    }
    while (curDone != NULL) {
        if (curDone->nama_montir == nama) Services(curDone, "All", false, false);
        curDone = curDone->allnext;
    }
}

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
        Services(headHistoryDue, "All", false, true);
    }
    else if (pilihan == "2") {
        cout << "====== New Service ======\n";
        NewService();
    }
    else if (pilihan == "3") {
        if (headHistoryDue == NULL) {
            cout << "====== Jobs Done======\n" 
                 << "Empti :c\n\n";
            return;
        }
        int index;
        jumlahMontir = 0;

        while (headMontirDue != NULL) { //reset list
            Montir* temp = headMontirDue;
            headMontirDue = headMontirDue->next;
            delete temp;
        }
        UpdateMontir(headHistoryDue, true, false);
        do {
            cout << "====== Jobs Done======\n" <<
            "Pilih Montir!\n\n";
            TampilMechanic(true);
            cout << "\nPilihan: ";
            while (!(cin >> index)) { // Selama input BUKAN angka
                system("cls");
                cout << "====== Jobs Done======\n" <<
                "Pilih Montir!\n\n";
                TampilMechanic(true);
                cout << "\nPilihan: ";
                cin.clear(); // 1. Bersihkan status error cin
                cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
            }
            cin.ignore(1000, '\n');
            cout << endl;
            validPilihan = true;
            MechanicFinishJob(FindMechanicName(index, true));
        } while(!validPilihan);
    }
    else if (pilihan == "4") {
        int index;
        jumlahMontir = 0;
        
        while (headMontirAll != NULL) { //reset list
            Montir* temp = headMontirAll;
            headMontirAll = headMontirAll->next;
            delete temp;
        }
        UpdateMontir(tailHistoryDone, false, true); //dari terlama (Done = Stack, jadi tail = terlama)
        UpdateMontir(headHistoryDue, false, false); //dari terlama (Due = Queue, jadi head =  terlama)
                                                    //dari terlama (Done lebih dahulu ada sebelum Due, harusnya)
        do {
            system("cls");
            cout << "====== Riwayat Kerja Montir ======\n" <<
            "Pilih Montir!\n\n";
            TampilMechanic(false);                      
            cout << "\nPilihan: ";
            while (!(cin >> index)) { // Selama input BUKAN angka
                system("cls");
                cout << "====== Jobs Done======\n" <<
                "Pilih Montir!\n\n";
                TampilMechanic(false);
                cout << "\nPilihan: ";
                cin.clear(); // 1. Bersihkan status error cin
                cin.ignore(1000, '\n'); // 2. Buang karakter sampah di buffer (maks 1000 karakter)
            }
            cin.ignore(1000, '\n');
            cout << endl;
            validPilihan = true;
            MechanicJobs(FindMechanicName(index, false));
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
3. Keluar

Pilihan : )";
    cin >> pilihan;
    cin.ignore();
    system("cls");
    if (pilihan == "1") MenuServisAdmin();
    else if (pilihan == "2") {
        if (!NewCustomer())
            cout << "\nPelanggan telah terdaftar\n\n";
    }
    else if (pilihan == "3") {
        keluarAdmin = true;
        return;
    }
    else cout << "Pilihan tidak valid!\n\n";
    
    cout << "Press enter to go back...";
    cin.ignore();
}

void MenuCustomer(){
    system("cls");
    string pilihan;
    cout << R"(====== Welcome to Lognuts ======
Pilih menu!

1. Antrian Servis
2. Riwayat Servis Anda
3. Keluar

Pilihan: )";
    cin >> pilihan;
    cin.ignore();
    cout << endl;
    if (pilihan == "adminacces8008") {
        keluarAdmin = false;
        while (!keluarAdmin) MenuAdmin();
        return;
    }
    else if (pilihan == "1") {
        cout << "====== All Services ======\n";
        if (headHistoryDue == NULL) {
            cout << "Antrean kosong!\n";
            return;
        }
        Services(headHistoryDue, "All", false, true);
    }
    else if (pilihan == "2") ServisAnda();
    else if (pilihan == "3") {
        keluar = true;
        return;
    }
    else cout << "Pilihan tidak valid!\n\n";

    cout << "Press enter to go back...";
    cin.get();
}

int main() {
    ifstream data_customer2("data_customer2.txt"); // FIFO
    string namaC, umur, gender, nomor_teleponC, alamat, kosong1;
    while (getline(data_customer2, namaC) && namaC != "") {
        getline(data_customer2, umur);
        getline(data_customer2, gender);
        getline(data_customer2, nomor_teleponC);
        getline(data_customer2, alamat);
        getline(data_customer2, kosong1);
        
        Customer *baruCustomer = new Customer;

        baruCustomer->nama = namaC;
        baruCustomer->umur = umur;
        baruCustomer->gender = gender;
        baruCustomer->nomor_telepon = nomor_teleponC;
        baruCustomer->alamat = alamat;
        if (headCustomer == NULL) {
            headCustomer = tailCustomer = baruCustomer;
        } else {
            baruCustomer->prev = tailCustomer;
            tailCustomer->next = baruCustomer;
            tailCustomer = baruCustomer;
        }
    }
    data_customer2.close();

    //link data servis -> data customer
    ifstream data_DueService2("data_DueService2.txt"); //FIFO - queue
    string namaSdue, model_mobil_due, merek_mobil_due, deskripsi_kendala_due, nama_montir_due, tanggaldue, kosong3;
    while (getline(data_DueService2, namaSdue) && namaSdue != "") {  
        getline(data_DueService2, model_mobil_due);  
        getline(data_DueService2, merek_mobil_due);
        getline(data_DueService2, deskripsi_kendala_due);
        getline(data_DueService2, nama_montir_due);
        getline(data_DueService2, tanggaldue);
        getline(data_DueService2, kosong3);

        Service *newDue = new Service;
        
        newDue->model_mobil = model_mobil_due;
        newDue->merek_mobil = merek_mobil_due;
        newDue->deskripsi_kendala = deskripsi_kendala_due;
        newDue->nama_montir = nama_montir_due;
        newDue->tanggal = tanggaldue;

        Customer* cur = headCustomer; //cur = cursor customer 
        while(cur != NULL){
            if (cur->nama == namaSdue) break;
            cur = cur->next;
        }
        if (cur != NULL) {
            // save to customer
            if (cur->HeadServiceDue == NULL) //new list
                cur->HeadServiceDue = cur->TailServiceDue = newDue;
            else {
                cur->TailServiceDue->next = newDue; //add back
                newDue->prev = cur->TailServiceDue;
                cur->TailServiceDue = newDue;
            }
            newDue->dataCustomer = cur;

            //save to history
            if (headHistoryDue == NULL) {
                headHistoryDue = tailHistoryDue = newDue;
            } else {
                tailHistoryDue->allnext = newDue;
                newDue->allprev = tailHistoryDue;
                tailHistoryDue = newDue;
            }
        }
    }
    data_DueService2.close();

    ifstream data_DoneService2("data_DoneService2.txt"); //LIFO - stack
    string namaSdone, model_mobil_done, merek_mobil_done, deskripsi_kendala_done, nama_montir_done, tanggaldone, kosong2;
    while (getline(data_DoneService2, namaSdone) && namaSdone != "") {
        getline(data_DoneService2, model_mobil_done);    
        getline(data_DoneService2, merek_mobil_done);
        getline(data_DoneService2, deskripsi_kendala_done);
        getline(data_DoneService2, nama_montir_done);
        getline(data_DoneService2, tanggaldone);
        getline(data_DoneService2, kosong2);
        
        Service *newDone = new Service;
        
        newDone->model_mobil = model_mobil_done;
        newDone->merek_mobil = merek_mobil_done;
        newDone->deskripsi_kendala = deskripsi_kendala_done;
        newDone->nama_montir = nama_montir_done;
        newDone->tanggal = tanggaldone;

        Customer* cur = headCustomer; //cur = cursor customer
        while(cur != NULL){
            if (cur->nama == namaSdone) break;
            cur = cur->next;
        }
        if (cur != NULL) {
            // save to customer
            if (cur->HeadServiceDone == NULL) //new list
                cur->HeadServiceDone = cur->TailServiceDone = newDone;
            else {
                newDone->next = cur->HeadServiceDone; //add front
                cur->HeadServiceDone->prev = newDone;
                cur->HeadServiceDone = newDone;
            }
            newDone->dataCustomer = cur;

            //save to history
            if (headHistoryDone == NULL) {
                headHistoryDone = tailHistoryDone = newDone;
            } else {
                newDone->allnext = headHistoryDone; //add front
                headHistoryDone->allprev = newDone;
                headHistoryDone = newDone;
            }
        }
    }
    data_DoneService2.close();

    while (!keluar) MenuCustomer();
    cout << "Bye! \n\n";

}