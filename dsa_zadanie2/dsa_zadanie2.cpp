#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <string>
#include <stdexcept>

using namespace std;

// Trieda Task reprezentuje jednu úlohu v zozname
class Task {
private:
    // Atribúty triedy - údaje o úlohe
    string title;        // názov úlohy
    string description;  // popis úlohy
    int priority;       // priorita
    string deadline;    // dátum dokončenia
    bool done;          // stav úlohy

public:
    // Konštruktor - vytvorenie novej úlohy s danými parametrami
    Task(const string &t, const string &d = "", int p = 5,
         const string &dl = "", bool dn = false) {
        title = t;
        description = d;
        priority = p;
        deadline = dl;
        done = dn;
    }

    // Gettery - funkcie na získanie hodnôt atribútov
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    int getPriority() const { return priority; }
    string getDeadline() const { return deadline; }
    bool getDone() const { return done; }

    // Settery - funkcie na nastavenie hodnôt atribútov
    void setDescription(const string &d) { description = d; }
    void setPriority(int p) { priority = p; }
    void setDeadline(const string &dl) { deadline = dl; }
    void setDone(bool d) { done = d; }

    // Funkcia na uloženie úlohy do súboru
    string toFileString() const {
        stringstream ss;
        ss << title << "|" << description << "|" << priority << "|" << deadline << "|" << (done ? "1" : "0");
        return ss.str();
    }

    // Funkcia na načítanie úlohy zo súboru
    static Task fromFileString(const string &data) {
        vector<string> parts;
        string part;
        stringstream ss(data);

        // Rozdelenie reťazca podľa znaku |
        while (getline(ss, part, '|')) {
            parts.push_back(part);
        }

        // Kontrola, či máme všetky potrebné časti
        if (parts.size() != 5) {
            throw runtime_error("Wrong format of data in file");
        }

        // Vytvorenie a vrátenie novej úlohy
        return Task(
            parts[0],                    // názov
            parts[1],                    // popis
            stoi(parts[2]),             // priorita
            parts[3],                    // termín
            parts[4] == "1"             // stav dokončenia
        );
    }
};

// Trieda pre uzol AVL stromu
class AVLNode {
private:
    string date;           // dátum (kľúč)
    vector<Task*> tasks;   // zoznam úloh pre daný dátum
    AVLNode* left;         // ľavý uzol
    AVLNode* right;        // pravý uzol
    int height;            // výška uzla

public:
    AVLNode(const string& d) : date(d), left(nullptr), right(nullptr), height(1) {}

    // Gettery
    string getDate() const { return date; }
    vector<Task*>& getTasks() { return tasks; }
    AVLNode* getLeft() const { return left; }
    AVLNode* getRight() const { return right; }
    int getHeight() const { return height; }

    // Settery
    void setDate(const string& d) { date = d; }
    void setLeft(AVLNode* node) { left = node; }
    void setRight(AVLNode* node) { right = node; }
    void setHeight(int h) { height = h; }

    // Pridanie úlohy do zoznamu
    void addTask(Task* task) {
        tasks.push_back(task);
    }

    // Odstránenie úlohy zo zoznamu
    void removeTask(const string& title) {
        tasks.erase(
            remove_if(tasks.begin(), tasks.end(),
                [&title](Task* task) { return task->getTitle() == title; }
            ),
            tasks.end()
        );
    }
};

// Trieda pre AVL strom
class AVLTree {
private:
    AVLNode* root;

    // Pomocné funkcie pre AVL strom
    int getHeight(AVLNode* node) {
        return node ? node->getHeight() : 0;
    }

    int getBalance(AVLNode* node) {
        return node ? getHeight(node->getLeft()) - getHeight(node->getRight()) : 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->getLeft();
        AVLNode* T2 = x->getRight();

        x->setRight(y);
        y->setLeft(T2);

        y->setHeight(max(getHeight(y->getLeft()), getHeight(y->getRight())) + 1);
        x->setHeight(max(getHeight(x->getLeft()), getHeight(x->getRight())) + 1);

        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->getRight();
        AVLNode* T2 = y->getLeft();

        y->setLeft(x);
        x->setRight(T2);

        x->setHeight(max(getHeight(x->getLeft()), getHeight(x->getRight())) + 1);
        y->setHeight(max(getHeight(y->getLeft()), getHeight(y->getRight())) + 1);

        return y;
    }

