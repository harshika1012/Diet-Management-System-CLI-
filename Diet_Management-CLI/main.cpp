#include "calories.h"
// #include "database.cpp"
#include "users.cpp"
#include "food.cpp"
#include "logs.cpp"
#include "api_food.cpp"
// string UserName = "root";
// string sqlPassword = "Jay@1573";

Logs *userLogs = nullptr;

void handleSignal(int signum)
{
    cout << "\nSignal (" << signum << ") received. Saving log before exiting..." << endl;
    if (userLogs)
    {
        userLogs->saveFoodLog();
        delete userLogs;
    }
    exit(signum); // Exit program
}

void mapToFunctions()
{
    int choice;
    int userId = 0;
    // Logs userLogs(userId);
    // Logs* userLogs = nullptr;
    while (true)
    {
        cout << "Enter your choice: ";
        // cin >> choice;
        while(!(cin >> choice) || choice < 1){
            cout << "Invalid choice. Please enter a valid choice: ";
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        }
        if(userId == 0 && choice != 1 && choice != 2 && choice != 19 ){
                cout << "No user logged in. Log in." << endl;
                continue;
            }
        if (choice == 1)
        {
            if(userId != 0){
                cout << "Already logged in. Please log out first." << endl;
                continue;
            }
            """ Register User """;
            registerUser();
            // exportDatabaseToFile("database_export.txt");
        }
        else if (choice == 2)
        {
            if(userId != 0){
                cout << "Already logged in. Please log out first." << endl;
                continue;
            }
            bool res = LoginUser(&userId);
            if(res == 0){
                cout << "Login failed. Please try again." << endl;
                continue;
            }
            else{
                cout << "Login successful!" << endl;
            }
            // exportDatabaseToFile("database_export.txt");
            cout << "User ID: " << userId << endl;
            if (userLogs != nullptr)
            {
                delete userLogs; 
            }
            userLogs = new Logs(userId);
             userLogs->saveFoodLog();
        }
        else if (choice == 3)
        {
            """ Update Profile """;
            updateProfile();
            // exportDatabaseToFile("database_export.txt");
        }
        else if (choice == 4)
        {
            """ Add Basic Food """;
            input_addBasicFood();
            // exportDatabaseToFile("database_export.txt");
        }
        else if (choice == 5)
        {
            """ Add Composite Food """;
            input_addCompositeFood();
            // exportDatabaseToFile("database_export.txt");
        }
        else if (choice == 6)
        {
            """ Show Basic Foods """;
            cout << "Show Basic Foods" << endl;
            Items items;
            items.showBasicFoods();
        }
        else if (choice == 7)
        {
            """ Show Composite Foods """;
            Items items;
            items.showCompositeFoods();
        }
        else if (choice == 8)
        {
            """ Show Calories Intake """;
            string inputMethod;
            cout << "Enter the method (M1 or M2 or M3): ";
            cin >> inputMethod;
            viewCalorieIntake(inputMethod);
        }
        else if (choice == 9)
        {
            """ Add Food to Daily Log """;
            Items items;
            cout << "\nAdd Food to Daily Log\n";
            cout << "1. Choose from Basic Foods\n";
            cout << "2. Choose from Composite Foods\n";
            cout << "3. Search by Keywords\n";
            cout << "Enter your foodChoice: ";

            int foodChoice;
            string fullId;
            // cin >> foodChoice;
            while(!(cin >> foodChoice) || foodChoice < 1 || foodChoice > 3){
                cout << "Invalid choice. Please enter a valid choice: ";
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cin.ignore();
            if(foodChoice == 1) {
                items.showBasicFoods();
                while(true) {
                    cout << "Enter food ID (B-1 for Basic Food ID 1): ";
                    cin >> fullId;
                    if(fullId[0] != 'B' && fullId[0] != 'b'){
                        cout << "Invalid food ID format. Use B-# for Basic Foods or C-# for Composite Foods." << endl;
                        continue;
                    }
                    break;
                }
            }
            else if(foodChoice == 2) {
                items.showCompositeFoods();
                while(true) {
                    cout << "Enter food ID (C-2 for Composite Food ID 2): ";
                    cin >> fullId;
                    if(fullId[0] != 'C' && fullId[0] != 'c'){
                        cout << "Invalid food ID format. Use B-# for Basic Foods or C-# for Composite Foods." << endl;
                        continue;
                    }
                    break;
                }
            }
            else if(foodChoice == 3) {
                cout << "Enter keywords (comma-separated): ";
                cin.ignore();
                string keywords;
                getline(cin, keywords);
                cout << "Match (1) ALL keywords or (2) ANY keyword? ";
                int matchChoice;
                cin >> matchChoice;
                cout << "Searching for foods..." << endl;
                vector<string> searchResults;

                // Capture the output of searchFoodsByKeywords
                streambuf* originalCoutBuffer = cout.rdbuf();
                ostringstream capturedOutput;
                cout.rdbuf(capturedOutput.rdbuf());

                userLogs->searchFoodsByKeywords(keywords, matchChoice == 1);

                // Restore the original cout buffer
                cout.rdbuf(originalCoutBuffer);

                // Display the captured output
                cout << capturedOutput.str();

                userLogs->searchFoodsByKeywords(keywords, matchChoice == 1);

                cout.rdbuf(originalCoutBuffer); // Restore original cout buffer

                // Parse captured output to extract food IDs
                string output = capturedOutput.str();
                istringstream outputStream(output);
                string line;
                while (getline(outputStream, line)) {
                    if (line.find("| B-") != string::npos || line.find("| C-") != string::npos) {
                        size_t idStart = line.find("|") + 2;
                        size_t idEnd = line.find(" ", idStart);
                        searchResults.push_back(line.substr(idStart, idEnd - idStart));
                    }
                }

                cout << "Enter food ID (B-1 for Basic Food ID 1): ";
                cin >> fullId;

                // Check if the entered food ID is in the search results
                bool isValidFoodId = false;
                for (const auto& result : searchResults) {
                    if (result == fullId) {
                        isValidFoodId = true;
                        break;
                    }
                }

                if (!isValidFoodId) {
                    cout << "Invalid food ID. The entered food ID is not in the search results." << endl;
                    continue;
                }

                if (fullId[0] != 'B' && fullId[0] != 'b' && fullId[0] != 'C' && fullId[0] != 'c') {
                    cout << "Invalid food ID format. Use B-# for Basic Foods or C-# for Composite Foods." << endl;
                    continue;
                }
            }
            else {
                cout << "Invalid choice." << endl;
                continue;
            }

            bool isBasicFood = false;
            int foodId = 0;
            cout << "Food ID: " << fullId << endl;

            if (fullId.size() >= 3 && (fullId[0] == 'B' || fullId[0] == 'b') && fullId[1] == '-')
            {
                isBasicFood = true;
                try
                {
                    foodId = stoi(fullId.substr(2));
                }
                catch (...)
                {
                    cout << "Invalid food ID format." << endl;
                    continue;
                }

                if (!items.check_if_basic_food_exists(foodId))
                {
                    cout << "Basic food with ID " << foodId << " does not exist." << endl;
                    continue;
                }
            }
            else if (fullId.size() >= 3 && (fullId[0] == 'C' || fullId[0] == 'c') && fullId[1] == '-')
            {
                isBasicFood = false;
                try
                {
                    foodId = stoi(fullId.substr(2));
                }
                catch (...)
                {
                    cout << "Invalid food ID format." << endl;
                    continue;
                }

                if (!items.check_if_composite_food_exists(foodId))
                {
                    cout << "Composite food with ID " << foodId << " does not exist." << endl;
                    continue;
                }
            }
            else 
            {
                cout << "Invalid food ID format. Use B-# for Basic Foods or C-# for Composite Foods." << endl;
                continue;
            }

            cout << "Enter number of servings: ";
            int servings;
            while (!(cin >> servings) || servings <= 0)
            {
                cout << "Invalid input. Please enter a positive integer for the number of servings: ";
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            }

            if (servings <= 0)
            {
                cout << "Number of servings must be positive." << endl;
                continue;
            }

            userLogs->addFoodToLog(foodId, isBasicFood, servings);
            // break;
        }
        else if (choice == 10)
        {
            """ Search Foods by Keywords """;
            // Logs userLogs(userId);
            cout << "Enter keywords (comma-separated): ";
            string keywords;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            getline(cin, keywords);

            cout << "Match (1) ALL keywords or (2) ANY keyword? ";
            int matchChoice;
            cin >> matchChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            userLogs->searchFoodsByKeywords(keywords, matchChoice == 1);
        }
        else if (choice == 11)
        {
            """ Show Today's Food Log """;
            // Logs userLogs(userId);
            userLogs->showFoodLog(userLogs->getCurrentDate());
            // break;
        }
        else if (choice == 12)
        {
            """ Show Food Log by Date """;
            // Logs userLogs(userId);
            cout << "Enter date (YYYY-MM-DD) or leave empty for all entries: ";
            string date;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, date);
            userLogs->showFoodLog(date);
            // break;
        }
        else if (choice == 13)
        {
            """ Save Food Log """;
            // Logs userLogs(userId);
            userLogs->saveFoodLog();
            // break;
        }
        else if (choice == 14)
        {
            """ Delete Food From Log """;
            int idx;
            userLogs->showFoodLog();
            cout << "Enter the index of food that you want to delete: ";
            cin >> idx;
            userLogs->deleteFoodFromLog(idx);
        }
        else if (choice == 15)
        {
            """ Update the Food Log """;
            cout << "Update the Food Log" << endl;
            cout << "Choose from the following options: " << endl;
            cout << "1. Update the servings of a food item." << endl;
            cout << "2. Update the name of a food item." << endl;
            cout << "3. Update the Food ID of a food item." << endl;
            cout << "4. Update the servings of a food item without deleting it." << endl;
            cout << "Enter your Updatechoice: ";
            while(!(cin >> choice) || choice < 1 || choice > 4){
                cout << "Invalid choice. Please enter a valid choice: ";
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            }
            if(choice == 1 || choice == 4) {
                userLogs->showFoodLog();
                int idx, servings;
                cout << "Enter the index of Food in FoodLog to update: ";
                // cin >> idx;
                while(!(cin >> idx) || idx < 0){
                    cout << "Invalid choice. Please enter a valid choice: ";
                    cin.clear(); // Clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                }
                cout << "Enter the new servings: ";
                // cin >> servings;
                while(!(cin >> servings) || servings <= 0){
                    cout << "Invalid choice. Please enter a valid choice: ";
                    cin.clear(); // Clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                }
                if(choice==4){
                    userLogs->updateFoodLogWithoutDelete(idx, servings);
                    cout << "Update the FoodLog." << endl;
                }
                else{
                    userLogs->updateFoodLog(idx, servings);
                    cout << "Update the FoodLog." << endl;
                }
            }
            else if(choice == 2)
            {
                userLogs->showFoodLog();
                int idx;
                string newName;
                cout << "Enter the index of Food in FoodLog to update: ";
                // cin >> idx;
                while(!(cin >> idx) || idx < 0){
                    cout << "Invalid choice. Please enter a valid choice: ";
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                cout << "Enter the new name: ";
                // cin >> servings;
                cin >> newName;
                userLogs->updateFoodName(idx, newName);
                // userLogs->updateFoodLog(idx, servings);
                cout << "Update the FoodLog." << endl;
            }
            else if(choice == 3)
            {
                userLogs->showFoodLog();
                int idx;
                string newFoodId;
                cout << "Enter the index of Food in FoodLog to update: ";
                // cin >> idx;
                while(!(cin >> idx) || idx < 0){
                    cout << "Invalid choice. Please enter a valid choice: ";
                    cin.clear(); // Clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                }
                cout << "Enter the new Food ID: ";
                cin >> newFoodId;
                userLogs->updateFoodLogByFoodId(idx, newFoodId);
                cout << "Update the FoodLog." << endl;
            }
            else
            {
                cout << "Enter a valid choice" << endl;  
            }
        }
        else if(choice == 16) {
            """ View Calorie Details for Date """;
            viewCalorieDetailsForDate();
        }
        
        else if(choice == 17){
            """ Undo """;
            userLogs->undo();
            userLogs->saveFoodLog();
            cout << "Undo successful." << endl;
        }
        else if (choice == 18)
        {
            """ Logout """;
            userId = 0;
            logout();
        }
        else if (choice == 19)
        {
            """ Exit """;
            cout << "Exiting the program." << endl;
            break;
        }
        else if(choice == 20){
            searchAndAddUsdaFood();
        }
        else
        {
            cout << "Enter a valid choice" << endl;
        }
        
    }
}

void showDetails()
{
    // Show User Options
    cout << "1. Register User" << endl;
    cout << "2. Login User" << endl;
    cout << "3. Update Profile" << endl;
    cout << "4. Add Basic Food" << endl;
    cout << "5. Add Composite Food" << endl;
    cout << "6. Show Basic Foods" << endl;
    cout << "7. Show Composite Foods" << endl;
    cout << "8. Show Calories Intake" << endl;
    cout << "9. Add food to Daily log." << endl;
    cout << "10. Search Foods by Keywords" << endl;
    cout << "11. Show Today's Food Log" << endl;
    cout << "12. Show Food Log by Date" << endl;
    cout << "13. Save Food Log" << endl;
    cout << "14. Delete Food From Log" << endl;
    cout << "15. Update the Food Log" << endl;
    cout << "16. View Calorie Details for Date" << endl;
    cout << "17. Undo" << endl;
    cout << "18. Logout" << endl;
    cout << "19. Exit" << endl;
    cout << "20. Search and Add USDA Food" << endl;
    mapToFunctions();
}

int main()
{
    signal(SIGINT, handleSignal);  // Ctrl + C
    signal(SIGTSTP, handleSignal); // Handle Ctrl+Z (SIGTSTP)
    signal(SIGHUP, handleSignal);  // Handle Terminal Close (SIGHUP)
    signal(SIGTERM, handleSignal); // kill PID
    signal(SIGSEGV, handleSignal); // Segmentation Fault
    signal(SIGABRT, handleSignal); // Abort signal
    signal(SIGQUIT, handleSignal); // Ctrl+'\'
    // createDatabaseAndTables();
    // exportDatabaseToFile("database_export.txt");
    showDetails();
    return 0;
}