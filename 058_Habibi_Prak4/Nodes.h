#ifndef NODES_H
#define NODES_H

#include <string>
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

#endif