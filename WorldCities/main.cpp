#include <iostream>
#include <vector>
#include "CSVReader.cpp"
#include <queue>
#include <unordered_map>
#include <random>
#include "trie.cpp"
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

void RAND_UpdateCache(vector<vector<string>>& cache, const vector<string>& city, mt19937& rand);
vector<vector<string>> RAND_SearchCache(const vector<vector<string>>& cache, const string& countryCode, const string& cityName);
void RAND_DisplayCache(vector<vector<string>>& cache);

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

    // Choose if using a trie or not
    bool usingTrie;

    cout << "Are you using a trie to store the data? (y,n) : ";
    string trieInput;
    getline(cin, trieInput);

    usingTrie = trieInput == "Y" || trieInput == "y";

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
    vector<vector<string>> rand_cityCache {10};

    // Prepare a random device for random cache replacement
    random_device rd;
    mt19937 gen(rd());

    // Set up the Trie in the case of using one
    CityTrie cityTrie;

    string userInput;

    if (usingTrie)
    {
        vector<vector<string>> allCities = CSVReader::readCSV(filePath);
        for (auto city : allCities)
        {
            cityTrie.insert(city[1], city);
        }
        cout << "Loaded all cities into the trie" << endl << endl;
    }

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
            else
            {
                RAND_DisplayCache(rand_cityCache);
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

        vector<vector<string>> foundInCache;
        if (cacheType == "lfu")
        {
            foundInCache = LFU_SearchCache(lfu_cityCacheKeys, lfu_cityCache, countryCode, cityName);
        }
        else if (cacheType == "fifo")
        {
            foundInCache = FIFO_SearchCache(fifo_cityCache, countryCode, cityName);
        }
        else
        {
            foundInCache = RAND_SearchCache(rand_cityCache, countryCode, cityName);
        }

        if (foundInCache.empty())
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

        vector<vector<string>> citiesFound;
        if (usingTrie)
        {
            vector<string> cityFound = cityTrie.search(cityName);
            if (!cityFound.empty())
            {
                citiesFound.push_back(cityFound);
            }
        }
        else
        {
            citiesFound = CSVReader::readCSV(filePath);
            citiesFound = FilterByCountry(citiesFound, countryCode);
            citiesFound = FilterByCity(citiesFound, cityName);
        }

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
                else
                {
                    RAND_UpdateCache(rand_cityCache, curCity, gen);
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
        if (cacheKeys[i].empty())
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

void RAND_UpdateCache(vector<vector<string>>& cache, const vector<string>& city, mt19937& rand)
{
    for (int i=0;i<10;i++)
    {
        if (cache[i].empty())
        {
            cache[i] = city;
            return;
        }
    }

    uniform_int_distribution distrib(0, 9);
    const int replaceIndex = distrib(rand);

    cache[replaceIndex] = city;
}
vector<vector<string>> RAND_SearchCache(const vector<vector<string>>& cache, const string& countryCode, const string& cityName)
{
    vector<vector<string>> foundCities;

    for (int i = 0; i < 10; i++)
    {
        if (cache[i].empty())
        {
            continue;
        }

        if (cache[i][0] == countryCode && cache[i][1] == cityName)
        {
            foundCities.push_back(cache[i]);
        }
    }

    return foundCities;
}
void RAND_DisplayCache(vector<vector<string>>& cache)
{
    for (const auto & curCity : cache)
    {
        if (curCity.empty())
        {
            continue;
        }

        cout << CityToString(curCity) << endl;
    }
}