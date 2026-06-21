// ============================================================
//  Simulasi Mesin Turing: Secure Data Sanitization
//  Topik  : Penghapusan Data Aman pada Memori Biner
//  Bahasa : C++ (Standard C++17)
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  KONSTANTA & DEFINISI STATE
// ─────────────────────────────────────────────────────────────

// Simbol pita
const char SYM_ZERO  = '0';
const char SYM_ONE   = '1';
const char SYM_BLANK = 'B';

// State Mesin Turing
enum State {
    Q0_SCAN,    // State awal: mulai memindai dari kiri ke kanan
    Q1_ERASE,   // State sanitasi: menghapus (overwrite) simbol biner
    Q2_VERIFY,  // State verifikasi: memindai ulang dari kiri untuk memastikan bersih
    Q_ACCEPT,   // Final State / Accept State: sanitasi selesai
    Q_REJECT    // Reject State: kondisi error (tidak dipakai di alur normal)
};

// Konversi enum State ke string (untuk output trace)
string stateName(State s) {
    switch (s) {
        case Q0_SCAN:   return "Q0_SCAN";
        case Q1_ERASE:  return "Q1_ERASE";
        case Q2_VERIFY: return "Q2_VERIFY";
        case Q_ACCEPT:  return "Q_ACCEPT";
        case Q_REJECT:  return "Q_REJECT";
        default:        return "UNKNOWN";
    }
}

// ─────────────────────────────────────────────────────────────
//  KELAS MESIN TURING
// ─────────────────────────────────────────────────────────────

class TuringMachine {
private:
    vector<char> tape;   // Pita (tape) memori
    int   head;          // Posisi head saat ini
    State currentState;  // State saat ini
    int   step;          // Counter langkah (untuk trace)

    // ── Cetak isi pita dengan penanda posisi head ──────────────
    //    Simbol pada posisi head dibungkus tanda kurung siku [ ]
    void printTape() const {
        cout << "  Tape : ";
        for (int i = 0; i < (int)tape.size(); i++) {
            if (i == head)
                cout << "[" << tape[i] << "]";
            else
                cout << " " << tape[i] << " ";
        }
        cout << "\n";
    }

    // ── Cetak satu baris log langkah ──────────────────────────
    void printStep(const string& action) const {
        cout << "+-- Step " << setw(3) << step
             << " | State: " << setw(10) << left
             << stateName(currentState) << "| Head[" << head << "] = '"
             << tape[head] << "'\n";
        cout << "|  Aksi : " << action << "\n";
        printTape();
        cout << "+" << string(65, '-') << "\n";
    }

    // ── Baca simbol pada posisi head ──────────────────────────
    char read() const {
        return tape[head];
    }

    // ── Tulis simbol ke posisi head ───────────────────────────
    void write(char sym) {
        tape[head] = sym;
    }

    // ── Gerakkan head (L = kiri, R = kanan) ──────────────────
    //    Pita diperluas otomatis jika head mencapai tepi
    void move(char direction) {
        if (direction == 'R') {
            head++;
            // Tambah sel Blank di ujung kanan jika perlu
            if (head >= (int)tape.size())
                tape.push_back(SYM_BLANK);
        } else if (direction == 'L') {
            if (head > 0) head--;
            // Jika sudah di ujung kiri, tidak bergerak lagi
        }
    }

public:
    // ── Konstruktor: inisialisasi pita dari string biner input ─
    TuringMachine(const string& binaryInput) {
        // Isi pita dengan data biner yang diberikan
        for (char c : binaryInput)
            tape.push_back(c);

        // Tambahkan satu sel Blank di ujung kanan sebagai batas pita
        tape.push_back(SYM_BLANK);

        head         = 0;        // Head dimulai dari sel paling kiri
        currentState = Q0_SCAN;  // State awal
        step         = 0;
    }

