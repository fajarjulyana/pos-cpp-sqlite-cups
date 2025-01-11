# Sistem Point of Sale (POS) Print Feature

Sistem Point of Sale (POS) sederhana ini menggunakan SQLite untuk database dan CUPS untuk pencetakan struk. Program ini dapat digunakan untuk mengelola produk, keranjang belanja, dan melakukan transaksi pembelian.

## Fitur

1. **Tambah Produk**: Menambahkan produk baru ke dalam database dengan nama, harga, dan stok. id produk input manual dan bisa ukuran panjang.
2. **Lihat Produk**: Menampilkan daftar produk yang ada di dalam database.
3. **Perbarui Produk**: Memperbarui harga dan stok produk yang sudah ada.
4. **Hapus Produk**: Menghapus produk dari database.
5. **Tambah ke Keranjang**: Menambahkan produk ke dalam keranjang belanja.
6. **Lihat Keranjang**: Menampilkan item yang ada di dalam keranjang belanja.
7. **Pembayaran**: Melakukan pembayaran untuk produk dalam keranjang dan menghasilkan struk pembelian. Struk dapat dicetak menggunakan printer yang tersedia.
   
## Persyaratan

- Compiler C++ yang mendukung C++11 atau lebih tinggi.
- **SQLite3**: Untuk database lokal.
- **CUPS**: Untuk pencetakan struk (hanya tersedia di sistem berbasis Unix).
- Library standar C++ (iostream, vector, string, fstream).

## Instalasi

1. Pastikan Anda telah menginstal SQLite3 dan CUPS di sistem Anda.
2. Unduh atau kloning repositori ini ke dalam direktori proyek.
3. Kompilasi program dengan menggunakan perintah sesuai dengan compiler yang Anda gunakan. Contoh dengan g++:

   ```bash
   g++ -std=c++11 -o pos pos.cpp -lsqlite3 -lcups
   ```

4. Jalankan program:

   ```bash
   ./pos
   ```

## Penjelasan Program

Program ini menggunakan SQLite untuk menyimpan data produk dan transaksi penjualan. Setiap produk memiliki ID, nama, harga, dan stok. Ketika transaksi dilakukan, data transaksi akan disimpan di tabel `penjualan`.

### Fungsi Utama

1. **buatTabel()**: Membuat tabel `produk` dan `penjualan` di database jika belum ada.
2. **tambahProduk()**: Menambahkan produk baru ke dalam tabel `produk`.
3. **tampilkanProduk()**: Menampilkan semua produk yang ada di dalam tabel `produk`.
4. **perbaruiProduk()**: Memperbarui harga dan stok produk berdasarkan ID.
5. **hapusProduk()**: Menghapus produk berdasarkan ID.
6. **tambahKeKeranjang()**: Menambahkan produk ke dalam keranjang belanja jika stok mencukupi.
7. **tampilkanKeranjang()**: Menampilkan produk yang ada di dalam keranjang.
8. **pembayaran()**: Melakukan pembayaran dan mengupdate stok produk yang dibeli, serta mencetak struk pembelian.

## Pencetakan Struk

Setelah pembayaran dilakukan, struk pembelian akan disimpan ke dalam file `struk.txt`. Program ini juga dapat mencetak struk menggunakan printer yang terhubung melalui CUPS.

## Catatan

- Sistem ini menggunakan database SQLite yang disimpan dalam file `pos.db`.
- Pencetakan struk hanya dapat dilakukan di sistem yang mendukung CUPS (misalnya, Linux atau macOS).
- Pastikan printer sudah terhubung dan dikonfigurasi dengan benar.
