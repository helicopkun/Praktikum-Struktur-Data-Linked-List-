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
    Service* kerjaTerlamaDue = NULL;
};

Customer* headCustomer = NULL; //first in - first out (data terakhir = customer terakhir)
Customer* tailCustomer = NULL;

Service *headHistoryDue = NULL; //first in - first out (data pertama = servis pertama montir)
Service *tailHistoryDue = NULL;

Service *headUndo = NULL; //last in - first out

Service *headHistoryDone = NULL; //last in - first out (data terakhir = riwayat pertama)
Service *tailHistoryDone = NULL;

Montir* headMontir = NULL; //first in - first out

string namaInput, noTelp;
bool validPilihan, 
     keluarAdmin, 
     keluar = false;
int indexMontir = 0;


//---------------------------------------Backend stuffs-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//New
bool NewMontirToList(string nama, Service* data_servis = NULL) { //add montir from back (skalian ngecek kesamaan), data_servis only for Due terlama
    Montir* bantuMontir = headMontir;
    bool baru = true;   

    while (bantuMontir != NULL) { //cek current list
        if (bantuMontir->namaMontir == nama) {
            baru = false;
            break;
        }
        bantuMontir = bantuMontir->next;
    }
    if (baru) {
        Montir* newMontir = new Montir; //new node
        newMontir->namaMontir = nama;
        newMontir->index = ++indexMontir; // ++Index
        newMontir->kerjaTerlamaDue = data_servis; // for Due service only

        if (headMontir == NULL) {
            headMontir = newMontir; // awal list montir
        } else {
            Montir* tail = headMontir;
            while (tail->next != NULL) tail = tail->next;
            tail->next = newMontir; //ujung list montir
        }
    }
    return baru;
}
//Updated
void UpdateMontir(Service* Data, bool Due_and_Singular, bool Reverse = false) { //list montir tergantung data servis due or done, due&singular list, reverse = from tail, then -> prev
    Service* bantuServis = Data;
    if (bantuServis == NULL) return;
    
    while (bantuServis != NULL) {
        if (Due_and_Singular) (bantuServis->nama_montir, bantuServis); //if list for due&singular add newest service
        else NewMontirToList(bantuServis->nama_montir);
     
        if (Reverse) bantuServis = bantuServis->allprev;
        else bantuServis = bantuServis->allnext;
    }
}
//New
void ResetMontir() { //reset list montir
    indexMontir = 0;
    while (headMontir != NULL) { 
        Montir* temp = headMontir;
        headMontir = headMontir->next;
        delete temp;
    }
}
//New
void ReadAllMontir() { //baca semua nama montir dalam file dan di simpan ke linkedlist
    ResetMontir();
    ifstream data_montir ("Nama_Montir.txt");
    string namaMontir, space;
    while (getline(data_montir, namaMontir) && namaMontir != "") {
        getline(data_montir, space);
        NewMontirToList(namaMontir); //tambah montir
    }
    data_montir.close();
}
//simplified
void TampilMechanic() {
    Montir* cur = headMontir;

    while (cur != NULL) {
        cout << cur->index << ". " << cur->namaMontir << endl;
        cur = cur->next;
    }
}
//simplified
string FindMechanicName(int indexInput) {
    Montir* cur = headMontir;

    while (cur != NULL) {
        if (cur->index == indexInput) return cur->namaMontir;
        cur = cur->next;
    }
    return "Tidak ada";
}
//New
void RewriteDue() {
    ofstream data_DueService("data_DueService3.txt", ios::trunc); //rewrite file due
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
//New
void ReLinkDue(Service* Servis) { //re-link setelah di hapus dari list
    if (Servis->allprev != NULL) Servis->allprev->allnext = Servis->allnext;//per History
    else headHistoryDue = Servis->allnext;
        
    if (Servis->allnext != NULL) Servis->allnext->allprev = Servis->allprev;
    else tailHistoryDue = Servis->allprev;

    if (Servis->prev != NULL) Servis->prev->next = Servis->next;//per Customer
    else Servis->dataCustomer->HeadServiceDue = Servis->next;

    if (Servis->next != NULL) Servis->next->prev = Servis->prev;
    else Servis->dataCustomer->TailServiceDue = Servis->prev;

    Servis->next = Servis->prev = Servis->allnext = Servis->allprev = NULL;//reset pointer
}
//New
void AddDueList(Service* Servis, Customer* Pelanggan) { //add referenced node to back Due - queue
    // save to customer
    if (Pelanggan->HeadServiceDue == NULL)
        Pelanggan->HeadServiceDue = Pelanggan->TailServiceDue = Servis;
    else {
        Pelanggan->TailServiceDue->next = Servis;
        Servis->prev = Pelanggan->TailServiceDue;
        Pelanggan->TailServiceDue = Servis;
    }
    Servis->dataCustomer = Pelanggan;

    //save to history
    if (headHistoryDue == NULL) {
        headHistoryDue = tailHistoryDue = Servis;
    } else {
        tailHistoryDue->allnext = Servis;
        Servis->allprev = tailHistoryDue;
        tailHistoryDue = Servis;
    }

}
//New
void AddDoneList(Service* Servis, Customer* Pelanggan) { //add referenced node to front Done - stack
    // save to customer
    if (Pelanggan->HeadServiceDone == NULL)
        Pelanggan->HeadServiceDone = Pelanggan->TailServiceDone = Servis;
    else {
        Servis->next = Pelanggan->HeadServiceDone;
        Pelanggan->HeadServiceDone->prev = Servis;
        Pelanggan->HeadServiceDone = Servis;
    }
    Servis->dataCustomer = Pelanggan;

    //save to history
    if (headHistoryDone == NULL) {
        headHistoryDone = tailHistoryDone = Servis;
    } else {
        Servis->allnext = headHistoryDone;
        headHistoryDone->allprev = Servis;
        headHistoryDone = Servis;
    }
}


//----------------------------------------Frontend stuffs-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//tiny change
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

    if (headCustomer == NULL) {
        headCustomer = tailCustomer = baruCustomer;
    } else {
        baruCustomer->prev = tailCustomer;
        tailCustomer->next = baruCustomer;
        tailCustomer = baruCustomer;
    }
    ofstream data_customer3("data_customer3.txt", ios::app);
    data_customer3 << namaInput << endl 
    << baruCustomer->umur << endl 
    << baruCustomer->gender << endl 
    << baruCustomer->nomor_telepon << endl 
    << baruCustomer->alamat << endl << endl;
    data_customer3.close();
    return true;
}
//Big update
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
                
                if (!customer) { //input nama misal bukan dari menu customer (update namaInput)
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
                    if (!customer) cout << "Nama Pelanggan: " << namaInput << endl;
                cout << "\nMobil ini sudah dibengkel anda ingin menambahkan kendala ini kepada servis" << endl <<
                        "tersebut (yes/no): ";
                cin >> pilihan;
                cin.ignore();
            } while (pilihan != "yes" && pilihan != "no");

            if (pilihan == "no") return;
            if (pilihan == "yes") {
                curService->deskripsi_kendala = curService->deskripsi_kendala + " + " + kendala;
                RewriteDue(); 
                if (customer) cout << "\nServis sudah tercatat, nomor antrian anda adalah: " << antrian << endl << endl;
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

    if (customer) { //kalo dari "menu booking customer" tidak perlu cek / daftar ulang
        if (Pelanggan->TailServiceDue == NULL) { 
            Pelanggan->HeadServiceDue = Pelanggan->TailServiceDue = baruService; // servis pertama
        } else {
            Pelanggan->TailServiceDue->next = baruService; // add queue
            baruService->prev = Pelanggan->TailServiceDue;
            Pelanggan->TailServiceDue = baruService;
        }
        baruService->dataCustomer = Pelanggan;
        cout << "Servis sudah tercatat, nomor antrian anda adalah: " << antrian << "\n\n\n";
    } else {
        if (NewCustomer(askName)) { //customer baru :D  (dari menu admin servis)
            tailCustomer->HeadServiceDue = tailCustomer->TailServiceDue = baruService;
            baruService->dataCustomer = tailCustomer;
            cout << "\n\nPelanggan baru dan servis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << "\n\n\n";
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

            cout << "\n\nServis sudah tercatat, nomor antrian " << namaInput << " adalah: " << antrian << "\n\n\n";
        }
    }

    ofstream data_DueService3("data_DueService3.txt", ios::app);
    data_DueService3 << namaInput << endl 
    << baruService->model_mobil << endl 
    << baruService->merek_mobil << endl 
    << baruService->deskripsi_kendala << endl 
    << baruService->nama_montir << endl 
    << baruService->tanggal << endl << endl;
    data_DueService3.close();
}
//New
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
        ofstream nama_montir ("Nama_Montir.txt", ios::app);
        nama_montir << nama << endl;
        nama_montir.close();
        cout << nama << " telah ditambahkan ke daftar montir" << endl << endl;
    } else {
        cout << nama << " sudah terdaftar sebagai montir" << endl << endl;
    }
}
//tiny change
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
//added
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
//New
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
//New
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
//Updated
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
//Simplified
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
        ofstream data_DoneService ("data_DoneService3.txt", ios::app); //terbaru di akhir data
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


