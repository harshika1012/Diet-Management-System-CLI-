#include "calories.h"
// #include "food.cpp"
// string "root" = "root";
// string "Jay@1573" = "Jay@1573";
using json = nlohmann::json;

class Logs{
public:
    Logs(int user);
    ~Logs();
    void readFoodLog();
    void saveFoodLog(bool onExit = false);
    void showFoodLog(const string& date = "");
    void addFoodToLog(int foodId, bool isBasicFood, int servings);
    void searchFoodsByKeywords(const string& keywords, bool matchAll);
    string getCurrentDate();
    void deleteFoodFromLog(int index);
    void updateFoodLog(int index, int newServings);
    void updateFoodName(int index, const string& newName);
    void updateFoodLogByFoodId(int index,const string& newFoodId);
    float getTotalCaloriesForDate(const string& date);
    void updateFoodLogWithoutDelete(int index, int newServings); 
    void undo();

private:
     struct LogEntry {
        string date;
        int foodId;
        bool isBasicFood;
        string foodName;
        float calories;
        int servings;
    };
    int userlog;
    string logFilePath;
    vector<LogEntry> foodLog;
    stack<vector<LogEntry>> undoStack;

    void saveState();
    // string getCurrentDate();
    // string logFilePath = "food_consumption_log.txt";
};

Logs::Logs(int user) : userlog(user) {
    if (userlog <= 0) {
        throw invalid_argument("Invalid userId provided to Logs constructor.");
    }
    std::ostringstream ss;
    ss << "food_log_" << userlog << ".txt";
    logFilePath = ss.str();
    readFoodLog();
}


// Destructor implementation
Logs ::~Logs() {
}

void Logs::saveState() {
    undoStack.push(foodLog);
}

void Logs::undo() {
    if (!undoStack.empty()) {
        foodLog = undoStack.top();
        undoStack.pop();
        cout << "Undo successful. Restored previous state." << endl;        
        cout << "Current Food Log after Undo:" << endl;
        for (const auto& entry : foodLog) {
            cout << "Food ID: " << entry.foodId << ", Name: " << entry.foodName
                 << ", Calories: " << entry.calories << ", Servings: " << entry.servings << endl;
        }
    } else {
        cout << "No more actions to undo." << endl;
    }
}

string Logs::getCurrentDate() {
    time_t now = time(0);
    tm* localTime = localtime(&now);
    
    stringstream dateStream;
    dateStream << (localTime->tm_year + 1900) << "-" 
               << setfill('0') << setw(2) << (localTime->tm_mon + 1) << "-"
               << setfill('0') << setw(2) << localTime->tm_mday;
               
    return dateStream.str();
}


void Logs::readFoodLog() {
    ifstream logFile(logFilePath);
    if (!logFile) {
        cout << "No existing food log found. Creating a new log." << endl;
        return;
    }
    
    string line;
    while (getline(logFile, line)) {
        stringstream ss(line);
        LogEntry entry;
        string isBasicFoodStr;
        
        // Format: date|foodId|isBasicFood|foodName|calories|servings
        getline(ss, entry.date, '|');
        string foodIdStr;
        getline(ss, foodIdStr, '|');
        entry.foodId = stoi(foodIdStr);
        getline(ss, isBasicFoodStr, '|');
        entry.isBasicFood = (isBasicFoodStr == "1");
        getline(ss, entry.foodName, '|');
        string caloriesStr;
        getline(ss, caloriesStr, '|');
        entry.calories = stof(caloriesStr);
        string servingsStr;
        getline(ss, servingsStr, '|');
        entry.servings = stoi(servingsStr);
        
        foodLog.push_back(entry);
    }
    
    cout << "Food log loaded successfully with " << foodLog.size() << " entries." << endl;
    logFile.close();
}

