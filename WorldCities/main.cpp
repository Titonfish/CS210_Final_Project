#include <iostream>
#include <vector>
#include "CSVReader.cpp"
#include <queue>
#include <unordered_map>
using namespace std;

vector<vector<string>> FilterByCountry(const vector<vector<string>>& allCities, const string& countryCode);
vector<vector<string>> FilterByCity(const vector<vector<string>>& allCities, const string& cityName);
string CityToString(const vector<string>& city);

void LFU_UpdateCache(vector<string>& cacheKeys, unordered_map<string, pair<vector<string>, pair<int,int>>>& cache, const vector<string>& city);
vector<vector<string>> LFU_SearchCache(const vector<string>& cacheKeys, unordered_map<string, pair<vector<string>, pair<int,int>>>& cache, const string& countryCode, const string& cityName);
void LFU_DisplayCache(const vector<string>& cacheKeys, unordered_map<string, pair<vector<string>, pair<int,int>>> cache);

void FIFO_UpdateCache(queue<vector<string>>& cache, const vector<string>& city);
vector<vector<string>> FIFO_SearchCache(queue<vector<string>>& cache, const string& countryCode, const string& cityName);
void FIFO_DisplayCache(queue<vector<string>> cache);

int main()
{
    // Define the file space to search in for the data
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

    // Choose which cache type to use
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

    // Prepare all the cache types, regardless of selection
    vector<string> lfu_cityCacheKeys {10};
    unordered_map<string, pair<vector<string>, pair<int, int>>> lfu_cityCache;
    queue<vector<string>> fifo_cityCache;

    string userInput;

    while (true)
    {
        cout << "Search for a city, or leave?" << endl << "Enter s to search, c to see the cache, or any other key to quit:" << endl;
        getline(cin, userInput);
        if (userInput != "s" && userInput != "S" && userInput != "c" && userInput != "C")
        {
            return 0;
        }
        if (userInput == "c" || userInput == "C")
        {
            if (cacheType == "lfu")
            {
                LFU_DisplayCache(lfu_cityCacheKeys, lfu_cityCache);
            }
            else if (cacheType == "fifo")
            {
                FIFO_DisplayCache(fifo_cityCache);
            }

            cout << endl << endl;
            continue;
        }

        string countryCode;
        string cityName;

        cout << "What is the country code of the city you are searching for?" << endl;
        getline(cin, countryCode);
        cout << "What is the name of the city you are searching for?" << endl;
        getline(cin, cityName);

        if (cacheType == "lfu")
        {
            if (vector<vector<string>> foundInCache = LFU_SearchCache(lfu_cityCacheKeys, lfu_cityCache, countryCode, cityName); foundInCache.empty())
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
        else if (cacheType == "fifo")
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

                if (cacheType == "lfu")
                {
                    LFU_UpdateCache(lfu_cityCacheKeys, lfu_cityCache, curCity);
                }
                else if (cacheType == "fifo")
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

void LFU_UpdateCache(vector<string>& cacheKeys, unordered_map<string, pair<vector<string>, pair<int,int>>>& cache, const vector<string>& city)
{

    int replaceIndex = 0;
    int minFreq = INT_MAX;
    int oldestTime = 0;
    for (int i=0;i<10;i++)
    {
        if (cacheKeys[i].empty())
        {
            cacheKeys[i] = city[0] + city[1] + city[2];
            cache[city[0] + city[1] + city[2]] = make_pair(city, make_pair(1,1));
            return;
        }

        if (cache[cacheKeys[i]].second.first < minFreq
            || (cache[cacheKeys[i]].second.first == minFreq && cache[cacheKeys[i]].second.second < oldestTime))
        {
            replaceIndex = i;
            minFreq = cache[cacheKeys[i]].second.first;
            oldestTime = cache[cacheKeys[i]].second.second;
        }
    }

    cacheKeys[replaceIndex] = city[0] + city[1] + city[2];
    cache[city[0] + city[1] + city[2]] = make_pair(city, make_pair(1,1));
}
vector<vector<string>> LFU_SearchCache(const vector<string>& cacheKeys, unordered_map<string, pair<vector<string>, pair<int,int>>>& cache, const string& countryCode, const string& cityName)
{
    vector<vector<string>> foundCities;

    const unsigned int size = cacheKeys.size();
    for (int i = 0; i < size; i++)
    {
        if (cacheKeys[i] == "")
        {
            continue;
        }

        if (cache[cacheKeys[i]].first[0] == countryCode && cache[cacheKeys[i]].first[1] == cityName)
        {
            foundCities.push_back(cache[cacheKeys[i]].first);
            cache[cacheKeys[i]].second.first++;
        }

        cache[cacheKeys[i]].second.second++;
    }

    return foundCities;
}
void LFU_DisplayCache(const vector<string>& cacheKeys, unordered_map<string, pair<vector<string>, pair<int,int>>> cache)
{
    for (const auto & cacheKey : cacheKeys)
    {
        if (cacheKey.empty())
        {
            continue;
        }

        const int freq = cache[cacheKey].second.first;
        const int time = cache[cacheKey].second.second;

        cout << CityToString(cache[cacheKey].first) + " - Freq: " << freq << ", Time: " << time << endl;
    }
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
void FIFO_DisplayCache(queue<vector<string>> cache)
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
