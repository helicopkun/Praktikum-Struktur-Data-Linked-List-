#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Nodes.h"
#include <fstream>
#include <iostream>

int indexMontir = 0;

//=====================Services Linked list Helper================================================================================================================================================================

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

void ResetMontir() { //reset list montir
    indexMontir = 0;
    while (headMontir != NULL) { 
        Montir* temp = headMontir;
        headMontir = headMontir->next;
        delete temp;
    }
}

void ReadAllMontir() { //baca semua nama montir dalam file dan di simpan ke linkedlist
    ResetMontir();
    ifstream data_montir ("../Nama_Montir.txt");
    string namaMontir, space;
    while (getline(data_montir, namaMontir) && namaMontir != "") {
        getline(data_montir, space);
        NewMontirToList(namaMontir); //tambah montir
    }
    data_montir.close();
}

void TampilMechanic() {
    Montir* cur = headMontir;

    while (cur != NULL) {
        cout << cur->index << ". " << cur->namaMontir << endl;
        cur = cur->next;
    }
}

string FindMechanicName(int indexInput) {
    Montir* cur = headMontir;

    while (cur != NULL) {
        if (cur->index == indexInput) return cur->namaMontir;
        cur = cur->next;
    }
    return "Tidak ada";
}

void RewriteDue() {
    ofstream data_DueService("../data_DueService.txt", ios::trunc); //rewrite file due
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

// Files
void ReadFiles() {
        ifstream data_customer("../data_customer.txt"); // FIFO
    string namaC, umur, gender, nomor_teleponC, alamat, kosong1;
    while (getline(data_customer, namaC) && namaC != "") {
        getline(data_customer, umur);
        getline(data_customer, gender);
        getline(data_customer, nomor_teleponC);
        getline(data_customer, alamat);
        getline(data_customer, kosong1);
        
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
    data_customer.close();

    //link data servis -> data customer
    ifstream data_DueService("../data_DueService.txt"); //FIFO - queue
    string namaSdue, model_mobil_due, merek_mobil_due, deskripsi_kendala_due, nama_montir_due, tanggaldue, kosong3;
    while (getline(data_DueService, namaSdue) && namaSdue != "") {  
        getline(data_DueService, model_mobil_due);  
        getline(data_DueService, merek_mobil_due);
        getline(data_DueService, deskripsi_kendala_due);
        getline(data_DueService, nama_montir_due);
        getline(data_DueService, tanggaldue);
        getline(data_DueService, kosong3);

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
    data_DueService.close();

    ifstream data_DoneService("../data_DoneService.txt"); //LIFO - stack
    string namaSdone, model_mobil_done, merek_mobil_done, deskripsi_kendala_done, nama_montir_done, tanggaldone, kosong2;
    while (getline(data_DoneService, namaSdone) && namaSdone != "") {
        getline(data_DoneService, model_mobil_done);    
        getline(data_DoneService, merek_mobil_done);
        getline(data_DoneService, deskripsi_kendala_done);
        getline(data_DoneService, nama_montir_done);
        getline(data_DoneService, tanggaldone);
        getline(data_DoneService, kosong2);
        
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
    data_DoneService.close();

}

void AddCustomer(Customer* customer) {
    if (headCustomer == NULL) {
        headCustomer = tailCustomer = customer;
    } else {
        customer->prev = tailCustomer;
        tailCustomer->next = customer;
        tailCustomer = customer;
    }
    ofstream data_customer("../data_customer.txt", ios::app);
    data_customer 
    << customer->nama << endl 
    << customer->umur << endl 
    << customer->gender << endl 
    << customer->nomor_telepon << endl 
    << customer->alamat << endl << endl;
    data_customer.close();
}

void AddService(Customer* Pelanggan, Service* baru){
    if (Pelanggan->TailServiceDue == NULL) { 
        Pelanggan->HeadServiceDue = Pelanggan->TailServiceDue = baru; // servis pertama
    } else {
        Pelanggan->TailServiceDue->next = baru; // add queue
        baru->prev = Pelanggan->TailServiceDue;
        Pelanggan->TailServiceDue = baru;
    }
    baru->dataCustomer = Pelanggan;

    ofstream data_DueService("../data_DueService.txt", ios::app);
    data_DueService 
    << baru->dataCustomer->nama << endl 
    << baru->model_mobil << endl 
    << baru->merek_mobil << endl 
    << baru->deskripsi_kendala << endl 
    << baru->nama_montir << endl 
    << baru->tanggal << endl << endl;
    data_DueService.close();
}


#endif