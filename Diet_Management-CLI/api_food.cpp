#include "calories.h"
#include <curl/curl.h>

using json = nlohmann::json;

// USDA FoodData Central API configuration
const string USDA_API_BASE_URL = "https://api.nal.usda.gov/fdc/v1/foods/search";
const string USDA_API_KEY = "txEE9F57IikaMVdzKanTuic5hbH9pB0FodPuue0T"; // Replace with your actual API key
const string USDA_FOODS_CACHE_FILE = "usda_foods_cache.json";

class UsdaFoodScraper {
public:
    UsdaFoodScraper();
    ~UsdaFoodScraper();
    
    bool searchFoodByKeyword(const string& keyword);
    bool addFoodFromUsda(int foodId);
    void showSearchResults();
    
private:
    json searchResults;
    json foodCache;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    string makeApiRequest(const string& url);
    bool saveFoodCache();
    bool loadFoodCache();
    string constructSearchUrl(const string& keyword);
    bool parseSearchResults(const string& response);
};

UsdaFoodScraper::UsdaFoodScraper() {
    loadFoodCache();
}

UsdaFoodScraper::~UsdaFoodScraper() {
    saveFoodCache();
}

size_t UsdaFoodScraper::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string UsdaFoodScraper::makeApiRequest(const string& url) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    return readBuffer;
}

string UsdaFoodScraper::constructSearchUrl(const string& keyword) {
    string encodedKeyword = "";
    
    // Basic URL encoding for the keyword
    for (char c : keyword) {
        if (isalnum(c)) {
            encodedKeyword += c;
        } else if (c == ' ') {
            encodedKeyword += "%20";
        } else {
            char hex[4];
            sprintf(hex, "%%%02X", (unsigned char)c);
            encodedKeyword += hex;
        }
    }
    
    return USDA_API_BASE_URL + "?api_key=" + USDA_API_KEY + "&query=" + encodedKeyword + "&pageSize=25";
}

bool UsdaFoodScraper::parseSearchResults(const string& response) {
    try {
        searchResults = json::parse(response);
        return true;
    } catch (const json::parse_error& e) {
        cerr << "Failed to parse USDA API response: " << e.what() << endl;
        return false;
    }
}

bool UsdaFoodScraper::searchFoodByKeyword(const string& keyword) {
    cout << "Searching USDA database for: " << keyword << endl;
    
    string url = constructSearchUrl(keyword);
    string response = makeApiRequest(url);
    
    if (response.empty()) {
        cout << "Error: Empty response from USDA API" << endl;
        return false;
    }
    
    return parseSearchResults(response);
}

void UsdaFoodScraper::showSearchResults() {
    if (searchResults.is_null() || !searchResults.contains("foods") || searchResults["foods"].empty()) {
        cout << "No food items found in the USDA database." << endl;
        return;
    }
    
    cout << "\n===== USDA Food Search Results =====\n" << endl;
    cout << "+-------+--------------------------------+------------------+----------+" << endl;
    cout << "| Index | Food Name                      | Brand Owner      | Calories |" << endl;
    cout << "+-------+--------------------------------+------------------+----------+" << endl;
    
    int index = 0;
    for (const auto& food : searchResults["foods"]) {
        string foodName = food.contains("description") ? food["description"] : "N/A";
        string brandOwner = food.contains("brandOwner") ? food["brandOwner"] : "N/A";
        
        // Find calories if available
        float calories = 0.0;
        if (food.contains("foodNutrients")) {
            for (const auto& nutrient : food["foodNutrients"]) {
                if (nutrient.contains("nutrientName") && 
                    nutrient["nutrientName"] == "Energy" && 
                    nutrient.contains("value")) {
                    calories = nutrient["value"];
                    break;
                }
            }
        }
        
        // Truncate long strings to fit in the table
        if (foodName.length() > 30) foodName = foodName.substr(0, 27) + "...";
        if (brandOwner.length() > 16) brandOwner = brandOwner.substr(0, 13) + "...";
        
        cout << "| " << left << setw(5) << index << " | "
             << left << setw(30) << foodName << " | "
             << left << setw(16) << brandOwner << " | "
             << right << setw(8) << fixed << setprecision(1) << calories << " |" << endl;
        
        index++;
    }
    
    cout << "+-------+--------------------------------+------------------+----------+" << endl;
    cout << "Total results: " << searchResults["foods"].size() << endl;
}