    AVLNode* insert(AVLNode* node, const string& key, Task* task) {
        if (!node) return new AVLNode(key);

        if (key < node->getDate())
            node->setLeft(insert(node->getLeft(), key, task));
        else if (key > node->getDate())
            node->setRight(insert(node->getRight(), key, task));
        else {
            node->addTask(task);
            return node;
        }

        node->setHeight(1 + max(getHeight(node->getLeft()), getHeight(node->getRight())));

        int balance = getBalance(node);

        // LL
        if (balance > 1 && key < node->getDate())
            return rightRotate(node);

        // RR
        if (balance < -1 && key > node->getDate())
            return leftRotate(node);

        // LR
        if (balance > 1 && key > node->getDate()) {
            node->setLeft(leftRotate(node->getLeft()));
            return rightRotate(node);
        }

        // RL
        if (balance < -1 && key < node->getDate()) {
            node->setRight(rightRotate(node->getRight()));
            return leftRotate(node);
        }

        return node;
    }

    AVLNode* findMin(AVLNode* node) {
        AVLNode* current = node;
        while (current->getLeft())
            current = current->getLeft();
        return current;
    }

    AVLNode* remove(AVLNode* node, const string& key) {
        if (!node) return node;

        if (key < node->getDate())
            node->setLeft(remove(node->getLeft(), key));
        else if (key > node->getDate())
            node->setRight(remove(node->getRight(), key));
        else {
            if (!node->getLeft() || !node->getRight()) {
                AVLNode* temp = node->getLeft() ? node->getLeft() : node->getRight();

                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else
                    *node = *temp;

                delete temp;
            } else {
                AVLNode* temp = findMin(node->getRight());
                node->setDate(temp->getDate());
                node->setLeft(remove(node->getLeft(), temp->getDate()));
            }
        }

        if (!node) return node;

        node->setHeight(1 + max(getHeight(node->getLeft()), getHeight(node->getRight())));

        int balance = getBalance(node);

        // LL
        if (balance > 1 && getBalance(node->getLeft()) >= 0)
            return rightRotate(node);

        // LR
        if (balance > 1 && getBalance(node->getLeft()) < 0) {
            node->setLeft(leftRotate(node->getLeft()));
            return rightRotate(node);
        }

        // RR
        if (balance < -1 && getBalance(node->getRight()) <= 0)
            return leftRotate(node);

        // RL
        if (balance < -1 && getBalance(node->getRight()) > 0) {
            node->setRight(rightRotate(node->getRight()));
            return leftRotate(node);
        }

        return node;
    }

    void inorderTraversal(AVLNode* node, vector<Task*>& tasks) {
        if (!node) return;

        inorderTraversal(node->getLeft(), tasks);
        tasks.insert(tasks.end(), node->getTasks().begin(), node->getTasks().end());
        inorderTraversal(node->getRight(), tasks);
    }

    void clear(AVLNode* node) {
        if (!node) return;
        clear(node->getLeft());
        clear(node->getRight());
        delete node;
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() {
        clear(root);
    }

    void insert(const string& key, Task* task) {
        root = insert(root, key, task);
    }

    void remove(const string& key) {
        root = remove(root, key);
    }

    // Nájdenie uzla podľa dátumu
    AVLNode* find(const string& date) {
        AVLNode* current = root;
        while (current) {
            if (date < current->getDate())
                current = current->getLeft();
            else if (date > current->getDate())
                current = current->getRight();
            else
                return current;
        }
        return nullptr;
    }

    // Nájdenie úlohy podľa názvu
    Task* findTaskByTitle(const string& title) {
        vector<Task*> allTasks = getAllTasks();
        for (Task* task : allTasks) {
            if (task->getTitle() == title) {
                return task;
            }
        }
        return nullptr;
    }

    vector<Task*> getAllTasks() {
        vector<Task*> tasks;
        inorderTraversal(root, tasks);
        return tasks;
    }
};

// Trieda TaskManager s AVL stromom
class TaskManager {
    private:
    AVLTree taskTree;  // AVL strom pre úlohy
    string dbFile = "tasks.db";  // názov súboru pre ukladanie

    // Načítanie úloh zo súboru
    void loadTasks() {
        ifstream file(dbFile);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            try {
                Task task = Task::fromFileString(line);
                taskTree.insert(task.getDeadline(), new Task(task));
            } catch (const exception& e) {
                continue;
            }
        }
    }

    // Uloženie úloh do súboru
    void saveTasks() {
        ofstream file(dbFile);
        if (!file.is_open()) return;

        vector<Task*> tasks = taskTree.getAllTasks();
        for (const auto& task : tasks) {
            file << task->toFileString() << '\n';
        }
    }

