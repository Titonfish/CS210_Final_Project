#include <iostream>
#include <vector>
#include "CSVReader.cpp"
using namespace std;

string GetFile();
vector<vector<string>> LoadData(const string& filePath);

int main()
{
    string filePath;
    while (filePath == "")
    {
        filePath = GetFile();
    }

    CSVReader csv;
    vector<vector<string>> fileData = csv.readCSV(filePath);

    for (int i=1;i<=25;i++)
    {
        cout << fileData[i][0] << "," << fileData[i][1] << "," << fileData[i][2] << endl;
    }
}

string GetFile()
{
    cout << "Input the name of the file containing all the world's cities: ";
    string inputFileName;
    getline(cin, inputFileName);

    ifstream inFile(inputFileName.c_str());
    if (!inFile) {
        cout << "Error: Could not open input file.\n";
        return "";
    }

    return inputFileName;
}