int main() {
    ifstream data_customer3("data_customer3.txt"); // FIFO
    string namaC, umur, gender, nomor_teleponC, alamat, kosong1;
    while (getline(data_customer3, namaC) && namaC != "") {
        getline(data_customer3, umur);
        getline(data_customer3, gender);
        getline(data_customer3, nomor_teleponC);
        getline(data_customer3, alamat);
        getline(data_customer3, kosong1);
        
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
    data_customer3.close();

    //link data servis -> data customer
    ifstream data_DueService3("data_DueService3.txt"); //FIFO - queue
    string namaSdue, model_mobil_due, merek_mobil_due, deskripsi_kendala_due, nama_montir_due, tanggaldue, kosong3;
    while (getline(data_DueService3, namaSdue) && namaSdue != "") {  
        getline(data_DueService3, model_mobil_due);  
        getline(data_DueService3, merek_mobil_due);
        getline(data_DueService3, deskripsi_kendala_due);
        getline(data_DueService3, nama_montir_due);
        getline(data_DueService3, tanggaldue);
        getline(data_DueService3, kosong3);

        Service *newDue = new Service;
        
        newDue->model_mobil = model_mobil_due;
        newDue->merek_mobil = merek_mobil_due;
        newDue->deskripsi_kendala = deskripsi_kendala_due;
        newDue->nama_montir = nama_montir_due;
        newDue->tanggal = tanggaldue;



        Customer* cur = headCustomer; //cur = cursor customer 
        while(cur != NULL){
            if (cur->nama == namaSdue) {
                AddDueList(newDue, cur);
                break;
            }
            cur = cur->next;
        }
    }
    data_DueService3.close();

    ifstream data_DoneService3("data_DoneService3.txt"); //LIFO - stack
    string namaSdone, model_mobil_done, merek_mobil_done, deskripsi_kendala_done, nama_montir_done, tanggaldone, kosong2;
    while (getline(data_DoneService3, namaSdone) && namaSdone != "") {
        getline(data_DoneService3, model_mobil_done);    
        getline(data_DoneService3, merek_mobil_done);
        getline(data_DoneService3, deskripsi_kendala_done);
        getline(data_DoneService3, nama_montir_done);
        getline(data_DoneService3, tanggaldone);
        getline(data_DoneService3, kosong2);
        
        Service *newDone = new Service;
        
        newDone->model_mobil = model_mobil_done;
        newDone->merek_mobil = merek_mobil_done;
        newDone->deskripsi_kendala = deskripsi_kendala_done;
        newDone->nama_montir = nama_montir_done;
        newDone->tanggal = tanggaldone;

        Customer* cur = headCustomer; //cur = cursor customer
        while(cur != NULL){
            if (cur->nama == namaSdone) {
                AddDoneList(newDone, cur);
                break;
            }
            cur = cur->next;
        }
    }
    data_DoneService3.close();


    string nama, no_telp;
    Customer* cur;
    do { //login page
        cur = headCustomer;
        system("cls");
        cout << "====== Welcome To Garasi Suby ======" << endl;
        cout << "Masukkan nama & Nomor Telepon" << endl << endl;

        cout << "Nama: ";
        getline(cin, nama);

        if (nama == "adminacces8008") { // access admin
            keluarAdmin = false;
            while (!keluarAdmin) MenuAdmin();
            continue;
        }

        cout << "No Telp: ";
        getline(cin, no_telp);

        while(cur != NULL) {
            if (cur->nama == nama && cur->nomor_telepon == no_telp) break;
            cur = cur->next;
        }

        if (cur == NULL) {
            cout << "\n\nNo customer bruh\n\n\n";
            cout << "Press enter to go back...";
            cin.get();
        }

    } while (cur == NULL || nama == "adminacces8008"); // kalo ujung / habis keluar dari menu admin 

    while (!keluar) MenuCustomer(nama); //menu customer

    cout << "Bye! \n\n";

}
