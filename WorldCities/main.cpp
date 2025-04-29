#include <iostream>
#include <vector>
#include "CSVReader.cpp"
#include <queue>
using namespace std;

vector<vector<string>> FilterByCountry(const vector<vector<string>>& allCities, const string& countryCode);
vector<vector<string>> FilterByCity(const vector<vector<string>>& allCities, const string& cityName);
string CityToString(const vector<string>& city);

void FIFO_UpdateCache(queue<vector<string>>& cache, const vector<string>& city);
vector<vector<string>> FIFO_SearchCache(queue<vector<string>>& cache, const string& countryCode, const string& cityName);
void FIFO_DisplayCache(queue<vector<string>>& cache);

int main()
{
    string filePath;
    while (filePath.empty())
    {
        cout << "Input the name of the file containing all the world's cities: ";
        string inputFileName;
        getline(cin, inputFileName);

        ifstream inFile(inputFileName.c_str());
        if (!inFile)
        {
            cout << "Error: Could not open input file." << endl;
            continue;
        }

        filePath = inputFileName;
    }

    string cacheType;
    while (cacheType.empty())
    {
        cout << "What kind of cache would you like to use? (lfu, fifo, rand) : ";
        string cacheTypeInput;
        getline(cin, cacheTypeInput);

        if (cacheTypeInput == "lfu" || cacheTypeInput == "fifo" || cacheTypeInput == "rand")
        {
            cacheType = cacheTypeInput;
        }
        else
        {
            cout << "Error: undefined cache type" << endl;
        }
    }

    cout << endl;

    queue<vector<string>> fifo_cityCache;

    string userInput;

    while (true)
    {
        cout << "Search for a city, or leave?" << endl << "Enter s to search, c to see the cache, or any other key to quit:" << endl;
        cin >> userInput;
        if (userInput != "s" && userInput != "S" && userInput != "c" && userInput != "C")
        {
            return 0;
        }
        if (userInput == "c" || userInput == "C")
        {
            if (cacheType == "fifo")
            {
                FIFO_DisplayCache(fifo_cityCache);
            }
            cout << endl << endl;
            continue;
        }

        string countryCode;
        string cityName;

        cout << "What is the country code of the city you are searching for?" << endl;
        cin >> countryCode;
        cout << "What is the name of the city you are searching for?" << endl;
        cin >> cityName;

        if (cacheType == "fifo")
        {
            if (vector<vector<string>> foundInCache = FIFO_SearchCache(fifo_cityCache, countryCode, cityName); foundInCache.empty())
            {
                cout << "No cities found in cache with that city name and country code" << endl;
            }
            else
            {
                cout << "Found " << foundInCache.size() << " from cache:" << endl;
                for (vector<string> &curCity : foundInCache)
                {
                    cout << CityToString(curCity) << endl;
                }
                cout << endl;

                continue;
            }
        }

        vector<vector<string>> citiesFound= CSVReader::readCSV(filePath);
        citiesFound = FilterByCountry(citiesFound, countryCode);
        citiesFound = FilterByCity(citiesFound, cityName);

        if (citiesFound.empty())
        {
            cout << "No cities found with that city name and country code" << endl;
        }
        else
        {
            cout << "Found " << citiesFound.size() << " from file:" << endl;
            for (vector<string> &curCity : citiesFound)
            {
                cout << CityToString(curCity) << endl;

                if (cacheType == "fifo")
                {
                    FIFO_UpdateCache(fifo_cityCache, curCity);
                }
            }
        }

        cout << endl;
    }
}

vector<vector<string>> FilterByCountry(const vector<vector<string>>& allCities, const string& countryCode)
{
    vector<vector<string>> remainingCities;
    for (const auto & curCity : allCities)
    {
        if (curCity[0] == countryCode)
        {
            remainingCities.push_back(curCity);
        }
    }

    return remainingCities;
}
vector<vector<string>> FilterByCity(const vector<vector<string>>& allCities, const string& cityName)
{
    vector<vector<string>> remainingCities;
    for (const auto & curCity : allCities)
    {
        if (curCity[1] == cityName)
        {
            remainingCities.push_back(curCity);
        }
    }

    return remainingCities;
}
string CityToString(const vector<string>& city)
{
    return "City Name: " + city[1] + ", Country Code: " + city[0] + ", Population: " + city[2];
}
void FIFO_UpdateCache(queue<vector<string>>& cache, const vector<string>& city)
{
    cache.push(city);
    if (cache.size() > 10)
    {
        cache.pop();
    }
}
vector<vector<string>> FIFO_SearchCache(queue<vector<string>>& cache, const string& countryCode, const string& cityName)
{
    vector<vector<string>> foundCities;

    const unsigned int size = cache.size();
    for (int i = 0; i < size; i++)
    {
        bool isFoundCity = false;
        if (cache.front()[0] == countryCode && cache.front()[1] == cityName)
        {
            foundCities.push_back(cache.front());
            isFoundCity = true;
        }

        vector<string> curCity = cache.front();
        cache.pop();

        if (!isFoundCity)
        {
            cache.push(curCity);
        }
    }

    for (const auto & curCity : foundCities)
    {
        cache.push(curCity);
    }

    return foundCities;
}
void FIFO_DisplayCache(queue<vector<string>>& cache)
{
    const unsigned int size = cache.size();
    for (int i = 0; i < size; i++)
    {
        vector<string> curCity = cache.front();
        cout << CityToString(curCity) << endl;

        cache.pop();
        cache.push(curCity);
    }
}