void Logs::saveFoodLog(bool onExit) {
    cout << "Attempting to save log file at: " << logFilePath << endl;
    
    ofstream logFile(logFilePath);
    if (!logFile) {
        cerr << "Error: Unable to open log file for writing." << endl;
        cerr << "Error details: " << strerror(errno) << endl;
        return;
    }
    
    if (foodLog.empty()) {
        cerr << "Warning: No entries to save." << endl;
        return;
    }
    
    for (const LogEntry& entry : foodLog) {
        logFile << entry.date << "|"
                << entry.foodId << "|"
                << (entry.isBasicFood ? "1" : "0") << "|"
                << entry.foodName << "|"
                << entry.calories << "|"
                << entry.servings << endl;
        
        if (!logFile) {
            cerr << "Error writing entry to log file" << endl;
            break;
        }
    }
    // for (const LogEntry& entry : foodLog) {
    // logFile << "Date          : " << entry.date << endl;
    // logFile << "Food ID       : " << entry.foodId << " (" << (entry.isBasicFood ? "Basic" : "Composite") << ")" << endl;
    // logFile << "Food Name     : " << entry.foodName << endl;
    // logFile << "Calories      : " << entry.calories << " kcal" << endl;
    // logFile << "Servings      : " << entry.servings << endl;
    // logFile << "----------------------------------------" << endl;

    // if (!logFile) {
    //     cerr << "Error writing entry to log file" << endl;
    //     break;
    // }
// }

    
    logFile.close();
    
    if (logFile.good()) {
        cout << "Food log saved successfully." << endl;
    } else {
        cerr << "Error: Problem saving food log" << endl;
    }
}

