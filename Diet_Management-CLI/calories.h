#ifndef CALORIES_H
#define CALORIES_H

#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h> 
#include <cppconn/resultset.h>
#include <limits>
#include <string.h>
#include <stdbool.h>
#include <regex>
#include <string>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include <csignal>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <regex>
#include <filesystem>


using namespace std;
using namespace sql;
extern string currentUserEmail;

void createDatacaseAndTables();
bool isValidEmail(string &email);
void CalculateCalorieIntake();
void registerUser();
bool LoginUser(int* userId);
void logout();
void updateProfile();
void showUsers();
void CalculateCalorieIntake();
void viewCalorieIntake();
void addBasicFood(const string& name, const string& keywords, float calories, const string& categories, const string& nutrients);
void input_addBasicFood();
void addCompositeFood(const string& name,  string& keywords, string& items,  string& quantities,string& itemIds, float *totalCalories, string& categories, string& nutrients);
void input_addCompositeFood();
void showBasicFoods();
void showCompositeFoods();
int check_if_basic_food_exists(int foodId);
int check_if_composite_food_exists(int foodId);
void add_basic_food_to_composite_food(int foodId, int quantity, string& quantities,string& itemIds, string& itemNames, float *totalCalories, string& categories, string& nutrients);
void add_composite_food_to_composite_food(int foodId, int quantity, string& quantities,string& itemIds, string& itemNames, float *totalCalories, string& categories, string& nutrients);
int BasicFoodsEmpty();
int CompositeFoodsEmpty();
string trim(const string& str);
void saveState();
void undo();
void updateFoodLogWithoutDelete(int index, int newServings); 
void add_to_basicFoods_API( string& foodName,  string& keywords, float calories,  string& categories, string& nutrients);


void readFoodLog();
void saveFoodLog(bool onExit = false);
void showFoodLog(const string& date = "");
void addFoodToLog(int foodId, bool isBasicFood, int servings);
void searchFoodsByKeywords(const string& keywords, bool matchAll);
string getCurrentDate();
// extern string UserName;
// extern string sqlPassword;

#endif