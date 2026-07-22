/*
    Student Management System
    --------------------------
    A console-based C++ application demonstrating:
      - Object-Oriented Programming (encapsulation, classes)
      - File handling (persistent storage using a text file)
      - CRUD operations: Add, Update, Delete, Search (+ Display All)

    Compile:  g++ -std=c++17 -o sms StudentManagementSystem.cpp
    Run:      ./sms
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const string DATA_FILE = "students.txt";

// ---------------------------------------------------------
// Student class: encapsulates a single student record
// ---------------------------------------------------------
class Student {
private:
    int id;
    string name;
    int age;
    string course;
    double gpa;

public:
    Student() : id(0), age(0), gpa(0.0) {}

    Student(int id, const string& name, int age, const string& course, double gpa)
        : id(id), name(name), age(age), course(course), gpa(gpa) {}

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getCourse() const { return course; }
    double getGpa() const { return gpa; }

    // Setters (used for update operation)
    void setName(const string& n) { name = n; }
    void setAge(int a) { age = a; }
    void setCourse(const string& c) { course = c; }
    void setGpa(double g) { gpa = g; }

    // Serialize record to a delimited line for file storage
    string toFileLine() const {
        ostringstream oss;
        oss << id << "|" << name << "|" << age << "|" << course << "|" << gpa;
        return oss.str();
    }

    // Parse a delimited line back into a Student object
    static Student fromFileLine(const string& line) {
        stringstream ss(line);
        string field;
        vector<string> tokens;
        while (getline(ss, field, '|')) {
            tokens.push_back(field);
        }
        if (tokens.size() != 5) {
            throw runtime_error("Corrupted record line: " + line);
        }
        return Student(stoi(tokens[0]), tokens[1], stoi(tokens[2]), tokens[3], stod(tokens[4]));
    }

    void display() const {
        cout << left
             << setw(6)  << id
             << setw(20) << name
             << setw(6)  << age
             << setw(15) << course
             << setw(6)  << fixed << setprecision(2) << gpa
             << "\n";
    }
};

// ---------------------------------------------------------
// StudentManager class: handles CRUD + file persistence
// ---------------------------------------------------------
class StudentManager {
private:
    vector<Student> students;
    string filename;

    void save() {
        ofstream out(filename, ios::trunc);
        if (!out) {
            cerr << "Error: could not open file for writing.\n";
            return;
        }
        for (const auto& s : students) {
            out << s.toFileLine() << "\n";
        }
    }

    void load() {
        students.clear();
        ifstream in(filename);
        if (!in) return; // file may not exist yet on first run

        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            try {
                students.push_back(Student::fromFileLine(line));
            } catch (const exception& e) {
                cerr << "Warning: skipping bad line -> " << e.what() << "\n";
            }
        }
    }

    bool idExists(int id) const {
        return find_if(students.begin(), students.end(),
                        [id](const Student& s) { return s.getId() == id; }) != students.end();
    }

public:
    StudentManager(const string& file) : filename(file) {
        load();
    }

    // ---------------- CREATE ----------------
    void addStudent() {
        int id, age;
        string name, course;
        double gpa;

        cout << "Enter Student ID: ";
        cin >> id;
        if (idExists(id)) {
            cout << "A student with ID " << id << " already exists.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Name: ";
        getline(cin, name);

        cout << "Enter Age: ";
        cin >> age;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Course: ";
        getline(cin, course);

        cout << "Enter GPA: ";
        cin >> gpa;

        students.emplace_back(id, name, age, course, gpa);
        save();
        cout << "Student added successfully.\n";
    }

    // ---------------- READ (search) ----------------
    void searchStudent() {
        int id;
        cout << "Enter Student ID to search: ";
        cin >> id;

        auto it = find_if(students.begin(), students.end(),
                           [id](const Student& s) { return s.getId() == id; });

        if (it == students.end()) {
            cout << "No student found with ID " << id << ".\n";
            return;
        }

        cout << left
             << setw(6) << "ID" << setw(20) << "Name" << setw(6) << "Age"
             << setw(15) << "Course" << setw(6) << "GPA" << "\n";
        cout << string(53, '-') << "\n";
        it->display();
    }

    // ---------------- UPDATE ----------------
    void updateStudent() {
        int id;
        cout << "Enter Student ID to update: ";
        cin >> id;

        auto it = find_if(students.begin(), students.end(),
                           [id](const Student& s) { return s.getId() == id; });

        if (it == students.end()) {
            cout << "No student found with ID " << id << ".\n";
            return;
        }

        int choice;
        cout << "What would you like to update?\n";
        cout << "1. Name\n2. Age\n3. Course\n4. GPA\n5. All fields\n";
        cout << "Enter choice: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1 || choice == 5) {
            string name;
            cout << "New Name: ";
            getline(cin, name);
            it->setName(name);
        }
        if (choice == 2 || choice == 5) {
            int age;
            cout << "New Age: ";
            cin >> age;
            it->setAge(age);
        }
        if (choice == 3 || choice == 5) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string course;
            cout << "New Course: ";
            getline(cin, course);
            it->setCourse(course);
        }
        if (choice == 4 || choice == 5) {
            double gpa;
            cout << "New GPA: ";
            cin >> gpa;
            it->setGpa(gpa);
        }

        save();
        cout << "Student record updated successfully.\n";
    }

    // ---------------- DELETE ----------------
    void deleteStudent() {
        int id;
        cout << "Enter Student ID to delete: ";
        cin >> id;

        auto it = find_if(students.begin(), students.end(),
                           [id](const Student& s) { return s.getId() == id; });

        if (it == students.end()) {
            cout << "No student found with ID " << id << ".\n";
            return;
        }

        students.erase(it);
        save();
        cout << "Student deleted successfully.\n";
    }

    // ---------------- DISPLAY ALL ----------------
    void displayAll() const {
        if (students.empty()) {
            cout << "No student records found.\n";
            return;
        }

        cout << left
             << setw(6) << "ID" << setw(20) << "Name" << setw(6) << "Age"
             << setw(15) << "Course" << setw(6) << "GPA" << "\n";
        cout << string(53, '-') << "\n";
        for (const auto& s : students) {
            s.display();
        }
    }
};

// ---------------------------------------------------------
// Menu-driven console interface
// ---------------------------------------------------------
void showMenu() {
    cout << "\n===== Student Management System =====\n";
    cout << "1. Add Student\n";
    cout << "2. Update Student\n";
    cout << "3. Delete Student\n";
    cout << "4. Search Student\n";
    cout << "5. Display All Students\n";
    cout << "6. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    StudentManager manager(DATA_FILE);
    int choice;

    do {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: manager.addStudent(); break;
            case 2: manager.updateStudent(); break;
            case 3: manager.deleteStudent(); break;
            case 4: manager.searchStudent(); break;
            case 5: manager.displayAll(); break;
            case 6: cout << "Exiting... Goodbye!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);

    return 0;
}
