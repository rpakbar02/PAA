#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

// ==========================================
// ALGORITMA NAIVE & KMP (CORE LOGIC)
// ==========================================

// 1. NAIVE
int naiveSearch(const string& text, const string& pattern) {
    int n = text.length();
    int m = pattern.length();
    int count = 0;
    
    // Loop ini akan sangat lambat jika terjadi banyak "partial match"
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j])
                break;
        }
        if (j == m) count++;
    }
    return count;
}

// 2. KMP HELPERS
void computeLPSArray(const string& pattern, int m, vector<int>& lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) len = lps[len - 1];
            else { lps[i] = 0; i++; }
        }
    }
}

// 3. KMP MAIN
int KMPSearch(const string& text, const string& pattern) {
    int n = text.length();
    int m = pattern.length();
    int count = 0;
    vector<int> lps(m);
    computeLPSArray(pattern, m, lps);
    
    int i = 0; 
    int j = 0; 
    while (i < n) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == m) {
            count++;
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) j = lps[j - 1];
            else i = i + 1;
        }
    }
    return count;
}

// ==========================================
// DATA GENERATION (DETERMINISTIC)
// ==========================================

int main() {
    // A. KONFIGURASI SIMULASI
    const int TEXT_LENGTH = 10000000; // 10 Juta karakter
    const int PATTERN_REPEAT_COUNT = 1000; // Panjang repetisi dalam pattern
    const string MOTIF = "CAG"; // Motif trinucleotide (Huntington's Disease style)
    
    cout << "=== DNA MATCHING: DETERMINISTIC SIMULATION ===" << endl;
    
    // B. MEMBUAT PATTERN (TARGET)
    // Pattern: "CAG" diulang 1000 kali + "T" di ujung.
    // Struktur ini "jahat" untuk Naive karena 99.9% pattern-nya cocok dengan background noise.
    string pattern = "";
    pattern.reserve((MOTIF.length() * PATTERN_REPEAT_COUNT) + 1);
    for(int i = 0; i < PATTERN_REPEAT_COUNT; i++) {
        pattern += MOTIF;
    }
    pattern += "T"; // Karakter pembeda di ujung akhir
    
    cout << "1. Pattern Created." << endl;
    cout << "   Structure : (" << MOTIF << " x " << PATTERN_REPEAT_COUNT << ") + 'T'" << endl;
    cout << "   Length    : " << pattern.length() << " bases" << endl;

    // C. MEMBUAT TEXT (BACKGROUND NOISE)
    // Kita isi text dengan "CAG" terus menerus.
    // Ini mensimulasikan area STR (Short Tandem Repeat) yang sangat panjang.
    cout << "2. Generating High-Repetition Genomic Text (" << TEXT_LENGTH << " bases)... ";
    string text;
    text.reserve(TEXT_LENGTH);
    while (text.length() < TEXT_LENGTH) {
        text += MOTIF;
    }
    // Potong agar pas ukurannya
    text = text.substr(0, TEXT_LENGTH);
    cout << "Done." << endl;

    // D. INJECT MATCHES (SUNTIKAN)
    // Kita paksa pattern masuk ke dalam text di posisi tertentu.
    // Kita masukkan 3 match: Awal, Tengah, Akhir.
    
    vector<int> injectionPoints = {
        100000,             // Dekat awal
        TEXT_LENGTH / 2,    // Tengah
        TEXT_LENGTH - (int)pattern.length() - 1000 // Dekat akhir
    };

    cout << "3. Injecting " << injectionPoints.size() << " exact matches into text..." << endl;
    for (int pos : injectionPoints) {
        // Replace isi text dengan pattern kita di posisi tersebut
        text.replace(pos, pattern.length(), pattern);
        cout << "   -> Match injected at index: " << pos << endl;
    }
    cout << endl;

    // ==========================================
    // EKSEKUSI DAN PENGUKURAN
    // ==========================================

    // RUN NAIVE
    cout << "[RUNNING NAIVE ALGORITHM]" << endl;
    auto startNaive = high_resolution_clock::now();
    
    int matchesNaive = naiveSearch(text, pattern);
    
    auto stopNaive = high_resolution_clock::now();
    auto durationNaive = duration_cast<milliseconds>(stopNaive - startNaive);
    cout << "   Result : " << matchesNaive << " matches found." << endl;
    cout << "   Time   : " << durationNaive.count() << " ms" << endl << endl;

    // RUN KMP
    cout << "[RUNNING KMP ALGORITHM]" << endl;
    auto startKMP = high_resolution_clock::now();
    
    int matchesKMP = KMPSearch(text, pattern);
    
    auto stopKMP = high_resolution_clock::now();
    auto durationKMP = duration_cast<milliseconds>(stopKMP - startKMP);
    cout << "   Result : " << matchesKMP << " matches found." << endl;
    cout << "   Time   : " << durationKMP.count() << " ms" << endl << endl;

    // ==========================================
    // KESIMPULAN LOGIS
    // ==========================================
    cout << "=== FINAL ANALYSIS ===" << endl;
    if (matchesNaive != injectionPoints.size() || matchesKMP != injectionPoints.size()) {
        cout << "ERROR: Algoritma gagal menemukan jumlah match yang benar!" << endl;
    } else {
        cout << "Validation: Both algorithms found exactly " << injectionPoints.size() << " matches." << endl;
        
        long naiveTime = durationNaive.count();
        long kmpTime = durationKMP.count();
        
        // Hindari pembagian nol (jika PC terlalu cepat)
        if (kmpTime == 0) kmpTime = 1; 
        
        cout << "Performance Ratio: KMP is " << (float)naiveTime / kmpTime << "x faster." << endl;
        cout << "Why? Because the text consists of repeated '" << MOTIF << "'." << endl;
        cout << "Naive checks every single repetition unnecessarily." << endl;
        cout << "KMP uses the LPS table to skip aligned repetitions." << endl;
    }

    return 0;
}