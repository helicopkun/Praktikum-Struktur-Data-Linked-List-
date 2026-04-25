#include "Menu.h"

int main() {
    string nama, no_telp;
    Customer* cur;

    ReadFiles();

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
