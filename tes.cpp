#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>

using namespace std;

// Struktur untuk menyimpan hasil analisis untuk laporan
struct AnalysisResult {
    string algorithm;
    long long comparisons;
    double duration;
    int matches;
};

// ==========================================
// 1. ALGORITMA NAIVE (BRUTE FORCE)
// ==========================================
AnalysisResult naiveSearch(const string& text, const string& pattern) {
    long long comparisons = 0;
    int matches = 0;
    int n = text.length();
    int m = pattern.length();

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            comparisons++; 
            if (text[i + j] != pattern[j])
                break;
        }
        if (j == m) matches++;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;

    return {"Naive", comparisons, elapsed.count(), matches};
}

// ==========================================
// 2. ALGORITMA KNUTH-MORRIS-PRATT (KMP)
// ==========================================

vector<int> computeLPS(const string& pattern, long long& comparisons) {
    int m = pattern.length();
    vector<int> lps(m);
    int len = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m) {
        comparisons++; 
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

AnalysisResult kmpSearch(const string& text, const string& pattern) {
    long long comparisons = 0;
    int matches = 0;
    int n = text.length();
    int m = pattern.length();

    auto start = chrono::high_resolution_clock::now();

    // Fase Preprocessing (LPS Table)
    vector<int> lps = computeLPS(pattern, comparisons);

    // Fase Matching
    int i = 0; // index untuk text
    int j = 0; // index untuk pattern
    while (i < n) {
        comparisons++;
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            matches++;
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;

    return {"KMP", comparisons, elapsed.count(), matches};
}

// ==========================================
// FUNGSI ANALISIS DATASET
// ==========================================
void runAnalysis(int limit) {
    ifstream file("human.txt"); // Membaca file di folder yang sama
    if (!file.is_open()) {
        cout << "Error: File human.txt tidak ditemukan!" << endl;
        return;
    }

    string pattern = "CAG"; // Pola target Huntington
    string dna;
    int dnaClass;
    int processedCount = 0;

    cout << fixed << setprecision(5);
    cout << "\nANALISIS PERBANDINGAN ALGORITMA (Pola: " << pattern << ")" << endl;
    cout << "==========================================================================================" << endl;
    cout << left << setw(8) << "No" << setw(10) << "Class" << setw(12) << "Algo" << setw(18) << "Comparisons" << setw(15) << "Time(ms)" << setw(10) << "Matches" << endl;
    cout << "------------------------------------------------------------------------------------------" << endl;

    // Membaca pasangan: [String DNA] [Angka Class]
    while (file >> dna >> dnaClass && processedCount < limit) {
        processedCount++;

        // Jalankan Naive
        AnalysisResult resNaive = naiveSearch(dna, pattern);
        // Jalankan KMP
        AnalysisResult resKMP = kmpSearch(dna, pattern);

        // Baris Naive
        cout << left << setw(8) << processedCount 
             << setw(10) << dnaClass 
             << setw(12) << resNaive.algorithm 
             << setw(18) << resNaive.comparisons 
             << setw(15) << resNaive.duration 
             << setw(10) << resNaive.matches << endl;

        // Baris KMP
        cout << left << setw(8) << "" 
             << setw(10) << dnaClass 
             << setw(12) << resKMP.algorithm 
             << setw(18) << resKMP.comparisons 
             << setw(15) << resKMP.duration 
             << setw(10) << resKMP.matches << endl;
        
        cout << "------------------------------------------------------------------------------------------" << endl;
    }

    file.close();
    if (processedCount == 0) cout << "File kosong atau format salah." << endl;
}

int main() {
    int limit;
    cout << "--- DNA Matching Huntington Analysis ---" << endl;
    cout << "Masukkan jumlah sekuens yang ingin dicek: ";
    cin >> limit;

    runAnalysis(limit);

    cout << "\nAnalisis Selesai. Gunakan data di atas untuk tabel laporan IEEE Anda." << endl;
    return 0;
}