#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;




void selection_sort(int* values, int size) {
    for (int i = 0; i < size - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < size; j++) {
            if (values[j] < values[minIdx])
                minIdx = j;
        }
        if (minIdx != i) {
            int tmp = values[i];
            values[i] = values[minIdx];
            values[minIdx] = tmp;
        }
    }
}

bool binary_search_recursive(int* values, int key, int start, int end) {
    if (start > end) return false;
    int mid = start + (end - start) / 2;
    if (values[mid] == key) return true;
    if (key < values[mid])  return binary_search_recursive(values, key, start, mid - 1);
    return binary_search_recursive(values, key, mid + 1, end);
}

bool binary_search(int* values, int key, int size) {
    return binary_search_recursive(values, key, 0, size - 1);
}

void writeBinary(string filename, int* values, int length) {
    ofstream out(filename, ios::binary);
    out.write(reinterpret_cast<char*>(&length), sizeof(int));
    out.write(reinterpret_cast<char*>(values), sizeof(int) * length);
    out.close();
}

void createBinaryFile(string filename) {
    srand(42);
    int size = 50;
    int* values = new int[size];
    for (int i = 0; i < size; i++)
        values[i] = rand() % 1000;
    writeBinary(filename, values, size);
    delete[] values;
}

// BinaryReader

class BinaryReader {
    int* m_values;
    int  m_size;
public:
    BinaryReader(string filename) : m_values(nullptr), m_size(0) {
        readValues(filename);
    }
    ~BinaryReader() { delete[] m_values; }
    int* getValues() { return m_values; }
    int  getSize() { return m_size; }
    void readValues(string filename) {
        ifstream in(filename, ios::binary);
        in.read(reinterpret_cast<char*>(&m_size), sizeof(int));
        m_values = new int[m_size];
        in.read(reinterpret_cast<char*>(m_values), sizeof(int) * m_size);
        in.close();
    }
};

// Analyzer

class Analyzer {
protected:
    int* m_values;
    int  m_size;
public:
    Analyzer(int* values, int size) : m_size(size) {
        m_values = cloneValues(values, size);
    }
    ~Analyzer() { delete[] m_values; }
    int* cloneValues(int* src, int size) {
        int* copy = new int[size];
        for (int i = 0; i < size; i++) copy[i] = src[i];
        return copy;
    }
    virtual string analyze() = 0;
};

// Duplicates Analyser

class DuplicatesAnalyser : public Analyzer {
public:
    DuplicatesAnalyser(int* values, int size) : Analyzer(values, size) {}
    string analyze() override {
        int count = 0;
        for (int i = 0; i < m_size - 1; i++)
            for (int j = i + 1; j < m_size; j++)
                if (m_values[i] == m_values[j]) { count++; break; }
        ostringstream oss;
        oss << "Duplicates: " << count;
        return oss.str();
    }
};


// Missing Analyser


class MissingAnalyser : public Analyzer {
public:
    MissingAnalyser(int* values, int size) : Analyzer(values, size) {}
    string analyze() override {
        // Count values in 0-999 not present in the array
        int missing = 0;
        for (int v = 0; v < 1000; v++) {
            bool found = false;
            for (int i = 0; i < m_size; i++)
                if (m_values[i] == v) { found = true; break; }
            if (!found) missing++;
        }
        ostringstream oss;
        oss << "Missing values (0-999): " << missing;
        return oss.str();
    }
};


// Search Analyzer


class SearchAnalyzer : public Analyzer {
public:
    SearchAnalyzer(int* values, int size) : Analyzer(values, size) {
        selection_sort(m_values, m_size);
    }
    string analyze() override {
        srand(static_cast<unsigned>(time(nullptr)));
        int found = 0;
        for (int i = 0; i < 100; i++) {
            int key = rand() % 1000;
            if (binary_search(m_values, key, m_size))
                found++;
        }
        ostringstream oss;
        oss << "Search found: " << found << " out of 100 random values";
        return oss.str();
    }
};


// Statistics Analyser


class StatisticsAnalyser : public Analyzer {
public:
    StatisticsAnalyser(int* values, int size) : Analyzer(values, size) {}
    string analyze() override {
        selection_sort(m_values, m_size);

        // Min and max are easy with sorted data
        int minVal = m_values[0];
        int maxVal = m_values[m_size - 1];

        // Mean
        double sum = 0;
        for (int i = 0; i < m_size; i++) sum += m_values[i];
        double mean = sum / m_size;

        // Median
        double median;
        if (m_size % 2 != 0)
            median = m_values[m_size / 2];
        else
            median = (m_values[m_size / 2 - 1] + m_values[m_size / 2]) / 2.0;

        // Mode (first most-frequent value)
        int modeVal = m_values[0], modeCount = 1, curCount = 1;
        for (int i = 1; i < m_size; i++) {
            if (m_values[i] == m_values[i - 1]) {
                curCount++;
                if (curCount > modeCount) { modeCount = curCount; modeVal = m_values[i]; }
            }
            else {
                curCount = 1;
            }
        }

        ostringstream oss;
        oss << "Min: " << minVal
            << ", Max: " << maxVal
            << ", Mean: " << mean
            << ", Median: " << median
            << ", Mode: " << modeVal << " (x" << modeCount << ")";
        return oss.str();
    }
};





int main() {
    const string filename = "data.bin";

    // Create and read binary file
    createBinaryFile(filename);
    BinaryReader reader(filename);
    int* values = reader.getValues();
    int  size = reader.getSize();

    cout << "Binary Data Analyzers" << endl;
    cout << "Data size: " << size << " values\n\n";

    DuplicatesAnalyser dup(values, size);
    cout << dup.analyze() << endl;

    MissingAnalyser miss(values, size);
    cout << miss.analyze() << endl;

    SearchAnalyzer search(values, size);
    cout << search.analyze() << endl;

    StatisticsAnalyser stats(values, size);
    cout << stats.analyze() << endl;

    return 0;
}
