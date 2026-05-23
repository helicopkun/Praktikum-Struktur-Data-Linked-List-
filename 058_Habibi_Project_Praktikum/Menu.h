#ifndef MENU_H
#define MENU_H

#include "Display.h"

//------------------------------------------ Menus ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
void main_menu() {
    cout << R"(==== Welcome to Inventory, Manager ====

1. Inventory View - View all Barang and their total stock
   - Barang details
   - Search Barang
   - Withdraw Barang
   - Register Barang
   - Change inventory location

2. Gudang Management - View stock per Gudang (barang_gudang)
   - View Gudang
   - Transfer Barang between Gudang
   - Buy Barang for Gudang
   - Register item for Gudang
   - Register Gudang

3. Demand Queue - View pending demand queue (queue_head)
   - Process/dequeue demand (dequeue_demand)

4. Riwayat - View history log (riwayat_head stack)


0. Exit

)";
   int pilihan = input_idx("Pilih menu: ", 0, 4);
   system("cls");
   if (pilihan == 0) return;
   else if (pilihan == 1) menu_inventory_management();
   else if (pilihan == 2) menu_gudang_management();
   else if (pilihan == 3) menu_queue_demand();
   else if (pilihan == 4) menu_riwayat();
   
   system("cls");
   main_menu();
}


#endif