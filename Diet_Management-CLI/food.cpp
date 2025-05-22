#include "calories.h"

using json = nlohmann::json;
// using namespace std;

// File paths for JSON storage
const string BASIC_FOODS_FILE = "basic_foods.json";
const string COMPOSITE_FOODS_FILE = "composite_foods.json";

class Items {
public:
    Items();   // Constructor
    ~Items();  // Destructor
    void addBasicFood(const string& name, const string& keywords, float calories, const string& categories, const string& nutrients);
    void addCompositeFood(const string& name, string& keywords, string& items, string& quantities, string& itemIds, float* totalCalories, string& categories, string& nutrients);
    void showBasicFoods();
    void showCompositeFoods();
    int check_if_basic_food_exists(int foodId);
    int check_if_composite_food_exists(int foodId);
    void add_basic_food_to_composite_food(int foodId, int quantity, string& quantities, string& itemIds, string& itemNames, float* totalCalories, string& categories, string& nutrients);
    void add_composite_food_to_composite_food(int foodId, int quantity, string& quantities, string& itemIds, string& itemNames, float* totalCalories, string& categories, string& nutrients);
    int BasicFoodsEmpty();
    int CompositeFoodsEmpty();

private:
    struct LogEntry {
        string date;
        int foodId;
        bool isBasicFood;
        string foodName;
        float calories;
        int servings;
    };
    vector<LogEntry> foodLog;
    
    // Helper functions for JSON operations
    json loadBasicFoods();
    json loadCompositeFoods();
    void saveBasicFoods(const json& foods);
    void saveCompositeFoods(const json& foods);
    int getNextBasicFoodId(const json& foods);
    int getNextCompositeFoodId(const json& foods);
};

Items::Items() {
    // Constructor implementation
}

Items::~Items() {
    // Destructor implementation 
}

json Items::loadBasicFoods() {
    json foods = json::array();
    
    if (filesystem::exists(BASIC_FOODS_FILE)) {
        ifstream file(BASIC_FOODS_FILE);
        if (file.is_open()) {
            try {
                file >> foods;
            } 
            catch (const json::parse_error& e) {
                cerr << "JSON parse error: " << e.what() << endl;
            }
            file.close();
        }
    }
    return foods;
}

json Items::loadCompositeFoods() {
    json foods = json::array();
    
    if (filesystem::exists(COMPOSITE_FOODS_FILE)) {
        ifstream file(COMPOSITE_FOODS_FILE);
        if (file.is_open()) {
            try {
                file >> foods;
            } 
            catch (const json::parse_error& e) {
                cerr << "JSON parse error: " << e.what() << endl;
            }
            file.close();
        }
    }
    return foods;
}

void Items::saveBasicFoods(const json& foods) {
    ofstream file(BASIC_FOODS_FILE);
    if (file.is_open()) {
        file << foods.dump(4);
        file.close();
    } else {
        throw runtime_error("Unable to open basic foods file for writing");
    }
}

void Items::saveCompositeFoods(const json& foods) {
    ofstream file(COMPOSITE_FOODS_FILE);
    if (file.is_open()) {
        file << foods.dump(4);
        file.close();
    } else {
        throw runtime_error("Unable to open composite foods file for writing");
    }
}

int Items::getNextBasicFoodId(const json& foods) {
    int maxId = 0;
    for (const auto& food : foods) {
        if (food.contains("foodId") && food["foodId"] > maxId) {
            maxId = food["foodId"];
        }
    }
    return maxId + 1;
}

int Items::getNextCompositeFoodId(const json& foods) {
    int maxId = 0;
    for (const auto& food : foods) {
        if (food.contains("foodId") && food["foodId"] > maxId) {
            maxId = food["foodId"];
        }
    }
    return maxId + 1;
}

struct CaseInsensitiveCompare {
    bool operator()(const string& a, const string& b) const {
        return lexicographical_compare(
            a.begin(), a.end(),
            b.begin(), b.end(),
            [](char c1, char c2) {
                return tolower(c1) < tolower(c2);
            });
    }
};


