#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sqlite3.h>
#include <cups/cups.h>
#include <ctime>

struct ItemKeranjang {
    std::string idProduk;
    std::string namaProduk;
    double harga;
    int jumlah;
};

std::vector<ItemKeranjang> keranjang;
sqlite3* db;
char* pesanError;

void buatTabel() {
    const char* sql = R"(
	CREATE TABLE IF NOT EXISTS produk (
	    id TEXT PRIMARY KEY,
	    nama TEXT NOT NULL,
	    harga REAL NOT NULL,
	    stok INTEGER NOT NULL
	);

	CREATE TABLE IF NOT EXISTS penjualan (
	    id TEXT PRIMARY KEY,
	    id_produk TEXT NOT NULL,
	    jumlah INTEGER NOT NULL,
	    total_harga REAL NOT NULL,
	    FOREIGN KEY(id_produk) REFERENCES produk(id)
	);
    )";

    if (sqlite3_exec(db, sql, nullptr, nullptr, &pesanError) != SQLITE_OK) {
        std::cerr << "Gagal membuat tabel: " << pesanError << "\n";
        sqlite3_free(pesanError);
    }
}

void tambahProduk(const std::string& id, const std::string& nama, double harga, int stok) {
    std::string sql = "INSERT INTO produk (id, nama, harga, stok) VALUES ('" + id + "', '" + nama + "', " +
                      std::to_string(harga) + ", " + std::to_string(stok) + ");";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &pesanError) != SQLITE_OK) {
        std::cerr << "Gagal menambahkan produk: " << pesanError << "\n";
        sqlite3_free(pesanError);
    } else {
        std::cout << "Produk berhasil ditambahkan.\n";
    }
}


void tampilkanProduk() {
    std::string sql = "SELECT id, nama, harga, stok FROM produk";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Gagal menyiapkan query: " << sqlite3_errmsg(db) << "\n";
        return;
    }

    std::cout << "ID Produk    Nama Produk     Harga    Stok\n";
    std::cout << "--------------------------------------------\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* nama = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double harga = sqlite3_column_double(stmt, 2);
        int stok = sqlite3_column_int(stmt, 3);

        // Menampilkan ID dengan lebar kolom yang sama, misalnya 10 karakter
        std::printf("%-12s %-15s %-8.2f %-5d\n", id, nama, harga, stok);
    }

    sqlite3_finalize(stmt);
}


void perbaruiProduk(const std::string& id, double harga, int stok) {
    std::string sql = "UPDATE produk SET harga = " + std::to_string(harga) +
                      ", stok = " + std::to_string(stok) +
                      " WHERE id = '" + id + "';";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &pesanError) != SQLITE_OK) {
        std::cerr << "Gagal memperbarui produk: " << pesanError << "\n";
        sqlite3_free(pesanError);
    } else {
        std::cout << "Produk berhasil diperbarui.\n";
    }
}


void hapusProduk(const std::string& id) {
    std::string sql = "DELETE FROM produk WHERE id = '" + id + "';";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &pesanError) != SQLITE_OK) {
        std::cerr << "Gagal menghapus produk: " << pesanError << "\n";
        sqlite3_free(pesanError);
    } else {
        std::cout << "Produk berhasil dihapus.\n";
    }
}

void tambahKeKeranjang(std::string idProduk, int jumlah) {
    std::string sql = "SELECT nama, harga, stok FROM produk WHERE id = " + idProduk + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string nama = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double harga = sqlite3_column_double(stmt, 1);
            int stok = sqlite3_column_int(stmt, 2);

            if (jumlah <= stok) {
                keranjang.push_back({idProduk, nama, harga, jumlah});
                std::cout << "Produk berhasil ditambahkan ke keranjang.\n";
            } else {
                std::cout << "Stok tidak mencukupi.\n";
            }
        } else {
            std::cout << "Produk tidak ditemukan.\n";
        }
    } else {
        std::cerr << "Gagal mengambil produk: " << sqlite3_errmsg(db) << "\n";
    }
    sqlite3_finalize(stmt);
}

void tampilkanKeranjang() {
    if (keranjang.empty()) {
        std::cout << "Keranjang kosong.\n";
        return;
    }

    double total = 0.0;
    std::cout << "Item dalam Keranjang:\n";
    std::cout << "----------------------------------------\n";
    for (const auto& item : keranjang) {
        double subtotal = item.harga * item.jumlah;
        std::cout << "ID: " << item.idProduk << ", Nama: " << item.namaProduk
                  << ", Harga: " << item.harga << ", Jumlah: " << item.jumlah
                  << ", Subtotal: " << subtotal << "\n";
        total += subtotal;
    }
    std::cout << "----------------------------------------\n";
    std::cout << "Total: Rp " << total << "\n";
}
void cetakStruk(const std::string& namaFile) {
    // Mendapatkan daftar printer yang tersedia
    cups_dest_t *dests;
    int num_dests = cupsGetDests(&dests);

    if (num_dests == 0) {
        std::cerr << "Tidak ada printer yang tersedia!" << std::endl;
  
    }

    // Menampilkan daftar printer
    std::cout << "Daftar printer yang tersedia:" << std::endl;
    for (int i = 0; i < num_dests; i++) {
        std::cout << i + 1 << ". " << dests[i].name << std::endl;
    }

    // Meminta pengguna memilih printer
    int choice;
    std::cout << "Pilih printer (masukkan nomor): ";
    std::cin >> choice;

    if (choice < 1 || choice > num_dests) {
        std::cerr << "Pilihan tidak valid!" << std::endl;
        cupsFreeDests(num_dests, dests);

    }

    // Mendapatkan nama printer yang dipilih
    const char *printer_name = dests[choice - 1].name;
    std::cout << "Printer yang dipilih: " << printer_name << std::endl;

    // Mencetak file
    int job_id = cupsPrintFile(printer_name, namaFile.c_str(), "Print Job", 0, NULL);
    if (job_id == 0) {
        std::cerr << "Gagal mengirim file untuk dicetak!" << std::endl;
        cupsFreeDests(num_dests, dests);

    }

    std::cout << "Pekerjaan cetak berhasil dengan ID: " << job_id << std::endl;

    // Membersihkan memori yang digunakan
    cupsFreeDests(num_dests, dests);


}


