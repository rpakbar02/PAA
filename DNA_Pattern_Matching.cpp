#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>

using namespace std;

struct AnalysisResult {
    string algorithm;
    long long comparisons;
    double duration;
    int matches;
    
    size_t inputMem;
    size_t lpsMem;   
    size_t stackMem; 
    size_t totalMem;
};

size_t getStringMemory(const string& s) {

    return sizeof(string) + (s.capacity() * sizeof(char));

}

AnalysisResult naiveSearch(const string& text, const string& pattern) {
    long long comparisons = 0;
    int matches = 0;
    int n = text.length();
    int m = pattern.length();

    size_t inputMem = getStringMemory(text) + getStringMemory(pattern);
    size_t lpsMem = 0;

    // 3. Stack Memory: Variabel lokal (n, m, matches, comparisons, i, j)
    //    3 int (n, m, matches) + 1 long long (comparisons) + 2 int iterator (i, j)
    size_t stackMem = (5 * sizeof(int)) + sizeof(long long);

    size_t totalMem = inputMem + lpsMem + stackMem;

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

    return {"Naive", comparisons, elapsed.count(), matches, inputMem, lpsMem, stackMem, totalMem};
}

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

    vector<int> lps = computeLPS(pattern, comparisons);

    int i = 0; 
    int j = 0; 
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

    size_t inputMem = getStringMemory(text) + getStringMemory(pattern);
    size_t lpsMem = sizeof(vector<int>) + (lps.capacity() * sizeof(int));
    size_t stackMem = (5 * sizeof(int)) + sizeof(long long);
    size_t totalMem = inputMem + lpsMem + stackMem;

    return {"KMP", comparisons, elapsed.count(), matches, inputMem, lpsMem, stackMem, totalMem};
}