void Logs::addFoodToLog(int foodId, bool isBasicFood, int servings) {
    if (currentUserEmail.empty()) {
        cout << "No user logged in. Log in first to add the food to the log." << endl;
        return;
    }  

    saveState();
    string currentDate = getCurrentDate();

    try {
        json foodData;
        string foodName;
        float calories = 0.0f;

        if (isBasicFood) {
            // Load basic foods from JSON
            ifstream basicFile("basic_foods.json");
            if (basicFile.is_open()) {
                basicFile >> foodData;
                basicFile.close();
            } else {
                cerr << "Error: Could not open basic_foods.json" << endl;
                return;
            }
            for (const auto& food : foodData) {
                if (food["foodId"] == foodId) {
                    foodName = food["name"].get<string>();
                    calories = food["calories"].get<float>();
                    break;
                }
            }
        } else {
            // Load composite foods from JSON
            ifstream compositeFile("composite_foods.json");
            if (compositeFile.is_open()) {
                compositeFile >> foodData;
                compositeFile.close();
            } else {
                cerr << "Error: Could not open composite_foods.json" << endl;
                return;
            }
            for (const auto& food : foodData) {
                if (food["foodId"] == foodId) {
                    foodName = food["name"].get<string>();
                    calories = food["totalCalories"].get<float>();
                    break;
                }
            }
        }

        if (!foodName.empty()) {
            // Check if this food already exists for today
            bool foodUpdated = false;
            for (auto& entry : foodLog) {
                if (entry.date == currentDate && entry.foodId == foodId && entry.isBasicFood == isBasicFood) {
                    // Update existing entry
                    entry.servings += servings;
                    entry.calories += (calories * servings);
                    
                    cout << "Updated " << foodName << " in your food log for today (" 
                         << currentDate << "). New total: " << entry.servings 
                         << " serving(s), " << entry.calories << " calories." << endl;
                    
                    foodUpdated = true;
                    break;
                }
            }
            
            // If food wasn't found for today, add a new entry
            if (!foodUpdated) {
                LogEntry newEntry;
                newEntry.date = currentDate;
                newEntry.foodId = foodId;
                newEntry.foodName = foodName;
                newEntry.isBasicFood = isBasicFood;
                newEntry.servings = servings;
                newEntry.calories = calories * servings;
                foodLog.push_back(newEntry);

                cout << servings << " serving(s) of " << newEntry.foodName
                     << " added to your food log for today (" << newEntry.date
                     << "), totaling " << newEntry.calories << " calories." << endl;
            }

           cout << "Current food log size: " << foodLog.size() << endl;
            for (const LogEntry& entry : foodLog) {
                cout << "Food ID: " << entry.foodId 
                     << ", Name: " << entry.foodName 
                     << ", Calories: " << entry.calories 
                     << ", Servings: " << entry.servings << endl;
            }
        } else {
            cout << "Error: Food item not found." << endl;
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}


void Logs::showFoodLog(const string& date) {
    if(currentUserEmail.empty()) {
        cout << "No user logged in. Log in first to show the foodlog." << endl;
        return;
    }
    
    ifstream logFile(logFilePath);
    if (!logFile) {
        cout << "No food log file found." << endl;
        return;
    }
    // if(currentUserEmail.empty()) {
    //     cout << "No user logged in. Log in first to show the foodlog." << endl;
    //     return;
    // }

    float totalCalories = 0.0;
    int entriesShown = 0;
    vector<LogEntry> displayedIndexes;

    cout << "\n===== Food Consumption Log ";
    if (!date.empty()) {
        cout << "for " << date;
    }
    cout << " =====\n" << endl;

    cout << "+-----+------------+---------------------------+----------+----------+----------+" << endl;
    cout << "| Idx | Date       | Food Name                 | Servings | Calories | Food ID  |" << endl;
    cout << "+-----+------------+---------------------------+----------+----------+----------+" << endl;

    string line;
    int lineIndex = 0;
    while (getline(logFile, line)) {
        stringstream ss(line);
        string entryDate, foodName, isBasicFoodStr, foodIdStr, caloriesStr, servingsStr;
        LogEntry entry;

        // Parse the log entry: date|foodId|isBasicFood|foodName|calories|servings
        getline(ss, entryDate, '|');
        getline(ss, foodIdStr, '|');
        getline(ss, isBasicFoodStr, '|');
        getline(ss, foodName, '|');
        getline(ss, caloriesStr, '|');
        getline(ss, servingsStr, '|');

        if (!date.empty() && entryDate != date) {
            continue;
        }

        int foodId = stoi(foodIdStr);
        bool isBasicFood = (isBasicFoodStr == "1");
        float calories = stof(caloriesStr);
        int servings = stoi(servingsStr);

        cout << "| " << left << setw(3) << lineIndex << " | "
             << left << setw(10) << entryDate << " | "
             << left << setw(25) << foodName << " | "
             << right << setw(8) << servings << " | "
             << right << setw(8) << fixed << setprecision(1) << calories << " | "
             << (isBasicFood ? "B" : "C") << "-" << left << setw(6) << foodId << " |" << endl;

        totalCalories += calories;
        entriesShown++;
        lineIndex++;
    }

    cout << "+------------+---------------------------+----------+----------+----------+" << endl;
    cout << "Total Entries: " << entriesShown << ", Total Calories: " << fixed << setprecision(1) << totalCalories << endl;

    logFile.close();

    if (entriesShown == 0 && !date.empty()) {
        cout << "No entries found for the date: " << date << endl;
    }
}

void Logs::searchFoodsByKeywords(const string& keywords, bool matchAll) {
    cout << "Entered" << endl;
    if (currentUserEmail.empty()) {
        cout << "No user logged in. Log in first to search food by keywords." << endl;
        return;
    }

    vector<string> keywordList;
    stringstream ss(keywords);
    string keyword;
    
    while (getline(ss, keyword, ',')) {
        keyword.erase(0, keyword.find_first_not_of(" \t"));
        keyword.erase(keyword.find_last_not_of(" \t") + 1);
        if (!keyword.empty()) {
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
            keywordList.push_back(keyword);
        }
    }
    
    if (keywordList.empty()) {
        cout << "No valid keywords provided." << endl;
        return;
    }
    
    try {
        // Load basic foods
        json basicFoods;
        ifstream basicFile("basic_foods.json");
        if (basicFile.is_open()) {
            basicFile >> basicFoods;
            basicFile.close();
        }

        // Load composite foods
        json compositeFoods;
        ifstream compositeFile("composite_foods.json");
        if (compositeFile.is_open()) {
            compositeFile >> compositeFoods;
            compositeFile.close();
        }

        cout << "\n===== Search Results for Keywords: " << keywords;
        cout << " (Match " << (matchAll ? "ALL" : "ANY") << ") =====\n" << endl;
        
        cout << "+-----+---------+---------------------------+----------+" << endl;
        cout << "| ID  | Type    | Name                      | Calories |" << endl;
        cout << "+-----+---------+---------------------------+----------+" << endl;
        
        int resultCount = 0;
        
        auto toLower = [](const string& str) {
            string lowerStr = str;
            transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
            return lowerStr;
        };
        
        // Search basic foods
        for (const auto& food : basicFoods) {
            string foodKeywords = toLower(food["keywords"].get<string>());
            bool matches = matchAll;
            
            for (const string& kw : keywordList) {
                bool keywordFound = (foodKeywords.find(kw) != string::npos);
                if (matchAll) {
                    matches = matches && keywordFound;
                } else {
                    matches = matches || keywordFound;
                }
            }
            
            if (matches) {
                cout << "| B-" << left << setw(2) << food["foodId"] << " | "
                     << left << setw(7) << "Basic" << " | "
                     << left << setw(25) << food["name"] << " | "
                     << right << setw(8) << fixed << setprecision(1) << food["calories"] << " |" << endl;
                resultCount++;
            }
        }
        
        // Search composite foods
        for (const auto& food : compositeFoods) {
            string foodKeywords = toLower(food["keywords"].get<string>());
            bool matches = matchAll;
            
            for (const string& kw : keywordList) {
                bool keywordFound = (foodKeywords.find(kw) != string::npos);
                if (matchAll) {
                    matches = matches && keywordFound;
                } else {
                    matches = matches || keywordFound;
                }
            }
            
            if (matches) {
                cout << "| C-" << left << setw(2) << food["foodId"] << " | "
                     << left << setw(7) << "Composite" << " | "
                     << left << setw(25) << food["name"] << " | "
                     << right << setw(8) << fixed << setprecision(1) << food["totalCalories"] << " |" << endl;
                resultCount++;
            }
        }
        
        cout << "+-----+---------+---------------------------+----------+" << endl;
        cout << "Total matches: " << resultCount << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void Logs::deleteFoodFromLog(int index) {
    if (index < 0 || index >= foodLog.size()) {
        cout << "Error: Invalid Index. Please enter a valid index." << endl;
        return;
    }
    if (currentUserEmail.empty()) {
        cout << "No user logged in. Log in first to delete the food from the log." << endl;
        return;
    }
    
    saveState();
    LogEntry deleteEntry = foodLog[index];
    
    try {
        foodLog.erase(foodLog.begin() + index);
        saveFoodLog();

        cout << "Successfully Deleted: " 
             << deleteEntry.servings << " serving(s) of " 
             << deleteEntry.foodName 
             << " from " << deleteEntry.date 
             << " (Total " << deleteEntry.calories << " calories)" << endl;

        cout << "Current food log size: " << foodLog.size() << endl;
        
        for (const LogEntry& entry : foodLog) {
            cout << "Food ID: " << entry.foodId 
                 << ", Name: " << entry.foodName 
                 << ", Calories: " << entry.calories 
                 << ", Servings: " << entry.servings << endl;
        }
    } 
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void Logs::updateFoodLog(int index, int newServings) {
    if (currentUserEmail.empty()) {
        cout << "No user logged in. First Login to update the foodLog." << endl;
        return;
    }
    if (index < 0 || index >= foodLog.size()) {
        cout << "Error: Invalid index. Please enter the valid index." << endl;
        return;
    }

    saveState();
    try {
        LogEntry oldEntry = foodLog[index];
        foodLog.erase(foodLog.begin() + index);
        saveFoodLog();

        cout << "Deleted entry: " << oldEntry.foodName 
             << ", Servings: " << oldEntry.servings 
             << ", Calories: " << oldEntry.calories << endl;

        addFoodToLog(oldEntry.foodId, oldEntry.isBasicFood, newServings);
        saveFoodLog(); // Save the updated food log to the file
        cout << "Re-entered entry with updated servings: " << newServings << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}


void Logs::updateFoodLogWithoutDelete(int index, int newServings) {
    if (currentUserEmail.empty()) {
        cout << "No user logged in. First Login to update the foodLog." << endl;
        return;
    }
    
    if (index < 0 || index >= foodLog.size()) {
        cout << "Error: Invalid index. Please enter the valid index." << endl;
        return;
    }

    saveState();
    try {
        LogEntry& entry = foodLog[index];
        float caloriesPerServing = entry.calories / entry.servings;
        
        // Store old values 
        int oldServings = entry.servings;
        float oldCalories = entry.calories;
        
        // Update servings and recalculate calories
        entry.servings = newServings;
        entry.calories = caloriesPerServing * newServings;
        
        // Save changes
        saveFoodLog();
        
        cout << "Updated " << entry.foodName << " from " << oldServings 
             << " serving(s) (" << oldCalories << " calories) to " 
             << newServings << " serving(s) (" << entry.calories << " calories)." << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void Logs::updateFoodName(int index, const string& newName) {
    if (currentUserEmail.empty()) {
        cout << "No user logged in. Log in first to update the food name in the log." << endl;
        return;
    }
    if (index < 0 || index >= foodLog.size()) {
        cout << "Error: Invalid index. Please enter a valid index." << endl;
        return;
    }

    saveState();
    try {
        LogEntry& entry = foodLog[index];
        string oldName = entry.foodName;
        int oldFoodId = entry.foodId;
        float oldCalories = entry.calories;

        // Search for the new food details in the JSON files
        json foodData;
        bool isBasicFood = false;
        int newFoodId = -1;
        float newCalories = 0.0f;

        // Search in basic foods
        ifstream basicFile("basic_foods.json");
        if (basicFile.is_open()) {
            basicFile >> foodData;
            basicFile.close();
            for (const auto& food : foodData) {
                if (food["name"] == newName) {
                    newFoodId = food["foodId"];
                    newCalories = food["calories"];
                    isBasicFood = true;
                    break;
                }
            }
        }

        // If not found in basic foods, search in composite foods
        if (newFoodId == -1) {
            ifstream compositeFile("composite_foods.json");
            if (compositeFile.is_open()) {
                compositeFile >> foodData;
                compositeFile.close();
                for (const auto& food : foodData) {
                    if (food["name"] == newName) {
                        newFoodId = food["foodId"];
                        newCalories = food["totalCalories"];
                        isBasicFood = false;
                        break;
                    }
                }
            }
        }

        if (newFoodId == -1) {
            cout << "Error: Food with the name '" << newName << "' not found in the database." << endl;
            return;
        }

        // Update the log entry
        entry.foodName = newName;
        entry.foodId = newFoodId;
        entry.isBasicFood = isBasicFood;
        entry.calories = newCalories * entry.servings;

        saveFoodLog(); 

        cout << "Successfully updated food log entry:" << endl;
        cout << "Old Name: " << oldName << ", Old Food ID: " << oldFoodId
             << ", Old Calories: " << oldCalories << endl;
        cout << "New Name: " << entry.foodName << ", New Food ID: " << entry.foodId
             << ", New Calories: " << entry.calories << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

// void Logs::updateFoodLogByFoodId(int index, const string& newFoodId) {
//     if (currentUserEmail.empty()) {
//         cout << "No user logged in. Log in first to update the food log by food ID." << endl;
//         return;
//     }
//     if (index < 0 || index >= foodLog.size()) {
//         cout << "Error: Invalid index. Please enter a valid index." << endl;
//         return;
//     }

//     saveState();
//     try {
//         LogEntry& entry = foodLog[index];
//         string oldFoodId = (entry.isBasicFood ? "B-" : "C-") + to_string(entry.foodId);
//         string oldFoodName = entry.foodName;
//         float oldCalories = entry.calories;

//         // Parse the newFoodId to determine type and ID
//         char type = newFoodId[0];
//         int id = stoi(newFoodId.substr(2));
//         if (type != 'B' && type != 'b' && type != 'C' && type != 'c') {
//             cout << "Error: Invalid food ID format. Use 'B-<id>' for basic food or 'C-<id>' for composite food." << endl;
//             return;
//         }

//         // Search for the new food details in the JSON files
//         json foodData;
//         string newFoodName;
//         float newCalories = 0.0f;
//         bool isBasicFood = (type == 'B' || type == 'b');

//         if (isBasicFood) {
//             // Search in basic foods
//             ifstream basicFile("basic_foods.json");
//             if (basicFile.is_open()) {
//                 basicFile >> foodData;
//                 basicFile.close();
//                 for (const auto& food : foodData) {
//                     if (food["foodId"] == id) {
//                         newFoodName = food["name"];
//                         newCalories = food["calories"];
//                         break;
//                     }
//                 }
//             }
//         } else {
//             // Search in composite foods
//             ifstream compositeFile("composite_foods.json");
//             if (compositeFile.is_open()) {
//                 compositeFile >> foodData;
//                 compositeFile.close();
//                 for (const auto& food : foodData) {
//                     if (food["foodId"] == id) {
//                         newFoodName = food["name"];
//                         newCalories = food["totalCalories"];
//                         break;
//                     }
//                 }
//             }
//         }

//         if (newFoodName.empty()) {
//             cout << "Error: Food with ID '" << newFoodId << "' not found." << endl;
//             return;
//         }

//         // Update the log entry
//         entry.foodId = id;
//         entry.foodName = newFoodName;
//         entry.isBasicFood = isBasicFood;
//         entry.calories = newCalories * entry.servings;

//         saveFoodLog(); // Save the updated food log to the file

//         cout << "Successfully updated food log entry:" << endl;
//         cout << "Old Food ID: " << oldFoodId << ", Old Name: " << oldFoodName
//              << ", Old Calories: " << oldCalories << endl;
//         cout << "New Food ID: " << newFoodId << ", New Name: " << entry.foodName
//              << ", New Calories: " << entry.calories << endl;

//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//     }
// }

void Logs::updateFoodLogByFoodId(int index, const string& newFoodId) {
    if (currentUserEmail.empty()) {
        cout << "No user logged in. Log in first to update the food log by food ID." << endl;
        return;
    }
    if (index < 0 || index >= foodLog.size()) {
        cout << "Error: Invalid index. Please enter a valid index." << endl;
        return;
    }

    saveState();
    try {
        LogEntry& entry = foodLog[index];
        string oldFoodId = (entry.isBasicFood ? "B-" : "C-") + to_string(entry.foodId);
        string oldFoodName = entry.foodName;
        float oldCalories = entry.calories;
        if (newFoodId.length() < 3 || newFoodId[1] != '-') {
            cout << "Error: Invalid food ID format. Use 'B-<id>' or 'C-<id>'." << endl;
            return;
        }

        char type = newFoodId[0];
        string idStr = newFoodId.substr(2);

        // idStr contains only digits
        if (idStr.empty() || !all_of(idStr.begin(), idStr.end(), ::isdigit)) {
            cout << "Error: Invalid food ID format. ID must be a number after 'B-' or 'C-'." << endl;
            return;
        }

        int id = stoi(idStr);

        if (type != 'B' && type != 'b' && type != 'C' && type != 'c') {
            cout << "Error: Invalid food ID type. Use 'B-<id>' for basic food or 'C-<id>' for composite food." << endl;
            return;
        }

        json foodData;
        string newFoodName;
        float newCalories = 0.0f;
        bool isBasicFood = (type == 'B' || type == 'b');

        if (isBasicFood) {
            ifstream basicFile("basic_foods.json");
            if (basicFile.is_open()) {
                basicFile >> foodData;
                basicFile.close();
                for (const auto& food : foodData) {
                    if (food["foodId"] == id) {
                        newFoodName = food["name"];
                        newCalories = food["calories"];
                        break;
                    }
                }
            }
        } else {
            ifstream compositeFile("composite_foods.json");
            if (compositeFile.is_open()) {
                compositeFile >> foodData;
                compositeFile.close();
                for (const auto& food : foodData) {
                    if (food["foodId"] == id) {
                        newFoodName = food["name"];
                        newCalories = food["totalCalories"];
                        break;
                    }
                }
            }
        }

        if (newFoodName.empty()) {
            cout << "Error: Food with ID '" << newFoodId << "' not found." << endl;
            return;
        }

        entry.foodId = id;
        entry.foodName = newFoodName;
        entry.isBasicFood = isBasicFood;
        entry.calories = newCalories * entry.servings;

        saveFoodLog();

        cout << "Successfully updated food log entry:" << endl;
        cout << "Old Food ID: " << oldFoodId << ", Old Name: " << oldFoodName
             << ", Old Calories: " << oldCalories << endl;
        cout << "New Food ID: " << newFoodId << ", New Name: " << entry.foodName
             << ", New Calories: " << entry.calories << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

float Logs::getTotalCaloriesForDate(const string& date) {
    float totalCalories = 0.0;

    for (const auto& entry : foodLog) {
        if (entry.date == date) {
            totalCalories += entry.calories * entry.servings;
        }
    }

    return totalCalories;
}

void viewCalorieDetailsForDate() {
    if (currentUserEmail.empty()) {
        cout << "No user currently logged in. Login first to view the calorie details." << endl;
        return;
    }

    try {
        json calories = loadCaloriesData();
        bool calorieDataFound = false;

        for (const auto& entry : calories) {
            if (entry["email"] == currentUserEmail) {
                calorieDataFound = true;

                cout << "Enter the date (YYYY-MM-DD) to view calorie details: ";
                string selectedDate;
                while (true) {
                    cin >> selectedDate;
                    // Validate the date format (YYYY-MM-DD)
                    regex datePattern(R"(\d{4}-\d{2}-\d{2})");
                    if (!regex_match(selectedDate, datePattern)) {
                        cout << "Invalid date format. Please enter the date in YYYY-MM-DD format: ";
                    } else {
                        break;
                    }
                }
                
                cout << "Enter the method (e.g., M1, M2, etc.) to calculate target calorie intake: ";
                string selectedMethod;
                cin >> selectedMethod;

                if (!entry.contains("calorieIntake" + selectedMethod)) {
                    cout << "Invalid method selected. Please try again." << endl;
                    return;
                }

                Logs userLog(entry["userId"].get<int>());
                float totalCaloriesConsumed = userLog.getTotalCaloriesForDate(selectedDate);
                float targetCalorieIntake = entry["calorieIntake" + selectedMethod]; 
                float calorieDifference = targetCalorieIntake - totalCaloriesConsumed;
                cout << "Date: " << selectedDate << endl;
                cout << "Total Calories Consumed: " << totalCaloriesConsumed << " kcal" << endl;
                cout << "Target Calorie Intake (" << selectedMethod << "): " << targetCalorieIntake << " kcal" << endl;
                cout << "Difference: " << calorieDifference << " kcal" << endl;

                if (calorieDifference < 0) {
                    cout << "You have exceeded your calorie intake by " << abs(calorieDifference) << " kcal." << endl;
                } else if (calorieDifference > 0) {
                    cout << "You are under your calorie intake by " << calorieDifference << " kcal." << endl;
                } else {
                    cout << "You have met your calorie intake for the day." << endl;
                }

                break;
            }
        }

        if (!calorieDataFound) {
            cout << "No calorie data found for the current user." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}