    // ─────────────────────────────────────────────────────────────────
    //  FUNGSI TRANSISI UTAMA
    //  Merupakan jantung Mesin Turing; mendefinisikan aturan:
    //      delta(state, simbol_dibaca) -> (state_baru, simbol_tulis, arah)
    //
    //  Alur sanitasi:
    //    Q0_SCAN   : Pindai pita. Temukan '0'/'1' -> masuk Q1_ERASE.
    //                Temukan 'B' di awal -> langsung verifikasi (Q2_VERIFY).
    //    Q1_ERASE  : Hapus (overwrite) '0'/'1' menjadi 'B', gerak kanan.
    //                Temukan 'B' -> semua data telah dihapus, masuk Q2_VERIFY.
    //    Q2_VERIFY : Balik ke kiri untuk kembali ke awal pita,
    //                lalu konfirmasi bersih -> masuk Q_ACCEPT.
    // ─────────────────────────────────────────────────────────────────
    void transition() {
        step++;
        char sym = read();
        string action;

        switch (currentState) {

            // ── Q0_SCAN: Mulai memindai pita dari kiri ───────────
            case Q0_SCAN:
                if (sym == SYM_ZERO || sym == SYM_ONE) {
                    // Ditemukan data biner -> mulai fase penghapusan
                    action = "Simbol biner ditemukan. Transisi ke Q1_ERASE untuk memulai sanitasi.";
                    currentState = Q1_ERASE;
                    // Tidak gerak, langsung masuk state ERASE di langkah berikutnya
                } else if (sym == SYM_BLANK) {
                    // Pita kosong sejak awal -> langsung verifikasi
                    action = "Pita sudah kosong (Blank). Transisi ke Q2_VERIFY.";
                    currentState = Q2_VERIFY;
                }
                printStep(action);
                break;

            // ── Q1_ERASE: Hapus (sanitasi) simbol biner ──────────
            case Q1_ERASE:
                if (sym == SYM_ZERO || sym == SYM_ONE) {
                    // Overwrite simbol biner dengan Blank (sanitasi)
                    action  = "Overwrite '";
                    action += sym;
                    action += "' -> 'B'. Gerak ke kanan (R).";
                    write(SYM_BLANK);  // Tulis 'B' menggantikan data biner
                    printStep(action);
                    move('R');         // Lanjut ke sel berikutnya
                } else if (sym == SYM_BLANK) {
                    // Seluruh data biner sudah dihapus, bertemu batas pita
                    action = "Batas pita (Blank) terdeteksi. Semua data terhapus. "
                             "Transisi ke Q2_VERIFY untuk konfirmasi.";
                    currentState = Q2_VERIFY;
                    printStep(action);
                    // Balikkan head ke posisi awal (kiri) untuk verifikasi
                    move('L');
                }
                break;

            // ── Q2_VERIFY: Verifikasi pita sudah bersih total ────
            case Q2_VERIFY:
                if (sym == SYM_BLANK && head == 0) {
                    // Head di posisi paling kiri, seluruh pita adalah 'B'
                    action = "Verifikasi selesai: Pita bersih total. Transisi ke Q_ACCEPT.";
                    currentState = Q_ACCEPT;
                    printStep(action);
                } else if (sym == SYM_BLANK && head > 0) {
                    // Masih ada sisa jalan ke kiri, terus mundur
                    action = "Mundur ke kiri untuk kembali ke posisi awal pita.";
                    printStep(action);
                    move('L');
                } else if (sym == SYM_ZERO || sym == SYM_ONE) {
                    // Ada data yang belum terhapus (seharusnya tidak terjadi di alur normal)
                    action = "PERINGATAN: Ditemukan data residual '";
                    action += sym;
                    action += "'. Kembali ke Q1_ERASE.";
                    currentState = Q1_ERASE;
                    printStep(action);
                }
                break;

            // ── Q_ACCEPT & Q_REJECT: State terminal ──────────────
            case Q_ACCEPT:
            case Q_REJECT:
                // Tidak ada transisi lebih lanjut dari state terminal
                break;
        }
    }

    // ── Jalankan mesin hingga mencapai state terminal ─────────
    void run() {
        cout << "\n";
        cout << "==================================================================\n";
        cout << "     MESIN TURING: SECURE DATA SANITIZATION SIMULATION\n";
        cout << "==================================================================\n\n";

        // Tampilkan kondisi awal
        cout << "[ KONDISI AWAL ]\n";
        cout << "  State Awal : " << stateName(currentState) << "\n";
        cout << "  Head       : posisi " << head << "\n";
        cout << "  Isi Pita   : ";
        for (char c : tape) cout << c << " ";
        cout << "\n\n";

        cout << "[ MULAI TRACE EKSEKUSI ]\n";
        cout << string(67, '-') << "\n";

        // Loop utama: jalankan transisi sampai Accept/Reject
        while (currentState != Q_ACCEPT && currentState != Q_REJECT) {
            transition();

            // Batasan pengaman agar tidak infinite loop
            if (step > 10000) {
                cout << "!! Batas maksimum langkah tercapai. Mesin dihentikan paksa.\n";
                break;
            }
        }

        // ── Tampilkan hasil akhir ─────────────────────────────
        cout << "\n[ HASIL AKHIR ]\n";
        cout << string(67, '-') << "\n";
        cout << "  State Akhir   : " << stateName(currentState) << "\n";
        cout << "  Total Langkah : " << step << "\n";
        cout << "  Isi Pita Akhir: ";
        for (char c : tape) cout << c << " ";
        cout << "\n\n";

        if (currentState == Q_ACCEPT) {
            cout << "  [OK] STATUS: SANITASI BERHASIL\n";
            cout << "       Seluruh data biner telah dihapus secara aman.\n";
            cout << "       Mesin mencapai Accept State (Q_ACCEPT).\n";
        } else {
            cout << "  [!!] STATUS: SANITASI GAGAL / TIDAK SELESAI\n";
        }
        cout << string(67, '-') << "\n";
    }
};

// ─────────────────────────────────────────────────────────────
//  MAIN PROGRAM
// ─────────────────────────────────────────────────────────────
int main() {
    cout << "==================================================================\n";
    cout << "          SECURE DATA SANITIZATION - MESIN TURING\n";
    cout << "==================================================================\n\n";

    string binaryData;
    cout << "Masukkan string biner (hanya '0' dan '1'): ";
    cin  >> binaryData;

    // Validasi input
    for (char c : binaryData) {
        if (c != '0' && c != '1') {
            cout << "ERROR: Input hanya boleh berisi '0' dan '1'.\n";
            return 1;
        }
    }

    // Buat dan jalankan Mesin Turing
    TuringMachine tm(binaryData);
    tm.run();

    return 0;
}
