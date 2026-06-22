// Simulasi Mesin Turing: Secure Data Sanitization (Tape Eraser)
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

const char SYM_ZERO  = '0';
const char SYM_ONE   = '1';
const char SYM_BLANK = 'B';

enum State {
    Q0_SCAN,
    Q1_ERASE,
    Q2_VERIFY,
    Q_ACCEPT,
    Q_REJECT
};

// Mengubah enum State menjadi string untuk keperluan log
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

class TuringMachine {
private:
    vector<char> tape;
    int   head;
    State currentState;
    int   step;

    // Mencetak isi pita, posisi head ditandai dengan kurung siku [ ]
    void printTape() const {
        cout << "  Tape : ";
        for (int i = 0; i < (int)tape.size(); i++) {
            if (i == head) cout << "[" << tape[i] << "]";
            else cout << " " << tape[i] << " ";
        }
        cout << "\n";
    }

    // Mencetak satu baris log eksekusi (trace)
    void printStep(const string& action, State oldState, char oldSym) const {
        cout << "+-- Step " << right << setw(3) << step
             << " | State: " << left << setw(10)
             << stateName(oldState) << "| Head[" << head << "] = '"
             << oldSym << "'\n";
        cout << "|  Aksi : " << action << "\n";
        printTape();
        cout << "+" << string(65, '-') << "\n";
    }

    char read() const {
        return tape[head];
    }

    void write(char sym) {
        tape[head] = sym;
    }

    // Menggerakkan head dan memperluas pita jika mencapai ujung kanan
    void move(char direction) {
        if (direction == 'R') {
            head++;
            if (head >= (int)tape.size()) tape.push_back(SYM_BLANK);
        } else if (direction == 'L') {
            if (head > 0) head--;
        }
    }

public:
    TuringMachine(const string& binaryInput) {
        for (char c : binaryInput) tape.push_back(c);
        tape.push_back(SYM_BLANK);

        head         = 0;
        currentState = Q0_SCAN;
        step         = 0;
    }

    // Fungsi utama transisi Mesin Turing (Scan -> Erase -> Verify)
    void transition() {
        step++;
        char sym = read();
        State oldState = currentState;
        string action;

        switch (currentState) {
            case Q0_SCAN:
                if (sym == SYM_ZERO || sym == SYM_ONE) {
                    action = "Simbol biner ditemukan. Transisi ke Q1_ERASE untuk memulai sanitasi.";
                    currentState = Q1_ERASE;
                } else if (sym == SYM_BLANK) {
                    action = "Pita sudah kosong (Blank). Transisi ke Q2_VERIFY.";
                    currentState = Q2_VERIFY;
                }
                printStep(action, oldState, sym);
                break;

            case Q1_ERASE:
                if (sym == SYM_ZERO || sym == SYM_ONE) {
                    action  = "Overwrite '";
                    action += sym;
                    action += "' -> 'B'. Gerak ke kanan (R).";
                    write(SYM_BLANK);
                    printStep(action, oldState, sym);
                    move('R');
                } else if (sym == SYM_BLANK) {
                    action = "Batas pita terdeteksi. Semua data terhapus. Transisi ke Q2_VERIFY.";
                    currentState = Q2_VERIFY;
                    printStep(action, oldState, sym);
                    move('L');
                }
                break;

            case Q2_VERIFY:
                if (sym == SYM_BLANK && head == 0) {
                    action = "Verifikasi selesai: Pita bersih total. Transisi ke Q_ACCEPT.";
                    currentState = Q_ACCEPT;
                    printStep(action, oldState, sym);
                } else if (sym == SYM_BLANK && head > 0) {
                    action = "Mundur ke kiri untuk kembali ke posisi awal pita.";
                    printStep(action, oldState, sym);
                    move('L');
                } else if (sym == SYM_ZERO || sym == SYM_ONE) {
                    action = "PERINGATAN: Ditemukan data residual '";
                    action += sym;
                    action += "'. Kembali ke Q1_ERASE.";
                    currentState = Q1_ERASE;
                    printStep(action, oldState, sym);
                }
                break;

            case Q_ACCEPT:
            case Q_REJECT:
                break;
        }
    }

    // Menjalankan mesin turing hingga selesai
    void run() {
        cout << "\n==================================================================\n";
        cout << "     MESIN TURING: SECURE DATA SANITIZATION SIMULATION\n";
        cout << "==================================================================\n\n";

        cout << "[ KONDISI AWAL ]\n";
        cout << "  State Awal : " << stateName(currentState) << "\n";
        cout << "  Head       : posisi " << head << "\n";
        cout << "  Isi Pita   : ";
        for (char c : tape) cout << c << " ";
        cout << "\n\n";

        cout << "[ MULAI TRACE EKSEKUSI ]\n";
        cout << string(67, '-') << "\n";

        while (currentState != Q_ACCEPT && currentState != Q_REJECT) {
            transition();
            if (step > 10000) {
                cout << "!! Batas maksimum langkah tercapai. Mesin dihentikan paksa.\n";
                break;
            }
        }

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

int main() {
    cout << "==================================================================\n";
    cout << "          SECURE DATA SANITIZATION - MESIN TURING\n";
    cout << "==================================================================\n\n";

    string binaryData;
    cout << "Masukkan string biner (hanya '0' dan '1'): ";
    cin  >> binaryData;

    for (char c : binaryData) {
        if (c != '0' && c != '1') {
            cout << "ERROR: Input hanya boleh berisi '0' dan '1'.\n";
            return 1;
        }
    }

    TuringMachine tm(binaryData);
    tm.run();

    return 0;
}
