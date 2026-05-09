#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Nodes.h"
#include <fstream>

int indexMontir = 0;

// Linked list Helper

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
        newMontir->kerjaTerpentingDue = data_servis; // for Due service only

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

void UpdateMontir(Service* Data, bool priority_service = false, bool Reverse = false) { //list montir tergantung data servis due or done, due&singular list, reverse = from tail, then -> prev
    Service* bantuServis = Data;
    if (bantuServis == NULL) return;
    
    while (bantuServis != NULL) {
        if (priority_service) NewMontirToList(bantuServis->nama_montir, bantuServis); //add newest service (terpenting)
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

string FindMechanicName(int indexInput) {
    Montir* cur = headMontir;

    while (cur != NULL) {
        if (cur->index == indexInput) return cur->namaMontir;
        cur = cur->next;
    }
    return "Tidak ada";
}

void RelinkDueHistory(Service* Servis){ //re-link history proximity (customer history remain queued)
    if (Servis->allprev != NULL) Servis->allprev->allnext = Servis->allnext;//per History
    else headHistoryDue = Servis->allnext;
        
    if (Servis->allnext != NULL) Servis->allnext->allprev = Servis->allprev;
    else tailHistoryDue = Servis->allprev;

    Servis->allnext = Servis->allprev = NULL;
}

void RelinkDueCustomer(Service* Servis){
    if (Servis->prev != NULL) Servis->prev->next = Servis->next;//per Customer
    else Servis->dataCustomer->HeadServiceDue = Servis->next;

    if (Servis->next != NULL) Servis->next->prev = Servis->prev;
    else Servis->dataCustomer->TailServiceDue = Servis->prev;

    Servis->next = Servis->prev = NULL;//reset pointer
}

void ReLinkDue(Service* Servis) { //re-link (include customer) proximity
    RelinkDueHistory(Servis);
    RelinkDueCustomer(Servis);
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

//Sorting Due Service
void SortByQueue(){ //default, reset and read back from file
    Customer* cus = headCustomer;
        while (cus != NULL) {
        cus->HeadServiceDue = cus->TailServiceDue = NULL;
        cus = cus->next;
    }
    
    while (headHistoryDue != NULL) { 
        Service* temp = headHistoryDue;
        headHistoryDue = headHistoryDue->allnext;
        delete temp;
    }

    ifstream data_DueService("../data_DueService.txt"); //FIFO - queue
    string namaSdue, model_mobil_due, merek_mobil_due, deskripsi_kendala_due, nama_montir_due, tanggaldue, kepentingan, kosong3;
    while (getline(data_DueService, namaSdue) && namaSdue != "") {  
        getline(data_DueService, model_mobil_due);  
        getline(data_DueService, merek_mobil_due);
        getline(data_DueService, deskripsi_kendala_due);
        getline(data_DueService, nama_montir_due);
        getline(data_DueService, tanggaldue);
        getline(data_DueService, kepentingan);
        getline(data_DueService, kosong3);

        Service *newDue = new Service;
        
        newDue->model_mobil = model_mobil_due;
        newDue->merek_mobil = merek_mobil_due;
        newDue->deskripsi_kendala = deskripsi_kendala_due;
        newDue->nama_montir = nama_montir_due;
        newDue->tanggal = tanggaldue;
        newDue->kepentingan = kepentingan;

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
}

void SortByDate(){ // Selection sort (find minimum, extract to left) // 1 4 5 6 2 3
    Service* min = headHistoryDue;                                   // i c
    int hari, bulan, tahun;                                          // m
    sscanf(min->tanggal.c_str(), "%d-%d-%d", &hari, &bulan, &tahun);
    
    Service* index = headHistoryDue;
    Service* cur = headHistoryDue->allnext;
    int cur_hari, cur_bulan, cur_tahun;
    while (cur != NULL){
        sscanf(cur->tanggal.c_str(), "%d-%d-%d", &cur_hari, &cur_bulan, &cur_tahun);

        if (cur_tahun < tahun) min = cur;
        else if (cur_tahun == tahun){

            if (cur_bulan < bulan) min = cur;
            else if (cur_bulan == bulan){

                if (cur_hari < hari) min = cur;
            }
        }
        if (min == cur){
            hari = cur_hari;
            bulan = cur_bulan;
            tahun = cur_tahun;
        }
        cur = cur->allnext;
        if (cur == NULL && index->allnext != NULL) { //reach last, append to next index
            if (index != min) {
                RelinkDueHistory(min); //link middle / edge list to proximity (min->next->prev = min->prev, etc..)
                min->allnext = index;
                if (index->allprev){ // if not in first index 
                    index->allprev->allnext = min;
                    min->allprev = index->allprev;
                } else {
                    headHistoryDue = min;
                }
                index->allprev = min;
            }
            if (index == min) index = index->allnext; //advance when no change
            cur = index->allnext;
            min = index;
            sscanf(min->tanggal.c_str(), "%d-%d-%d", &hari, &bulan, &tahun);
        }
    }
    // sort customer's service (wire new list)
    Customer* cus = headCustomer;
    while (cus != NULL) {
        cus->HeadServiceDue = cus->TailServiceDue = NULL;
        cur = headHistoryDue;
        while(cur != NULL) {
            if (cur->dataCustomer == cus) {
                if (cus->HeadServiceDue == NULL) {
                    cur->prev = NULL;
                    cus->HeadServiceDue = cus->TailServiceDue = cur;
                } else {
                    cus->TailServiceDue->next = cur;
                    cur->prev = cus->TailServiceDue;
                    cus->TailServiceDue = cur;
                }
            }
            cur = cur->allnext;
        }
        if (cus->TailServiceDue != NULL) 
            cus->TailServiceDue->next = NULL;


        cus = cus->next;
    }
}

void SortByUrgency(){
    SortByDate();
    
    Service* index = headHistoryDue;
    Service* min = index;
    int urgent = stoi(min->kepentingan);
    
    Service* cur = headHistoryDue->allnext;
    int cur_urgent;
    while (cur != NULL) {
        cur_urgent = stoi(cur->kepentingan); 

        if (cur_urgent > urgent) {
            min = cur;
            urgent = cur_urgent;
        }
        cur = cur->allnext;

        if (cur == NULL && index->allnext != NULL) { //reach last, append to next index
            if (index != min) {
                RelinkDueHistory(min); //link middle / edge list to proximity (min->next->prev = min->prev, etc..)
                min->allnext = index;
                if (index->allprev){
                    index->allprev->allnext = min;
                    min->allprev = index->allprev;
                } else {
                    headHistoryDue = min;
                }
                index->allprev = min;
            }
            if (index == min) index = index->allnext; //advanced when stays in place
            cur = index->allnext;
            min = index;
            urgent = stoi(min->kepentingan);
        }
    }
    // sort customer's service (rewire existing list)
    Customer* cus = headCustomer;
    while (cus != NULL) {
        if (cus->HeadServiceDue == NULL || cus->HeadServiceDue->next == NULL) {
            cus = cus->next;
            continue; // nothing to sort
        }
        index = cus->HeadServiceDue;
        min = index;
        urgent = stoi(min->kepentingan);

        cur = cus->HeadServiceDue->next;
        while(cur != NULL) {
            cur_urgent = stoi(cur->kepentingan);
            if (cur_urgent > urgent) {
                min = cur;
                urgent = cur_urgent;
            }
            cur = cur->next;

            if (cur == NULL && index->next != NULL) { //reach last, append to next index
                if (index != min) {
                    RelinkDueCustomer(min); //link middle / edge list to proximity (min->next->prev = min->prev, etc..)
                    min->next = index;
                    if (index->prev){
                        index->prev->next = min;
                        min->prev = index->prev;
                    } else {
                        cus->HeadServiceDue = min;
                    }
                    index->prev = min;
                }
                if (index == min) index = index->next; //advanced when stays in place
                cur = index->next;
                min = index;
                urgent = stoi(min->kepentingan);
            }
        }
        cus = cus->next;
    }
}



#endif