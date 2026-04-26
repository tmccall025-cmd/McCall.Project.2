#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <fstream>
using namespace std;


void selection_sort(int* arr, int size)
{
	for (int i = 0; i < size - 1; ++i)
	{
		int minIndex = i;
		for (int j = i + 1; j < size; ++j)
		{
			if (arr[j] < arr[minIndex])
			minIndex = j;
		}
		if ( minIndex != i)
		{
			int temp = arr[i];
			arr[i] = arr[minIndex];
			arr[minIndex] = temp;
		}
	}

}

bool binary_search_recursive(int* arr, int value, int start, int end)
{
	if (start > end)
		return false;

	int mid = start + (end - start) / 2;

	if (arr[mid] == value)
		return true;

	else if (arr[mid] > value)
		return binary_search_recursive(arr, value, start, mid - 1);

	else
		return binary_search_recursive(arr, value, mid + 1, end);
}
bool binary_search(int* arr, int key, int size)
{
	return binary_search_recursive(arr, key, 0, size - 1);
}
void writeBinary(string filename, int* values, int length)
{
	ofstream out(filename, ios::binary);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	out.write(reinterpret_cast<char*>(values), sizeof(int) * length);
	out.close();
}
void createBinaryFile(string filename)
{
	const int SIZE = 50;
	int values[SIZE];
	mt19937 gen(random_device{}());
	uniform_int_distribution<int> dist(0, 999);
	for (int i = 0; i < SIZE; ++i)
		values[i] = dist(gen);

	writeBinary(filename, values, SIZE);
}

	class Analyzer
	{
	protected:
		int* data;
		int size;

	public:
		Analyzer(int* arr, int size)
		{
			this->size = size;
			data = cloneValues(arr,size);
			for (int i = 0; i < size; ++i)
				data[i] = arr[i];
		}
		int* cloneValues(int* arr, int size)
		{
			int* copy = new int[size];
		for (int i = 0; i < size; ++i)
			copy[i] = arr[i];
			return copy;
		}
		~Analyzer()
		{
			delete[] data;
		}
		virtual string analyze() = 0;
	};

	class SearchAnalyzer : public Analyzer
	{
	public:
		SearchAnalyzer(int* arr, int size) : Analyzer(arr, size)
		{
			selection_sort(data, this->size);
		}
		string analyze() override
		{
			mt19937 gen(random_device{}());
			uniform_int_distribution<int>dist(0, 999);

			int foundCount = 0;
			for (int i = 0; i < 100; ++i)
			{
				int value = dist(gen);
				if (binary_search(data, value, size))
					++foundCount;
			}
			return to_string(foundCount);
		}
	};

	class StatisticsAnalyzer : public Analyzer
	{
		public:
		StatisticsAnalyzer(int* arr, int size) : Analyzer(arr, size) {}
		string analyze() override;

	};

	string StatisticsAnalyzer::analyze()
	{
		selection_sort(data, size);

		int min = data[0];
		int max = data[size - 1];

		double median = 0.0;
		if (size % 2 == 1)
			median = data[size / 2];
		else
			median = (data[size / 2 - 1] + data[size / 2]) / 2.0;
		int mode = data[0];
		int modeCount = 1;
		int currentValue = data[0];
		int currentCount = 1;

		for (int i = 1; i < size; ++i)
		{

			if (data[i] == currentValue)
			{
				++currentCount;
			}
			else
			{
				if (currentCount > modeCount)
				{
					modeCount = currentCount;
					mode = currentValue;
				}
				currentValue = data[i];
				currentCount = 1;
			}

		}
		if (currentCount > modeCount)
		{
			modeCount = currentCount;
			mode = currentValue;
		}
		ostringstream out;
		out << "Minimum: " << min << "\n";
		out << "Maximum: " << max << "\n";
		out << "Median: " << median << "\n";
		out << "Mode: " << mode << "\n";
		return out.str();
	}

	class DuplicatesAnalyzer : public Analyzer
	{

	public:
		DuplicatesAnalyzer(int* values, int size) : Analyzer(values, size) {}
		string analyze() override
		{
			int duplicateCount = 0;
			for (int i = 0; i < size; ++i)
			{
				bool countedAlready = false;
				for (int k = 0; k < i; ++k)
				{
					if (data[k] == data[i])
					{
						countedAlready = true;
						break;
					}
				}
				if (countedAlready)
					continue;
				for (int j = i + 1; j < size; ++j)
				{
					if (data[i] == data[j])
					{
						++duplicateCount;
						break;
					}
				}
			}
			return to_string(duplicateCount);
		}

	};

	class MissingAnalyzer : public Analyzer
	{
	public:
		MissingAnalyzer(int* values, int size) : Analyzer(values, size) {}
		string analyze() override
		{
			int missingCount = 0;
			for (int num = 0; num <= 999; ++num)
			{
				bool found = false;
				for (int i = 0; i < size; ++i)
				{
					if (data[i] == num)
					{
						found = true;
						break;
					}
				}
				if (!found)
					++missingCount;
			}
			return to_string(missingCount);
		}
	};

	class BinaryReader 
	{
	private:
		int* values;
		int size;
	public:
		BinaryReader(string filename)
		{
			values = nullptr;
			size = 0;
			readValues(filename);
		}
		~BinaryReader()
		{
			delete[] values;
		}
		int* getValues()
		{
			return values;
		}
		int getSize()
		{
			return size;
		}
		void readValues(string filename)
		{
			ifstream in(filename, ios::binary);
			in.read(reinterpret_cast<char*>(&size), sizeof(size));
			values = new int[size];
			in.read(reinterpret_cast<char*>(values), sizeof(int) * size);
			in.close();
		}

	};

	int main()
	{
		createBinaryFile("data.bin");
		BinaryReader reader("data.bin");
		int* data = reader.getValues();
		int size = reader.getSize();

		DuplicatesAnalyzer duplicates(data, size);
		MissingAnalyzer missing(data, size);
		SearchAnalyzer search(data, size);
		StatisticsAnalyzer stats(data, size);

		cout << "Duplicates: " << duplicates.analyze() << endl;
		cout << "Quantity Between 1 and 1000 Not Reflected: " << missing.analyze() << endl;
		cout << "Successful Values in Array: " << search.analyze() << endl;
		cout << "Stats:\n" << stats.analyze() << endl;

		return 0;
	}
