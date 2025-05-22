#include "calories.h"
// #include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// File path for JSON storage
const string USERS_FILE = "users.json";
const string CALORIES_FILE = "calories_intake.json";

class User {
private:
    string name;
    string password;
    string email;
    string gender; 
    double height;
    double weight;
    int age;
    string activityLevel;
    int userId;

public:
    User(string name, 
        string password, 
        string email,
        string gender, 
        double height,
        double weight,
        int age, 
        string activityLevel) 
        : name(name), password(password), email(email), gender(gender), 
          height(height), weight(weight), age(age), activityLevel(activityLevel), userId(-1) {}

    void registerUser() {
        try {
            json users = loadUsers();
            
            for (const auto& user : users) {
                if (user["email"] == email) {
                    cout << "User with email already exists! Please try with other email." << endl;
                    return;
                }
            }
            
            int newUserId = 1;
            for (const auto& user : users) {
                if (user.contains("userId") && user["userId"] >= newUserId) {
                    newUserId = user["userId"].get<int>() + 1;
                }
            }
            
            json newUser = {
                {"userId", newUserId},
                {"name", name},
                {"password", password},
                {"email", email},
                {"gender", gender},
                {"height", height},
                {"weight", weight},
                {"age", age},
                {"activityLevel", activityLevel}
            };
            
            users.push_back(newUser);            
            saveUsers(users);
            cout << "Registration Successful!!" << endl;
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
    
private:
    json loadUsers() {
        json users = json::array();
        
        if (filesystem::exists(USERS_FILE)) {
            ifstream file(USERS_FILE);
            if (file.is_open()) {
                try {
                    file >> users;
                } 
                catch (const json::parse_error& e) {
                    cerr << "JSON parse error: " << e.what() << endl;
                }
                file.close();
            }
        }
        
        return users;
    }
    
    void saveUsers(const json& users) {
        ofstream file(USERS_FILE);
        if (file.is_open()) {
            file << users.dump(4);
            file.close();
        } else {
            throw runtime_error("Unable to open users file for writing");
        }
    }
};

bool isValidEmail(string &email) {
    regex pattern(R"(^[a-zA-Z0-9._%+-]+@(gmail\.com|email\.com)$)");    
    return std::regex_match(email, pattern);
}

void registerUser() {
    string name, password, email, activityLevel;
    string gender;
    double height, weight;
    int age; 
    
    cout << "Enter the username: ";
    cin >> name;

    cout << "Enter the password: ";
    cin >> password;

    while(true) {
        cout << "Enter the email: ";
        cin >> email;

        if(isValidEmail(email)) {
            break;
        }
        cout << "Invalid email! Enter the valid email." << endl;
    }
    
    while(true) {
        cout << "Enter the Gender: ";
        cin >> gender;
        if(gender == "F" || gender == "M") {
            break;
        }
        cout << "Invalid Gender ! Please Enter F or M" << endl;
    }
    
    while(true) {
        cout << "Enter the height: ";
        cin >> height;

        if(cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout << "Invalid height! Please enter the correct height." << endl;
        }
        else {
            break;
        }
    }

    while(true) {
        cout << "Enter the weight: ";
        cin >> weight;

        if(cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout << "Invalid weight! Please enter the correct weight." << endl;
        }
        else {
            break;
        }
    }

    while(true) {
        age = 0;
        cout << "Enter the Age: ";

        if(!(cin >> age)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid Age! Please enter a valid integer." << endl;
            continue;
        }

        char extra;
        if (cin.get(extra) && !isspace(extra)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid Age! Please enter a valid integer." << endl;
            continue;
        }

        if (age < 0 || age > 150) { 
            cout << "Invalid Age! Please enter a realistic age (0-150)." << endl;
            continue;
        }
        break;
    }
    
    while(true) {
        cout << "Enter the ActivityLevel: ";
        getline(cin, activityLevel);
        if(activityLevel == "Sedentary" || activityLevel == "Lightly Active" || activityLevel == "Moderately Active" || activityLevel == "Very Active" || activityLevel == "Super Active") {
            break;
        }
        cout << "Invalid Activity Level ! Please enter valid one" << endl;
    }

    User user(name, password, email, gender, height, weight, age, activityLevel);
    user.registerUser();
    // CalculateCalorieIntake();
}

string currentUserEmail = "";

bool LoginUser(int* userId) {
    string email, password;
    cout << "Enter the email: ";
    cin >> email;

    cout << "Enter the password: ";
    cin >> password;

    try {
        json users = json::array();
        ifstream file(USERS_FILE);
        if (file.is_open()) {
            file >> users;
            file.close();
        } else {
            cout << "No users found. Please register first." << endl;
            return false;
        }
        
        for (const auto& user : users) {
            if (user["email"] == email) {
                string storedPassword = user["password"];
                if (storedPassword == password) {
                    cout << "Login Successful!!" << endl;
                    currentUserEmail = email;
                    // *userId = user["userId"];
                    if (user.contains("userId") && user["userId"].is_number()) {
                        *userId = user["userId"];
                    } else {
                        cout << "Error: Invalid or missing userId in the user data." << endl;
                        return false;
                    }
                    CalculateCalorieIntake();
                    return true;
                } else {
                    cout << "Invalid Password! Please enter the correct password." << endl;
                    return false;
                }
            }
        }
        
        cout << "User doesn't exist" << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    
    return false;
}

void logout() {
    if(!currentUserEmail.empty()) {
        cout << "Logging out " << currentUserEmail << "...." << endl;
        currentUserEmail = "";
        cout << "Logged out" << endl;
    }
    else {
        cout << "No user currently logged in" << endl;
    }
}

// Helper function to load users from JSON file
json loadUsersData() {
    json users = json::array();
    
    if (filesystem::exists(USERS_FILE)) {
        ifstream file(USERS_FILE);
        if (file.is_open()) {
            try {
                file >> users;
            } 
            catch (const json::parse_error& e) {
                cerr << "JSON parse error: " << e.what() << endl;
            }
            file.close();
        }
    }
    
    return users;
}

// Helper function to save users to JSON file
void saveUsersData(const json& users) {
    ofstream file(USERS_FILE);
    if (file.is_open()) {
        file << users.dump(4); 
        file.close();
    } else {
        throw runtime_error("Unable to open users file for writing");
    }
}

// Helper function to load calories data from JSON file
json loadCaloriesData() {
    json calories = json::array();
    
    if (filesystem::exists(CALORIES_FILE)) {
        ifstream file(CALORIES_FILE);
        if (file.is_open()) {
            try {
                file >> calories;
            } 
            catch (const json::parse_error& e) {
                cerr << "JSON parse error: " << e.what() << endl;
            }
            file.close();
        }
    }
    
    return calories;
}

// Helper function to save calories data to JSON file
void saveCaloriesData(const json& calories) {
    ofstream file(CALORIES_FILE);
    if (file.is_open()) {
        file << calories.dump(4); 
        file.close();
    } else {
        throw runtime_error("Unable to open calories file for writing");
    }
}

void updateProfile() {
    if(currentUserEmail.empty()) {
        cout << "No user currently logged in. Login first to update the profile!" << endl;
        return;
    }

    int updated_age;
    double updated_weight;
    string updated_activityLevel;

    cout << "Enter the age: ";
    while(!(cin >> updated_age) || updated_age <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        cout << "Invalid age. Please enter the correct age." << endl;
    }

    cout << "Enter the weight: ";
    while(!(cin >> updated_weight) || updated_weight <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        cout << "Invalid weight. Please enter the correct weight." << endl;
    }
    cin.ignore();

    cout << "Enter new Activity Level : ";
    while (true) {
        getline(cin, updated_activityLevel);
        if (updated_activityLevel == "Sedentary" || updated_activityLevel == "Lightly Active" || 
            updated_activityLevel == "Moderately Active" || updated_activityLevel == "Very Active" || 
            updated_activityLevel == "Super Active") {
            break;
        }
        cout << "Invalid Activity Level! Please enter a valid one: " << endl;
    }

    try {
        json users = loadUsersData();
        bool userFound = false;
        
        for (auto& user : users) {
            if (user["email"] == currentUserEmail) {
                user["age"] = updated_age;
                user["weight"] = updated_weight;
                user["activityLevel"] = updated_activityLevel;
                userFound = true;
                break;
            }
        }
        
        if (userFound) {
            saveUsersData(users);
            cout << "User details updated successfully!" << endl;
        } else {
            cout << "Error in updating the user details! User not found." << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    
    CalculateCalorieIntake();
}

void CalculateCalorieIntake() {
    if(currentUserEmail.empty()) {
        cout << "No user logged in." << endl;
        return;
    }
    
    try {
        json users = loadUsersData();        
        json currentUser;
        int userId = -1;
        bool userFound = false;
        
        for (const auto& user : users) {
            if (user["email"] == currentUserEmail) {
                currentUser = user;
                userId = user["userId"];
                userFound = true;
                break;
            }
        }
        
        if (!userFound) {
            cout << "User not found." << endl;
            return;
        }        
        string gender = currentUser["gender"];
        double height = currentUser["height"];
        double weight = currentUser["weight"];
        int age = currentUser["age"];
        string activityLevel = currentUser["activityLevel"];
        
        double BMR_M1;
        double BMR_M2;
        double BMR_M3;
        
        if(gender == "M") {
            BMR_M1 = (13.7516 * weight) + (5.0033 * height) - (6.755 * age) + 66.473;   // Harris-Benedict BMR
            BMR_M2 = (10 * weight) + (6.25 * height) - (5 * age) + 5;   // Mifflin-St Jeor BMR
            BMR_M3 = (13.397 * weight) + (4.799 * height) - (5.677 * age) + 88.362;   // Katch-McArdle BMR
        }
        else if(gender == "F") {
            BMR_M1 = (9.5634 * weight) + (1.8496 * height) - (4.6756 * age) + 655.0955; // Harris-Benedict BMR
            BMR_M2 = (10 * weight) + (6.25 * height) - (5 * age) - 161; // Mifflin-St Jeor BMR
            BMR_M3 = (9.247 * weight) + (3.098 * height) - (4.330 * age) + 447.593; // Katch-McArdle BMR
        }
        
        double activityFactor;
        if(activityLevel == "Sedentary") {
            activityFactor = 1.2;
        }
        else if(activityLevel == "Lightly Active") {
            activityFactor = 1.375;
        }
        else if(activityLevel == "Moderately Active") {
            activityFactor = 1.55;
        }
        else if(activityLevel == "Very Active") {
            activityFactor = 1.725;
        }
        else if(activityLevel == "Super Active") {
            activityFactor = 1.9;
        }
        else {
            cout << "Invalid Activity Level! Setting by default to Sedentary (1.2)." << endl;
            activityFactor = 1.2; 
        }
        
        // Calculate calorie intake
        double CalorieIntakeDaily_M1 = BMR_M1 * activityFactor;
        double CalorieIntakeDaily_M2 = BMR_M2 * activityFactor;
        double CalorieIntakeDaily_M3 = BMR_M3 * activityFactor;
        
        json calories = loadCaloriesData();
        
        bool userCalorieExists = false;
        
        for (auto& entry : calories) {
            if (entry["email"] == currentUserEmail) {
                entry["calorieIntakeM1"] = CalorieIntakeDaily_M1;
                entry["calorieIntakeM2"] = CalorieIntakeDaily_M2;
                entry["calorieIntakeM3"] = CalorieIntakeDaily_M3;
                userCalorieExists = true;
                break;
            }
        }
        
        if (!userCalorieExists) {
            json newCalorieEntry = {
                {"userId", userId},
                {"email", currentUserEmail},
                {"calorieIntakeM1", CalorieIntakeDaily_M1},
                {"calorieIntakeM2", CalorieIntakeDaily_M2},
                {"calorieIntakeM3", CalorieIntakeDaily_M3}
            };
            
            calories.push_back(newCalorieEntry);
        }
        
        saveCaloriesData(calories);
        cout << "Calorie Intake stored successfully." << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void viewCalorieIntake(string inputMethod) {
    if(currentUserEmail.empty()) {
        cout << "No user currently logged in. Login first to view the calorie intake." << endl;
        return;
    }
    
    try {
        json calories = loadCaloriesData();        
        bool calorieDataFound = false;
        
        for (const auto& entry : calories) {
            if (entry["email"] == currentUserEmail) {
                if(inputMethod == "M1") {
                    cout << "Calorie Intake for: " << currentUserEmail << endl;
                    cout << "Method1: " << entry["calorieIntakeM1"] << " kcal\n";
                }
                else if(inputMethod == "M2") {
                    cout << "Calorie Intake for: " << currentUserEmail << endl;
                    cout << "Method2: " << entry["calorieIntakeM2"] << " kcal\n";
                }
                else if(inputMethod == "M3") {
                    cout << "Calorie Intake for: " << currentUserEmail << endl;
                    cout << "Method3: " << entry["calorieIntakeM3"] << " kcal\n";
                }
                else {
                    cout << "Invalid input method. Please enter M1, M2, or M3." << endl;
                    return;
                }
                calorieDataFound = true;
                break;
            }
        }
        
        if (!calorieDataFound) {
            cout << "No Calorie Intake data found for the logged in user!" << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}