    public:
    // Konštruktor - načíta úlohy zo súboru
    TaskManager() {
        loadTasks();
    }

    // Pridanie novej úlohy
    bool addTask(const string &title, const string &description = "",
                int priority = 5, const string &deadline = "") {
        if (taskTree.findTaskByTitle(title)) {
            return false;
        }

        Task* newTask = new Task(title, description, priority, deadline);
        taskTree.insert(deadline, newTask);
        saveTasks();
        return true;
    }

    // Odstránenie úlohy
    bool removeTask(const string &title) {
        Task* task = taskTree.findTaskByTitle(title);
        if (!task) {
            return false;
        }

        string deadline = task->getDeadline();
        AVLNode* node = taskTree.find(deadline);
        if (node) {
            node->removeTask(title);
        }
        saveTasks();
        return true;
    }

    // Označenie úlohy ako dokončenej
    bool markTaskAsDone(const string &title) {
        Task* task = taskTree.findTaskByTitle(title);
        if (!task) {
            return false;
        }

        task->setDone(true);
        saveTasks();
        return true;
    }

    // Vrátenie zoznamu úloh pre daný dátum
    vector<Task> listTasksByDate(const string &date, bool showCompleted = false) {
        vector<Task> result;
        vector<Task*> tasks = taskTree.getAllTasks();

        for (const auto& task : tasks) {
            if (task->getDeadline() == date && (showCompleted || !task->getDone())) {
                result.push_back(*task);
            }
        }
        return result;
    }

    // Vrátenie zoznamu všetkých úloh
    vector<Task> listAllTasks(bool showCompleted = false) {
        vector<Task> result;
        vector<Task*> tasks = taskTree.getAllTasks();

        for (const auto& task : tasks) {
            if (showCompleted || !task->getDone()) {
                result.push_back(*task);
            }
        }
        return result;
    }
};

// Funkcia na získanie dnešného dátumu
string getTodayDate() {
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);
    return string(buffer);
}

// Funkcia na získanie zajtrajšieho dátumu
string getTomorrowDate() {
    time_t now = time(0) + 24 * 60 * 60; // Pridáme jeden deň
    tm* localTime = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);
    return string(buffer);
}

// Funkcia na výpis help textu
void printHelp() {
    cout << "Usage: zadanie2 [command] [options]" << endl;
    cout << endl;
    cout << "Commands:" << endl;
    cout << "  add     Add a new task" << endl;
    cout << "  remove  Remove a task" << endl;
    cout << "  done    Mark a task as completed" << endl;
    cout << "  list    List tasks" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -h, --help     Show this help message" << endl;
    cout << endl;
    cout << "Command Details:" << endl;
    cout << "  add <-t|--title title> [--desc description] [-p|--priority priority] [-d|--deadline deadline]" << endl;
    cout << "    -t, --title     Task title (required)" << endl;
    cout << "    --desc         Task description (optional)" << endl;
    cout << "    -p, --priority Task priority 1-5 (optional, default: 5)" << endl;
    cout << "    -d, --deadline Task deadline YYYY-MM-DD (optional, default: today)" << endl;
    cout << endl;
    cout << "  remove <-t|--title title>" << endl;
    cout << "    -t, --title    Title of task to remove" << endl;
    cout << endl;
    cout << "  done <-t|--title title>" << endl;
    cout << "    -t, --title    Title of task to mark as completed" << endl;
    cout << endl;
    cout << "  list [-d|--deadline date] [--done]" << endl;
    cout << "  list [--today] [--done]" << endl;
    cout << "  list [--tomorrow] [--done]" << endl;
    cout << "    -d, --deadline Show tasks for specific date" << endl;
    cout << "    --today       Show tasks for today" << endl;
    cout << "    --tomorrow    Show tasks for tomorrow" << endl;
    cout << "    --done        Show only completed tasks" << endl;
}