void runAnalysis(int limit) {
    ifstream file("human.txt"); 
    if (!file.is_open()) {
        cout << "Error: File human.txt tidak ditemukan!" << endl;
        return;
    }

    string pattern = "ATGTGTGGCATTTGGGCGCTGTTTGGCAGTGATGATTGCCTTTCTGTTCAGTGTCTGAGTGCTATGAAGATTGCACACAGAGGTCCAGATGCATTCCGTTTTGAGAATGTCAATGGATACACCAACTGCTGCTTTGGATTTCACCGGTTGGCGGTAGTTGACCCGCTGTTTGGAATGCAGCCAATTCGAGTGAAGAAATATCCGTATTTGTGGCTCTGTTACAATGGTGAAATCTACAACCATAAGAAGATGCAACAGCATTTTGAATTTGAATACCAGACCAAAGTGGATGGTGAGATAATCCTTCATCTTTATGACAAAGGAGGAATTGAGCAAACAATTTGTATGTTGGATGGTGTGTTTGCATTTGTTTTACTGGATACTGCCAATAAGAAAGTGTTCCTGGGTAGAGATACATATGGAGTCAGACCTTTGTTTAAAGCAATGACAGAAGATGGATTTTTGGCTGTATGTTCAGAAGCTAAAGGTCTTGTTACATTGAAGCACTCCGCGACTCCCTTTTTAAAAGTGGAGCCTTTTCTTCCTGGACACTATGAAGTTTTGGATTTAAAGCCAAATGGCAAAGTTGCATCCGTGGAAATGGTTAAATATCATCACTGTCGGGATGTACCCCTGCACGCCCTCTATGACAATGTGGAGAAACTCTTTCCAGGTTTTGAGATAGAAACTGTGAAGAACAACCTCAGGATCCTTTTTAATAATGCTGTAAAGAAACGTTTGATGACAGACAGAAGGATTGGCTGCCTTTTATCAGGGGGCTTGGACTCCAGCTTGGTTGCTGCCACTCTGTTGAAGCAGCTGAAAGAAGCCCAAGTACAGTATCCTCTCCAGACATTTGCAATTGGCATGGAAGACAGCCCCGATTTACTGGCTGCTAGAAAGGTGGCAGATCATATTGGAAGTGAACATTATGAAGTCCTTTTTAACTCTGAGGAAGGCATTCAGGCTCTGGATGAAGTCATATTTTCCTTGGAAACTTATGACATTACAACAGTTCGTGCTTCAGTAGGTATGTATTTAATTTCCAAGTATATTCGGAAGAACACAGATAGCGTGGTGATCTTCTCTGGAGAAGGATCAGATGAACTTACGCAGGGTTACATATATTTTCACAAGGCTCCTTCTCCTGAAAAAGCCGAGGAGGAGAGTGAGAGGCTTCTGAGGGAACTCTATTTGTTTGATGTTCTCCGCGCAGATCGAACTACTGCTGCCCATGGTCTTGAACTGAGAGTCCCATTTCTAGATCATCGATTTTCTTCCTATTACTTGTCTCTGCCACCAGAAATGAGAATTCCAAAGAATGGGATAGAAAAACATCTCCTGAGAGAGACGTTTGAGGATTCCAATCTGATACCCAAAGAGATTCTCTGGCGACCAAAAGAAGCCTTCAGTGATGGAATAACTTCAGTTAAGAATTCCTGGTTTAAGATTTTACAGGAATACGTTGAACATCAGGTTGATGATGCAATGATGGCAAATGCAGCCCAGAAATTTCCCTTCAATACTCCTAAAACCAAAGAAGGATATTACTACCGTCAAGTCTTTGAACGCCATTACCCAGGCCGGGCTGACTGGCTGAGCCATTACTGGATGCCCAAGTGGATCAATGCCACTGACCCTTCTGCCCGCACGCTGACCCACTACAAGTCAGCTGTCAAAGCTTAG"; 
    string dna;
    int dnaClass;
    int processedCount = 0;

    cout << fixed << setprecision(4);
    cout << "\nANALISIS DETAIL MEMORI (Satuan: Byte)" << endl;
    cout << "========================================================================================================================================" << endl;
    
    cout << left << setw(4) << "No" 
         << setw(7) << "Class" 
         << setw(8) << "Algo" 
         << setw(12) << "Comp." 
         << setw(10) << "Time(ms)" 
         << setw(8) << "Match" 
         << "| "
         << setw(10) << "InputMem" 
         << setw(10) << "LPS Mem" 
         << setw(10) << "StackMem" 
         << setw(12) << "TOTAL MEM" << endl;
         
    cout << "----------------------------------------------------------------------------------------------------------------------------------------" << endl;

    while (file >> dna >> dnaClass && processedCount < limit) {
        processedCount++;

        AnalysisResult resNaive = naiveSearch(dna, pattern);
        AnalysisResult resKMP = kmpSearch(dna, pattern);

        cout << left << setw(4) << processedCount 
             << setw(7) << dnaClass 
             << setw(8) << resNaive.algorithm 
             << setw(12) << resNaive.comparisons 
             << setw(10) << resNaive.duration 
             << setw(8) << resNaive.matches 
             << "| "
             << setw(10) << resNaive.inputMem 
             << setw(10) << resNaive.lpsMem 
             << setw(10) << resNaive.stackMem 
             << setw(12) << resNaive.totalMem << endl;

        cout << left << setw(4) << "" 
             << setw(7) << dnaClass 
             << setw(8) << resKMP.algorithm 
             << setw(12) << resKMP.comparisons 
             << setw(10) << resKMP.duration 
             << setw(8) << resKMP.matches 
             << "| "
             << setw(10) << resKMP.inputMem 
             << setw(10) << resKMP.lpsMem 
             << setw(10) << resKMP.stackMem 
             << setw(12) << resKMP.totalMem << endl;
        
        cout << "----------------------------------------------------------------------------------------------------------------------------------------" << endl;
    }

    file.close();
    if (processedCount == 0) cout << "File kosong atau format salah." << endl;
}

int main() {
    int limit;
    cout << "--- DNA Matching Memory Analysis ---" << endl;
    cout << "Masukkan jumlah sekuens yang ingin dicek: ";
    cin >> limit;

    runAnalysis(limit);

    cout << "\nKeterangan:" << endl;
    cout << "- InputMem : Memori untuk menyimpan teks DNA dan pola pencarian." << endl;
    cout << "- LPS Mem  : Memori tambahan array (Longest Prefix Suffix) pada KMP." << endl;
    cout << "- StackMem : Estimasi memori variabel lokal (int, iterator, dll)." << endl;
    
    return 0;
}