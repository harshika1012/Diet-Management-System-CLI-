Overview :
    This program is a calorie tracking system that allows the users to manage their daily  food consumption, track calorie intake and maintain a food log.
    It supports basic and conposite food items, user registration, login, profile updates. The program calculates the calorie intake based on user details and activity levels.

Compilation :
    g++ -o calorie main.cpp -lcurl
Running the program :
    ./calorie

Features:

1. Register User:
    Select option 1 from menu.
    Enter the user details like name, emain, password, gender, height, weight, age and activityLevel.
    The program validates the input and stores the user in "users.json".

2. Login User:
    Select option 2 from the menu.
    Enter your email and password.
    Upon successful login, the program calculates your calorie intake and loads your food log.

3. Update Profile:
    Select option 3 from the menu.
    Update your age, weight, and activity level.
    The program recalculates your calorie intake based on the updated details.

4. Add Basic Food:
    Select option 4 from the menu.
    Enter details like name, keywords, calories, categories, and nutrients.
    The program adds the food to basic_foods.json.

5. Add Composite Food:
    Select option 5 from the menu.
    Add multiple basic or composite food items to create a composite food.
    The program calculates the total calories and stores the food in composite_foods.json.

6. Show Basic Foods:
    Select option 6 from the menu.
    Displays all basic foods stored in basic_foods.json.

7. Show Composite Foods:
    Select option 7 from the menu.
    Displays all composite foods stored in composite_foods.json.

8. Show Calorie Intake:
    Select option 8 from the menu.
    Enter the method (M1, M2, or M3) to view your calculated calorie intake.

9. Add Food to Daily Log:
    Select option 9 from the menu.
    Choose a basic or composite food and specify the number of servings.
    The program updates your daily food log.

10. Search Foods by Keywords:
    Select option 10 from the menu.
    Enter keywords to search for foods in basic_foods.json and composite_foods.json.

11. Show Today's Food Log:
    Select option 11 from the menu.
    Displays the food log for the current date.

12. Show Food Log by Date:
    Select option 12 from the menu.
    Enter a specific date to view the food log for that day.

13. Save Food Log:
    Select option 13 from the menu to save the current food log to a file.

14. Delete Food From Log:
    Select option 14 from the menu.
    Enter the index of the food item to delete it from the log.

15. Update the Food Log:
    Select option 15 from the menu.
    Update servings, food name, or food ID for a specific log entry.

16. View Calorie Details for Date:
    Select option 16 from the menu.
    Enter a date and method to view calorie details for that day.
    It also calculates the difference between the calories intake on that day and actually need to be taken according to their height, weight, age, activityLevel.

17. Undo:
    Select option 17 to undo the last action performed on the food log.

18. Logout:
    Select option 18 from the menu to log out.

19. Exit:
    Select option 19 to exit the program.

20. Search and Add USDA Food:
    Select option 20 to get the food items there in USDA page by searching using keywords and add it in food database.

Files :
    basic_foods.json : Stores the basic food Items.
    composite_foods.json : Stores the composite food items.
    users.json : Stores the user deatils as soon as they register.
    calories_intake.json : Stores calculated calories intake for the users based on 3 methods.
    food_log<userId>.txt : Stores the food log for each user.
 

Note:
   The assignment states that the design must clearly show how it can be easily extended to handle an 
   additional website without changes rippling through the system.

    In our implementation, we are using an API to retrieve information from the USDA website. 
    The design is flexible, allowing for changes such as updating the API key or switching to a different API endpoint, 
    which would enable integration with other websites with minimal impact on the existing system.

    However, since the assignment does not require the actual implementation, and the API integration is not the core focus, 
    we have not included these details in the UML or Sequence Diagrams. 
    Our diagrams focus on the overall architecture and flow as per the assignment requirements, 
    but the implementation is already structured to support such extensibility.