int main() {
    TaskManager manager;
    string input;

    cout << "Task Manager" << endl;
    cout << "Enter commands in format: zadanie2 [command] [options]" << endl;
    cout << "Type 'exit' to quit" << endl;
    cout << endl;

    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "exit") {
            break;
        }

        // Rozdelenie vstupu na príkazy a parametre
        vector<string> args;
        stringstream ss(input);
        string arg;
        while (ss >> arg) {
            args.push_back(arg);
        }

        if (args.empty()) {
            continue;
        }

        // Kontrola, či príkaz začína na "zadanie2"
        if (args[0] != "zadanie2") {
            cout << "Error: Command must start with 'zadanie2'" << endl;
            continue;
        }

        // Odstránenie "zadanie2" z argumentov
        args.erase(args.begin());

        if (args.empty()) {
            printHelp();
            continue;
        }

        string command = args[0];
        args.erase(args.begin());

        if (command == "-h" || command == "--help") {
            printHelp();
        }
        else if (command == "add") {
            string title, description = "", deadline = getTodayDate();
            int priority = 5;
            bool titleFound = false;

            for (size_t i = 0; i < args.size(); i++) {
                string arg = args[i];
                if ((arg == "-t" || arg == "--title") && i + 1 < args.size()) {
                    title = args[++i];
                    titleFound = true;
                } else if (arg == "--desc" && i + 1 < args.size()) {
                    description = args[++i];
                } else if ((arg == "-p" || arg == "--priority") && i + 1 < args.size()) {
                    try {
                        priority = stoi(args[++i]);
                        if (priority < 1) {
                            cout << "Error: Priority must be greater than 0" << endl;
                            continue;
                        }
                    } catch (const exception& e) {
                        cout << "Error: Invalid priority value" << endl;
                        continue;
                    }
                } else if ((arg == "-d" || arg == "--deadline") && i + 1 < args.size()) {
                    deadline = args[++i];
                }
            }

            if (!titleFound) {
                cout << "Error: Task title is required" << endl;
                continue;
            }

            if (manager.addTask(title, description, priority, deadline)) {
                cout << "Task was added" << endl;
            } else {
                cout << "Error: Task with this name already exists" << endl;
            }
        }
        else if (command == "remove") {
            string title;
            bool titleFound = false;

            for (size_t i = 0; i < args.size(); i++) {
                string arg = args[i];
                if ((arg == "-t" || arg == "--title") && i + 1 < args.size()) {
                    title = args[++i];
                    titleFound = true;
                }
            }

            if (!titleFound) {
                cout << "Error: Task title is required" << endl;
                continue;
            }

            if (manager.removeTask(title)) {
                cout << "Task was removed" << endl;
            } else {
                cout << "Error: Task not found" << endl;
            }
        }
        else if (command == "done") {
            string title;
            bool titleFound = false;

            for (size_t i = 0; i < args.size(); i++) {
                string arg = args[i];
                if ((arg == "-t" || arg == "--title") && i + 1 < args.size()) {
                    title = args[++i];
                    titleFound = true;
                }
            }

            if (!titleFound) {
                cout << "Error: Task title is required" << endl;
                continue;
            }

            if (manager.markTaskAsDone(title)) {
                cout << "Task was marked as completed" << endl;
            } else {
                cout << "Error: Task not found" << endl;
            }
        }
        else if (command == "list") {
            string date;
            bool showOnlyCompleted = false;
            bool dateSpecified = false;

            for (size_t i = 0; i < args.size(); i++) {
                string arg = args[i];
                if ((arg == "-d" || arg == "--deadline") && i + 1 < args.size()) {
                    date = args[++i];
                    dateSpecified = true;
                } else if (arg == "--today") {
                    date = getTodayDate();
                    dateSpecified = true;
                } else if (arg == "--tomorrow") {
                    date = getTomorrowDate();
                    dateSpecified = true;
                } else if (arg == "--done") {
                    showOnlyCompleted = true;
                }
            }

            vector<Task> tasks;
            if (dateSpecified) {
                tasks = manager.listTasksByDate(date, showOnlyCompleted);
            } else {
                tasks = manager.listAllTasks(showOnlyCompleted);
            }

            if (tasks.empty()) {
                if (showOnlyCompleted) {
                    cout << "No completed tasks found" << endl;
                } else {
                    cout << "No tasks found" << endl;
                }
            } else {
                if (showOnlyCompleted) {
                    cout << "List of completed tasks:" << endl;
                } else {
                    cout << "List of tasks:" << endl;
                }
                for (const auto &task : tasks) {
                    cout << "Title: " << task.getTitle() << endl;
                    cout << "Description: " << task.getDescription() << endl;
                    cout << "Priority: " << task.getPriority() << endl;
                    cout << "Deadline: " << task.getDeadline() << endl;
                    cout << "Status: " << (task.getDone() ? "Completed" : "In progress") << endl;
                    cout << "-------------------" << endl;
                }
            }
        }
        else {
            cout << "Unknown command. Use 'zadanie2 -h' or 'zadanie2 --help' for available commands." << endl;
        }

        cout << endl;
    }

    return 0;
}