void Items::addBasicFood(const string& name, const string& keywords, float calories, const string& categories, const string& nutrients) {
    try {
        json foods = loadBasicFoods();
        int newId = getNextBasicFoodId(foods);
        
        json newFood = {
            {"foodId", newId},
            {"name", name},
            {"keywords", keywords},
            {"calories", calories},
            {"categories", categories},
            {"nutrients", nutrients}
        };
        
        foods.push_back(newFood);
        saveBasicFoods(foods);
        
        cout << "Basic Food Item added successfully with ID: " << newId << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}


void input_addBasicFood() {
    string name, keywords, categories, nutrients;
    float calories;
    cout << "Enter name: ";
    cin.ignore(); // To ignore the newline character left by previous cin
    getline(cin, name);
    cout << "Enter keywords: ";
    getline(cin, keywords);
    set<string, CaseInsensitiveCompare> uniqueKeywords;
    stringstream ss(keywords);
    string keyword;
    while (getline(ss, keyword, ',')) {
        string trimmed = trim(keyword);
        if (!trimmed.empty()) {
            uniqueKeywords.insert(trimmed);
        }
    }
    // Reconstruct the unique keywords string 
    string uniqueKeywordsStr;
    for (const auto& kw : uniqueKeywords) {
        if (!uniqueKeywordsStr.empty()) {
            uniqueKeywordsStr += ", ";
        }
        uniqueKeywordsStr += kw;
    }

    cout << "Enter calories: ";
    while (!(cin >> calories)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "Invalid input. Please enter a number for calories: ";
    }
    cin.ignore(); // To ignore the newline character left by previous cin
    cout << "Enter categories: ";
    getline(cin, categories);
    set<string, CaseInsensitiveCompare> uniqueCategories;
    stringstream ssCategories(categories);
    string category;
    while (getline(ssCategories, category, ',')) {
        string trimmed = trim(category);
        if (!trimmed.empty()) {
            uniqueCategories.insert(trimmed);
        }
    }
    // Reconstruct the unique categories string
    string uniqueCategoriesStr;         
    for (const auto& cat : uniqueCategories) {
        if (!uniqueCategoriesStr.empty()) {
            uniqueCategoriesStr += ", ";
        }
        uniqueCategoriesStr += cat;
    }
    cout << "Enter nutrients: ";
    getline(cin, nutrients);     
    set<string, CaseInsensitiveCompare> uniqueNutrients;
    stringstream ssNutrients(nutrients);
    string nutrient;
    while (getline(ssNutrients, nutrient, ',')) {
        string trimmed = trim(nutrient);
        if (!trimmed.empty()) {
            uniqueNutrients.insert(trimmed);
        }
    }
    // Reconstruct the unique nutrients string  
    string uniqueNutrientsStr;
    for (const auto& nut : uniqueNutrients) {
        if (!uniqueNutrientsStr.empty()) {
            uniqueNutrientsStr += ", ";
        }
        uniqueNutrientsStr += nut;
    }                           
    Items items;
    items.addBasicFood(name, uniqueKeywordsStr, calories, uniqueCategoriesStr, uniqueNutrientsStr);
}

string removeCommaSeparatedDuplicates(const string& input) {
    unordered_set<string> seenLower;
    istringstream ss(input);
    string token, result;

    while (getline(ss, token, ',')) {
        // Trim leading and trailing spaces
        token.erase(0, token.find_first_not_of(" \t\n\r"));
        token.erase(token.find_last_not_of(" \t\n\r") + 1);

        string lowered = token;
        transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

        if (seenLower.find(lowered) == seenLower.end()) {
            seenLower.insert(lowered);
            if (!result.empty()) result += ",";
            result += token;
        }
    }

    return result;
}



void Items::addCompositeFood(const string& name, string& keywords, string& items, string& quantities, string& itemIds, float* totalCalories, string& categories, string& nutrients) {
    try {
        json foods = loadCompositeFoods();
        int newId = getNextCompositeFoodId(foods);
        
        if (!categories.empty() && categories.back() == ',') {
            categories.pop_back();
        }
        if (!nutrients.empty() && nutrients.back() == ',') {
            nutrients.pop_back();
        }
        if (!items.empty() && items.back() == ',') {
            items.pop_back();
        }
        if (!quantities.empty() && quantities.back() == ',') {
            quantities.pop_back();
        }
        if (!itemIds.empty() && itemIds.back() == ',') {
            itemIds.pop_back();
        }
        cout << "Categories: " << categories << endl;
        categories = removeCommaSeparatedDuplicates(categories);
        cout << "Categories after removing duplicates: " << categories << endl;
        nutrients = removeCommaSeparatedDuplicates(nutrients);
        
        json newFood = {
            {"foodId", newId},
            {"name", name},
            {"keywords", keywords},
            {"items", items},
            {"quantities", quantities},
            {"itemIds", itemIds},
            {"totalCalories", *totalCalories},
            {"categories", categories},
            {"nutrients", nutrients}
        };
        
        foods.push_back(newFood);
        saveCompositeFoods(foods);
        
        cout << "Composite Food Item added successfully with ID: " << newId << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}


string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (string::npos == first) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

void input_addCompositeFood() {
    string name, keywords;
    float calories;
    cout << "Enter name: "; 
    cin.ignore();
    getline(cin, name);
    cout << "Enter keywords: ";
    getline(cin, keywords);
    set<string, CaseInsensitiveCompare> uniqueKeywords;
    stringstream ss(keywords);
    string keyword;
    
    while (getline(ss, keyword, ',')) {
        string trimmed = trim(keyword);
        if (!trimmed.empty()) {
            uniqueKeywords.insert(trimmed);
        }
    }
    
    // Reconstruct the unique keywords string
    string uniqueKeywordsStr;
    for (const auto& kw : uniqueKeywords) {
        if (!uniqueKeywordsStr.empty()) {
            uniqueKeywordsStr += ", ";
        }
        uniqueKeywordsStr += kw;
    }
    int numItems = 0;
    string itemNames = "";
    string quantities = "";
    string itemIds = "";
    string categories = "";
    string nutrients = "";
    float totalCalories = 0;
    while(1){
        cout << "Select the type of food item you want to add: " << endl;
        cout << "1. Basic Food" << endl;
        cout << "2. Composite Food" << endl;
        cout << "3. Finish adding food items" << endl;
        int foodType;
        cout << "Enter your choice: ";
        cin >> foodType;
        if (foodType == 1) {
            Items items;
            items.showBasicFoods();
            int isEmpty = items.BasicFoodsEmpty();
            if(isEmpty == 0){
                cout << "No basic food items found. Please add basic food items first." << endl;
                break;
            }
            cout << "Enter the ID of the basic food item: ";
            int foodId;
            cin >> foodId;
            if(cin.fail()){
                cout << "Invalid input. Please enter a valid number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            int foodExists = items.check_if_basic_food_exists(foodId);
            if(foodExists == 0){
                cout << "The basic food item does not exist. Please add existing food." << endl;
                continue;
            }
            cout << "Enter the quantity: ";
            int quantity;
            cin.ignore();
            while(!(cin >> quantity)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter a positive number for quantity: ";
            }

            items.add_basic_food_to_composite_food(foodId, quantity,quantities,itemIds, itemNames, &totalCalories,categories, nutrients);
            numItems++;
        } else if (foodType == 2) {
            Items items;
            items.showCompositeFoods();
            int isEmpty = items.CompositeFoodsEmpty();
            if(isEmpty == 0){
                cout << "No composite food items found. Please add composite food items first or add basic items." << endl;
                continue;
            }
            cout << "Enter the ID of the composite food item: ";
            int foodId;
            cin >> foodId;
                if(cin.fail()){
                cout << "Invalid input. Please enter a valid number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            int foodExists = items.check_if_composite_food_exists(foodId);
            if(foodExists == 0){
                cout << "The composite food item does not exist. Please add ta valid food ID." << endl;
                continue;
            }
            cout << "Enter the quantity: ";
            int quantity;
            cin.ignore(); 
            while(!(cin >> quantity)) {
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter a positive number for quantity: ";
            }

            items.add_composite_food_to_composite_food(foodId, quantity,quantities,itemIds, itemNames, &totalCalories,categories, nutrients);
            numItems++;
        }
        else if (foodType == 3) {
            // if(itemNames.find(",") != string::npos){
            //     itemNames = itemNames.substr(0, itemNames.size()-1);
            // }
            if (numItems < 2) {
                cout << "A composite food item should have at least 2 items. Please add more items." << endl;
                continue;
            }
            else{
                break;
            }
        } 
        else if(cin.fail()){
            cout << "Invalid input. Please enter a valid number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        else {
            cout << "Enter a valid choice" << endl;
        }
    }
    Items items;
    items.addCompositeFood(name, uniqueKeywordsStr, itemNames, quantities, itemIds, &totalCalories, categories, nutrients);
}

void Items::showBasicFoods() {
    try {
        json foods = loadBasicFoods();
        if (foods.empty()) {
            cout << "No basic food items found." << endl;
            return;
        }
        
        cout << "Basic Food Items:" << endl;
        cout << "ID\tName\t\tCalories\tCategories" << endl;
        cout << "------------------------------------------------" << endl;
        
        for (const auto& food : foods) {
            cout << food["foodId"] << "\t" 
                 << food["name"] << "\t\t"
                 << food["calories"] << "\t\t"
                 << food["categories"] << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void Items::showCompositeFoods() {
    try {
        json foods = loadCompositeFoods();
        if (foods.empty()) {
            cout << "No composite food items found." << endl;
            return;
        }
        
        cout << "Composite Food Items:" << endl;
        cout << "ID\tName\t\tTotal Calories\tItems" << endl;
        cout << "------------------------------------------------" << endl;
        
        for (const auto& food : foods) {
            cout << food["foodId"] << "\t" 
                 << food["name"] << "\t\t"
                 << food["totalCalories"] << "\t\t"
                 << food["items"] << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

int Items::check_if_basic_food_exists(int foodId) {
    try {
        json foods = loadBasicFoods();
        for (const auto& food : foods) {
            if (food["foodId"] == foodId) {
                return 1;
            }
        }
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 0;
    }
}

int Items::check_if_composite_food_exists(int foodId) {
    try {
        json foods = loadCompositeFoods();
        for (const auto& food : foods) {
            if (food["foodId"] == foodId) {
                return 1;
            }
        }
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 0;
    }
}

int Items::BasicFoodsEmpty() {
    json foods = loadBasicFoods();
    return foods.empty() ? 0 : 1;
}

int Items::CompositeFoodsEmpty() {
    json foods = loadCompositeFoods();
    return foods.empty() ? 0 : 1;
}

void Items::add_basic_food_to_composite_food(int foodId, int quantity, string& quantities, string& itemIds, string& itemNames, float* totalCalories, string& categories, string& nutrients) {
    try {
        json foods = loadBasicFoods();
        for (const auto& food : foods) {
            if (food["foodId"] == foodId) {
                if (!itemNames.empty()) {
                    itemNames += ",";
                }
                itemNames += food["name"].get<string>();
                
                if (!quantities.empty()) {
                    quantities += ",";
                }
                quantities += to_string(quantity);
                
                if (!itemIds.empty()) {
                    itemIds += ",";
                }
                itemIds += to_string(foodId);
                
                *totalCalories += food["calories"].get<float>() * quantity;
                
                // Add categories if not already present
                string foodCategories = food["categories"].get<string>();
                if (categories.find(foodCategories) == string::npos) {
                    if (!categories.empty()) {
                        categories += ",";
                    }
                    categories += foodCategories;
                }
                
                // Add nutrients if not already present
                string foodNutrients = food["nutrients"].get<string>();
                if (nutrients.find(foodNutrients) == string::npos) {
                    if (!nutrients.empty()) {
                        nutrients += ",";
                    }
                    nutrients += foodNutrients;
                }
                
                break;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void Items::add_composite_food_to_composite_food(int foodId, int quantity, string& quantities, string& itemIds, string& itemNames, float* totalCalories, string& categories, string& nutrients) {
    try {
        json foods = loadCompositeFoods();
        for (const auto& food : foods) {
            if (food["foodId"] == foodId) {
                if (!itemNames.empty()) {
                    itemNames += ",";
                }
                itemNames += food["name"].get<string>();
                
                if (!quantities.empty()) {
                    quantities += ",";
                }
                quantities += to_string(quantity);
                
                if (!itemIds.empty()) {
                    itemIds += ",";
                }
                itemIds += to_string(foodId);
                
                *totalCalories += food["totalCalories"].get<float>() * quantity;
                
                // Add categories if not already present
                string foodCategories = food["categories"].get<string>();
                if (categories.find(foodCategories) == string::npos) {
                    if (!categories.empty()) {
                        categories += ",";
                    }
                    categories += foodCategories;
                }
                
                // Add nutrients if not already present
                string foodNutrients = food["nutrients"].get<string>();
                if (nutrients.find(foodNutrients) == string::npos) {
                    if (!nutrients.empty()) {
                        nutrients += ",";
                    }
                    nutrients += foodNutrients;
                }
                
                break;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}



void add_to_basicFoods_API(string& foodName,string & keywords,float calories,string& categories,string& nutrients){
    Items items;
    items.addBasicFood(foodName, keywords, calories, categories, nutrients);
    cout << "Basic Food Item added successfully : " << foodName << endl;
}
       
// Name: Sweet Potato
// Keywords: sweet potato, root vegetable
// Calories: 86
// Categories: Vegetables
// Nutrients: beta-carotene, fiber, vitamin A, potassium

