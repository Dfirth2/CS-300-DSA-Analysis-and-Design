//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Dayne R Firth II
// Version     : 1.0
// Copyright   : 4/14/2025 DayneFirth
// Description : SNHU Project two create a program that stores course 
//               displays the information and is searchable
//============================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;


// Structure to store course information
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

// Hash table class
class HashTable {
private:
    static const int TABLE_SIZE = 179;
    vector<vector<Course>> table;

    int hash(string courseId) {
        int hashVal = 0;
        for (char c : courseId) {
            hashVal += c;
        }
        return hashVal % TABLE_SIZE;
    }

public:
    HashTable() {
        table.resize(TABLE_SIZE);
    }

    void insert(Course course) {
        int key = hash(course.courseNumber);
        table[key].push_back(course);
    }

    Course* search(string courseNumber) {
        int key = hash(courseNumber);
        for (auto& course : table[key]) {
            if (course.courseNumber == courseNumber) {
                return &course;
            }
        }
        return nullptr;
    }

    vector<Course> getAllCourses() {
        vector<Course> allCourses;
        for (auto& bucket : table) {
            for (auto& course : bucket) {
                allCourses.push_back(course);
            }
        }
        return allCourses;
    }
};

// Global instance of the hash table
HashTable courses;

//Sanitize input to upper case running into errors where i would forgot the capitalize
string toUpper(string str) {
    for (size_t i = 0; i < str.length(); ++i) {
        str[i] = toupper(static_cast<unsigned char>(str[i]));
    }
    return str;
}


// Function to insert into the hash table
void insertCourseHashTable(string number, string name, string prereq1, string prereq2) {
    Course course;
    course.courseNumber = number;
    course.courseName = name;

    if (prereq1 != "NONE") course.prerequisites.push_back(prereq1);
    if (prereq2 != "NONE") course.prerequisites.push_back(prereq2);

    courses.insert(course);
}

// Function to read and parse the course data file
void parseCourseHash(string filename, bool& loadedFlag) {
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cout << "ERROR: file cannot be read." << endl;
        return;
    }

    while (getline(file, line)) {
        vector<string> lineData;
        stringstream ss(line);
        string token;

        while (getline(ss, token, ',')) {
            lineData.push_back(token);
        }

        if (lineData.size() < 2) {
            cout << "'" << line << "' is formatted incorrectly" << endl;
            continue;
        }

        string courseNumber = lineData[0];
        string courseName = lineData[1];

        string prereq1 = (lineData.size() > 2 && lineData[2] != "null") ? lineData[2] : "NONE";
        string prereq2 = (lineData.size() > 3 && lineData[3] != "null") ? lineData[3] : "NONE";

        insertCourseHashTable(courseNumber, courseName, prereq1, prereq2);
    }

    file.close();
    cout << "Courses loaded successfully." << endl;
    loadedFlag = true; // sets the loaded flag
}

// Function to display all courses
void printCourseList() {
    vector<Course> allCourses = courses.getAllCourses();
    sort(allCourses.begin(), allCourses.end(), [](Course a, Course b) {
        return a.courseNumber < b.courseNumber;
        });

    for (const auto& course : allCourses) {
        cout << course.courseNumber << ", " << course.courseName << endl;
    }
}

// Function to search for and display a single course
void searchCourse(string courseNumber) {
    Course* course = courses.search(courseNumber);
    if (!course) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseName << endl;

    // Print prerequisites line
    cout << "Prerequisites: ";
    if (course->prerequisites.empty()) {
        cout << "NONE" << endl;
    }
    else {
        // Handle case where vector has empty strings
        bool printed = false;
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            if (!course->prerequisites[i].empty()) {
                if (printed) cout << ", ";
                cout << course->prerequisites[i];
                printed = true;
            }
        }
        if (!printed) {
            cout << "NONE";
        }
        cout << endl;
    }
}


// Menu-driven program
int main() {
    int choice;
    string filename, courseId;
    bool coursesLoaded = false;

    //welcome note
    cout << "Welcome to the ABCU Course Planner" << endl;

    do {
        cout << "\nMenu:\n"
            << "1. Load Data Structure\n"
            << "2. Print Course List\n"
            << "3. Print Course\n"
            << "9. Exit\n"
            << "Enter choice: ";

        //created because accidently entering the course Id in the main menu and the program errored infinitly
        if (!(cin >> choice)) {
            cin.clear(); // Reset error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input
            cout << choice << " is not a valid input. Please enter a number from the menu." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            parseCourseHash("CS 300 ABCU_Advising_Program_Input.csv", coursesLoaded);
            break;
        case 2:
            if (!coursesLoaded) { //checks the if the courses were loaded
                cout << "Courses have not been loaded, please load Courses (Option 1)." << endl;
            }
            else {
                printCourseList();
            }
            break;
        case 3:
            if (!coursesLoaded) { //checks the if the courses were loaded
                cout << "Courses have not been loaded, please load Courses (Option 1)." << endl;
            }
            else {
                cout << "Enter course number (e.g., CS101): ";
                cin >> courseId;
                courseId = toUpper(courseId);  // Sanitize input to all caps
                searchCourse(courseId); 
            }
            break;
        case 9:
            cout << "Thank you for using the ABCU Course Planner" << endl;
            cout << "Exiting program..." << endl;
            break;
        default:
            cout << choice <<" is not a valid option. Try again." << endl;
        }
    } while (choice != 9);

    return 0;
}