void pembayaran() {
    if (keranjang.empty()) {
        std::cout << "Keranjang kosong.\n";
        return;
    }

    double total = 0.0;
    for (const auto& item : keranjang) {
        total += item.harga * item.jumlah;
    }

    double pembayaran;
    std::cout << "Total: Rp " << total << "\n";
    std::cout << "Masukkan jumlah pembayaran: Rp ";
    std::cin >> pembayaran;

    if (pembayaran >= total) {
        double kembalian = pembayaran - total;
        std::cout << "Pembayaran berhasil. Kembalian: Rp " << kembalian << "\n";

        for (const auto& item : keranjang) {
            std::string sql = "INSERT INTO penjualan (id_produk, jumlah, total_harga) VALUES (" +
                              item.idProduk + ", " +
                              std::to_string(item.jumlah) + ", " +
                              std::to_string(item.harga * item.jumlah) + ");";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &pesanError);

            std::string updateStok = "UPDATE produk SET stok = stok - " + std::to_string(item.jumlah) +
                                     " WHERE id = " + item.idProduk + ";";
            sqlite3_exec(db, updateStok.c_str(), nullptr, nullptr, &pesanError);
        }

        std::ofstream struk("struk.txt");
	// Mendapatkan tanggal dan waktu saat ini
	std::time_t now = std::time(nullptr);
	std::tm* localTime = std::localtime(&now);

	struk << "=================================\n";
	struk << "=  STRUK TRANSAKSI FJ FOTOCOPY  =\n";
	struk << "=================================\n";
	struk << "Tanggal: " << (localTime->tm_mday) << "/"
	      << (localTime->tm_mon + 1) << "/" << (localTime->tm_year + 1900) << "\n";
	struk << "Waktu  : " << (localTime->tm_hour) << ":"
	      << (localTime->tm_min) << ":"
	      << (localTime->tm_sec) << "\n";
	struk << "---------------------------------\n";

	for (const auto& item : keranjang) {
	    struk << "Nama       : " << item.namaProduk << "\n"
		  << "Harga      : Rp " << item.harga << "\n"
		  << "Jumlah     : " << item.jumlah << "\n"
		  << "Subtotal   : Rp " << item.harga * item.jumlah << "\n"
		  << "---------------------------------\n";
	}

	struk << "Total      : Rp " << total << "\n";
	struk << "Pembayaran : Rp " << pembayaran << "\n";
	struk << "Kembalian  : Rp " << kembalian << "\n";
	struk.close();

	std::cout << "Struk disimpan ke struk.txt.\n";
	cetakStruk("struk.txt");
	keranjang.clear();
    } else {
        std::cout << "Pembayaran tidak mencukupi.\n";
    }
}

void menuUtama() {
    int pilihan;
    do {
        std::cout << "\n1. Tambah Produk\n2. Lihat Produk\n3. Perbarui Produk\n4. Hapus Produk\n";
        std::cout << "5. Tambah ke Keranjang\n6. Lihat Keranjang\n7. Pembayaran\n8. Keluar\n";
        std::cout << "Pilih: ";
        std::cin >> pilihan;

        switch (pilihan) {
	case 1: {
	    std::string id, nama;
	    double harga;
	    int stok;

	    std::cout << "Masukkan ID Produk: ";
	    std::cin >> id;
	    std::cout << "Masukkan Nama Produk: ";
	    std::cin.ignore(); // Membersihkan buffer
	    std::getline(std::cin, nama);
	    std::cout << "Masukkan Harga Produk: ";
	    std::cin >> harga;
	    std::cout << "Masukkan Stok Produk: ";
	    std::cin >> stok;

	    tambahProduk(id, nama, harga, stok);
	    break;
	}

            case 2:
                tampilkanProduk();
                break;
            case 3: {
                std::string id;
                double harga;
                int stok;
                std::cout << "Masukkan ID Produk yang akan diperbarui: ";
                std::cin >> id;
                std::cout << "Masukkan Harga Baru: ";
                std::cin >> harga;
                std::cout << "Masukkan Stok Baru: ";
                std::cin >> stok;
                perbaruiProduk(id, harga, stok);
                break;
            }
            case 4: {
                std::string id;
                std::cout << "Masukkan ID Produk yang akan dihapus: ";
                std::cin >> id;
                hapusProduk(id);
                break;
            }
            case 5: {
                std::string id;
                int jumlah;
                std::cout << "ID Produk: ";
                std::cin >> id;
                std::cout << "Jumlah: ";
                std::cin >> jumlah;
                tambahKeKeranjang(id, jumlah);
                break;
            }
            case 6:
                tampilkanKeranjang();
                break;
            case 7:
                pembayaran();
                break;
        }
    } while (pilihan != 8);
}

int main() {
    if (sqlite3_open("pos.db", &db) != SQLITE_OK) {
        std::cerr << "Gagal membuka database: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }

    buatTabel();
    menuUtama();
    sqlite3_close(db);

    return 0;
}
