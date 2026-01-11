#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <sys/resource.h> // Untuk getrusage (Linux)

using namespace std;
using namespace std::chrono;

// --- Fungsi Helper Memori (Khusus Linux/Ubuntu) ---
long getPeakRSS() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // Nilai dalam Kilobytes (KB)
}

// ==========================================
// ALGORITMA NAIVE & KMP (CORE LOGIC)
// ==========================================

int naiveSearch(const string& text, const string& pattern) {
    int n = text.length();
    int m = pattern.length();
    int count = 0;
    // Auxiliary Space: O(1)
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j]) break;
        }
        if (j == m) count++;
    }
    return count;
}

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

int KMPSearch(const string& text, const string& pattern, size_t& extraMemKMP) {
    int n = text.length();
    int m = pattern.length();
    int count = 0;
    
    // Alokasi tabel LPS
    vector<int> lps(m);
    extraMemKMP = lps.size() * sizeof(int); // Hitung memori tabel LPS
    
    computeLPSArray(pattern, m, lps);
    
    int i = 0; 
    int j = 0; 
    while (i < n) {
        if (pattern[j] == text[i]) { j++; i++; }
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

int main() {
    const int TEXT_LENGTH = 10000000; 
    const int PATTERN_REPEAT_COUNT = 1000; 
    const string MOTIF = "CAG"; 
    
    cout << "=== DNA MATCHING: MEMORY & TIME ANALYSIS ===" << endl;
    
    // 1. Cek Memori Sebelum Data Dibuat
    long memBefore = getPeakRSS();

    string pattern = "";
    for(int i = 0; i < PATTERN_REPEAT_COUNT; i++) pattern += MOTIF;
    pattern += "T"; 

    string text;
    text.reserve(TEXT_LENGTH);
    while (text.length() < TEXT_LENGTH) text += MOTIF;
    text = text.substr(0, TEXT_LENGTH);

    // 2. Cek Memori Setelah String Dimuat
    long memAfterData = getPeakRSS();
    
    cout << "Input Analysis:" << endl;
    cout << "  - Text Size    : " << (float)text.length() / (1024*1024) << " MB" << endl;
    cout << "  - Pattern Size : " << (float)pattern.length() / 1024 << " KB" << endl;
    cout << "  - System RSS   : " << memAfterData << " KB" << endl << endl;

    // --- EKSEKUSI NAIVE ---
    auto startNaive = high_resolution_clock::now();
    int matchesNaive = naiveSearch(text, pattern);
    auto stopNaive = high_resolution_clock::now();
    
    // --- EKSEKUSI KMP ---
    size_t extraMemKMP = 0;
    auto startKMP = high_resolution_clock::now();
    int matchesKMP = KMPSearch(text, pattern, extraMemKMP);
    auto stopKMP = high_resolution_clock::now();

    // ==========================================
    // HASIL ANALISIS
    // ==========================================
    cout << "--- Performance Comparison ---" << endl;
    cout << "Naive Algorithm:" << endl;
    cout << "  - Time         : " << duration_cast<milliseconds>(stopNaive - startNaive).count() << " ms" << endl;
    cout << "  - Extra Memory : 0 bytes (In-place)" << endl;

    cout << "\nKMP Algorithm:" << endl;
    cout << "  - Time         : " << duration_cast<milliseconds>(stopKMP - startKMP).count() << " ms" << endl;
    cout << "  - Extra Memory : " << extraMemKMP << " bytes (LPS Table)" << endl;
    cout << "    (Pattern Length " << pattern.length() << " * " << sizeof(int) << " bytes)" << endl << endl;

    cout << "Summary:" << endl;
    cout << "KMP membutuhkan " << extraMemKMP << " bytes memori tambahan untuk mempercepat pencarian." << endl;
    
    return 0;
}