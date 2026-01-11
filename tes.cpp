#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <sys/resource.h> // Khusus Linux
#include <iomanip>

using namespace std;
using namespace std::chrono;

// --- STRUKTUR DATA HASIL ---
struct AnalysisResult {
    string algorithm;
    long long comparisons;
    double duration; // ms
    int matches;
    
    // Rincian Memori Teoritis (Bytes)
    size_t inputMem;
    size_t lpsMem;
    size_t stackMem;
    size_t totalMem;
};

// --- FUNGSI HELPER ---

// 1. Cek Memori Real (OS Level - RSS)
long getPeakRSS() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // Kilobytes (KB)
}

// 2. Hitung Memori String Teoritis
size_t getStringMemory(const string& s) {
    return sizeof(string) + (s.capacity() * sizeof(char));
}

// ==========================================
// 1. ALGORITMA NAIVE
// ==========================================
AnalysisResult naiveSearch(const string& text, const string& pattern) {
    long long comparisons = 0;
    int matches = 0;
    int n = text.length();
    int m = pattern.length();

    // -- Perhitungan Memori Teoritis --
    size_t inputMem = getStringMemory(text) + getStringMemory(pattern);
    size_t lpsMem = 0; // Naive tidak punya tabel LPS
    size_t stackMem = (5 * sizeof(int)) + sizeof(long long); // i, j, n, m, matches, comparisons
    size_t totalMem = inputMem + stackMem;

    auto start = high_resolution_clock::now();

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            comparisons++;
            if (text[i + j] != pattern[j]) break;
        }
        if (j == m) matches++;
    }

    auto stop = high_resolution_clock::now();
    double duration = duration_cast<milliseconds>(stop - start).count();

    return {"Naive", comparisons, duration, matches, inputMem, lpsMem, stackMem, totalMem};
}

// ==========================================
// 2. ALGORITMA KMP
// ==========================================
void computeLPSArray(const string& pattern, int m, vector<int>& lps, long long& comparisons) {
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
            if (len != 0) len = lps[len - 1];
            else { lps[i] = 0; i++; }
        }
    }
}

AnalysisResult KMPSearch(const string& text, const string& pattern) {
    long long comparisons = 0;
    int matches = 0;
    int n = text.length();
    int m = pattern.length();

    auto start = high_resolution_clock::now();

    // Alokasi LPS
    vector<int> lps(m);
    computeLPSArray(pattern, m, lps, comparisons); // Preprocessing cost count

    int i = 0; 
    int j = 0; 
    while (i < n) {
        comparisons++;
        if (pattern[j] == text[i]) { j++; i++; }
        if (j == m) {
            matches++;
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) j = lps[j - 1];
            else i = i + 1;
        }
    }

    auto stop = high_resolution_clock::now();
    double duration = duration_cast<milliseconds>(stop - start).count();

    // -- Perhitungan Memori Teoritis --
    size_t inputMem = getStringMemory(text) + getStringMemory(pattern);
    size_t lpsMem = sizeof(vector<int>) + (lps.capacity() * sizeof(int)); // Memori Array LPS
    size_t stackMem = (5 * sizeof(int)) + sizeof(long long);
    size_t totalMem = inputMem + lpsMem + stackMem;

    return {"KMP", comparisons, duration, matches, inputMem, lpsMem, stackMem, totalMem};
}

// ==========================================
// MAIN PROGRAM
// ==========================================
int main() {
    // SAYA UBAH KE 10 JUTA AGAR LAPTOP ANDA TIDAK HANG.
    // Jika berani, ubah kembali ke 100000000 (100MB).
    const int TEXT_LENGTH = 100000; 
    const int PATTERN_REPEAT_COUNT = 1000; 
    const string MOTIF = "CAG"; 
    
    cout << "=== DNA MATCHING: MEMORY & TIME ANALYSIS ===" << endl;
    cout << "Generasi Data Random (" << (TEXT_LENGTH / 1000000) << " Juta Karakter)... Mohon tunggu." << endl;

    // 1. Cek System RAM Awal
    long memStart = getPeakRSS();

    // Membuat Pattern
    string pattern = "";
    pattern.reserve(PATTERN_REPEAT_COUNT * 3 + 1);
    for(int i = 0; i < PATTERN_REPEAT_COUNT; i++) pattern += MOTIF;
    pattern += "T"; 

    // Membuat Text
    string text;
    text.reserve(TEXT_LENGTH);
    while (text.length() < TEXT_LENGTH) text += MOTIF;
    text = text.substr(0, TEXT_LENGTH);

    // 2. Cek System RAM Setelah Load Data
    long memLoaded = getPeakRSS();
    
    cout << "\n[1] SYSTEM MEMORY STATUS (OS VIEW - RSS)" << endl;
    cout << "----------------------------------------" << endl;
    cout << "RSS Before Load : " << memStart << " KB" << endl;
    cout << "RSS After Load  : " << memLoaded << " KB" << endl;
    cout << "Memory Increase : " << (memLoaded - memStart) << " KB (Fakta di RAM)" << endl;

    cout << "\n[2] ALGORITHM PERFORMANCE ANALYSIS" << endl;
    cout << "================================================================================================" << endl;
    cout << left << setw(8) << "Algo" 
         << setw(15) << "Comparisons" 
         << setw(10) << "Time(ms)" 
         << "| "
         << setw(12) << "Input(Byte)" 
         << setw(12) << "LPS(Byte)" 
         << setw(12) << "Stack(Byte)" 
         << setw(12) << "TOTAL(Byte)" << endl;
    cout << "------------------------------------------------------------------------------------------------" << endl;

    // --- EXECUTE NAIVE ---
    AnalysisResult resNaive = naiveSearch(text, pattern);
    cout << left << setw(8) << resNaive.algorithm 
         << setw(15) << resNaive.comparisons 
         << setw(10) << resNaive.duration 
         << "| "
         << setw(12) << resNaive.inputMem 
         << setw(12) << resNaive.lpsMem 
         << setw(12) << resNaive.stackMem 
         << setw(12) << resNaive.totalMem << endl;

    // --- EXECUTE KMP ---
    AnalysisResult resKMP = KMPSearch(text, pattern);
    cout << left << setw(8) << resKMP.algorithm 
         << setw(15) << resKMP.comparisons 
         << setw(10) << resKMP.duration 
         << "| "
         << setw(12) << resKMP.inputMem 
         << setw(12) << resKMP.lpsMem 
         << setw(12) << resKMP.stackMem 
         << setw(12) << resKMP.totalMem << endl;

    cout << "------------------------------------------------------------------------------------------------" << endl;
    
    // Kesimpulan Logis
    cout << "\nKESIMPULAN LOGIS:" << endl;
    long diff = resKMP.totalMem - resNaive.totalMem;
    cout << "1. KMP menggunakan " << diff << " Bytes lebih banyak daripada Naive." << endl;
    cout << "   (Ini adalah biaya memori array LPS sepanjang " << pattern.length() << " integer)." << endl;
    
    if (resKMP.duration < resNaive.duration) {
        cout << "2. Namun, KMP jauh lebih cepat. Trade-off memori kecil ini SANGAT LAYAK." << endl;
    } else {
        cout << "2. Kecepatan seimbang (karena pola repetitif), tapi KMP tetap lebih aman untuk Worst Case." << endl;
    }

    return 0;
}