bool UsdaFoodScraper::addFoodFromUsda(int index) {
    if (searchResults.is_null() || !searchResults.contains("foods") || 
        index < 0 || index >= searchResults["foods"].size()) {
        cout << "Invalid food index." << endl;
        return false;
    }
    
    try {
        const auto& food = searchResults["foods"][index];
        string foodName = food.contains("description") ? food["description"] : "Unknown";
        
        // Process nutrients
        string nutrients = "";
        float calories = 0.0;
        string categories = "USDA";
        
        if (food.contains("foodCategory")) {
            categories += ", " + food["foodCategory"].get<string>();
        }
        
        if (food.contains("foodNutrients")) {
            for (const auto& nutrient : food["foodNutrients"]) {
                if (nutrient.contains("nutrientName")) {
                    string nutrientName = nutrient["nutrientName"];
                    
                    if (nutrientName == "Energy" && nutrient.contains("value")) {
                        calories = nutrient["value"];
                    }
                    
                    if (nutrient.contains("value")) {
                        if (!nutrients.empty()) nutrients += ", ";
                        nutrients += nutrientName + ": " + to_string(nutrient["value"].get<float>());
                        
                        if (nutrient.contains("unitName")) {
                            nutrients += " " + nutrient["unitName"].get<string>();
                        }
                    }
                }
            }
        }
        
        // Generate keywords based on food properties
        string keywords = foodName;
        if (food.contains("brandOwner")) {
            keywords += ", " + food["brandOwner"].get<string>();
        }
        if (food.contains("additionalDescriptions")) {
            keywords += ", " + food["additionalDescriptions"].get<string>();
        }
        
        // Cache the food data
        json cacheEntry = {
            {"name", foodName},
            {"keywords", keywords},
            {"calories", calories},
            {"categories", categories},
            {"nutrients", nutrients},
            {"source", "USDA"},
            {"fdcId", food.contains("fdcId") ? food["fdcId"].get<int>() : 0}
        };
        
        // Add to our cache for future use
        string cacheKey = "usda_" + to_string(food.contains("fdcId") ? food["fdcId"].get<int>() : 0);
        foodCache[cacheKey] = cacheEntry;
        saveFoodCache();
        
        // Add as a basic food
        // Items items;
        // items.addBasicFood(foodName, keywords, calories, categories, nutrients);
        add_to_basicFoods_API(foodName, keywords, calories, categories, nutrients);
        return true;
    } catch (const exception& e) {
        cerr << "Error adding food from USDA: " << e.what() << endl;
        return false;
    }
}

bool UsdaFoodScraper::loadFoodCache() {
    try {
        if (filesystem::exists(USDA_FOODS_CACHE_FILE)) {
            ifstream file(USDA_FOODS_CACHE_FILE);
            if (file.is_open()) {
                file >> foodCache;
                file.close();
                return true;
            }
        }
    } catch (const exception& e) {
        cerr << "Error loading USDA food cache: " << e.what() << endl;
    }
    
    foodCache = json::object();
    return false;
}

bool UsdaFoodScraper::saveFoodCache() {
    try {
        ofstream file(USDA_FOODS_CACHE_FILE);
        if (file.is_open()) {
            file << foodCache.dump(4);
            file.close();
            return true;
        }
    } catch (const exception& e) {
        cerr << "Error saving USDA food cache: " << e.what() << endl;
    }
    return false;
}

void searchAndAddUsdaFood() {
    cout << "Search for USDA Food Data" << endl;
    cout << "Enter keyword to search: ";
    string keyword;
    cin.ignore();
    getline(cin, keyword);
    
    UsdaFoodScraper scraper;
    if (scraper.searchFoodByKeyword(keyword)) {
        scraper.showSearchResults();
        
        cout << "Enter index of the food to add (-1 to cancel): ";
        int index;
        while (!(cin >> index)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number: ";
        }
        
        if (index >= 0) {
            if (scraper.addFoodFromUsda(index)) {
                cout << "Successfully added food from USDA database." << endl;
            } else {
                cout << "Failed to add food from USDA database." << endl;
            }
        }
    } else {
        cout << "Failed to search USDA database. Please check your internet connection or try a different keyword." << endl;
    }
}