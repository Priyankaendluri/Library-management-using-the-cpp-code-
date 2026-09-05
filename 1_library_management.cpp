#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Book {
public:
    int id;
    string title;
    string author;
    bool isIssued;
    int issuedToMemberId; // -1 if not issued

    Book() : id(0), isIssued(false), issuedToMemberId(-1) {}
    Book(int i, string t, string a) : id(i), title(t), author(a), isIssued(false), issuedToMemberId(-1) {}

    string toFileString() const {
        stringstream ss;
        ss << id << "," << title << "," << author << "," << isIssued << "," << issuedToMemberId;
        return ss.str();
    }

    static Book fromFileString(const string& line) {
        stringstream ss(line);
        string idStr, title, author, issuedStr, memberStr;
        getline(ss, idStr, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, issuedStr, ',');
        getline(ss, memberStr, ',');

        Book b(stoi(idStr), title, author);
        b.isIssued = (issuedStr == "1");
        b.issuedToMemberId = stoi(memberStr);
        return b;
    }

    void display() const {
        cout << "ID: " << id << " | Title: " << title << " | Author: " << author
             << " | Status: " << (isIssued ? ("Issued to Member #" + to_string(issuedToMemberId)) : "Available")
             << endl;
    }
};

class Member {
public:
    int id;
    string name;

    Member() : id(0) {}
    Member(int i, string n) : id(i), name(n) {}

    string toFileString() const {
        return to_string(id) + "," + name;
    }

    static Member fromFileString(const string& line) {
        stringstream ss(line);
        string idStr, name;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        return Member(stoi(idStr), name);
    }

    void display() const {
        cout << "ID: " << id << " | Name: " << name << endl;
    }
};

class Library {
private:
    vector<Book> books;
    vector<Member> members;
    const string bookFile = "books.csv";
    const string memberFile = "members.csv";

public:
    Library() {
        loadBooks();
        loadMembers();
    }

    void loadBooks() {
        ifstream file(bookFile);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) books.push_back(Book::fromFileString(line));
        }
    }

    void loadMembers() {
        ifstream file(memberFile);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) members.push_back(Member::fromFileString(line));
        }
    }

    void saveBooks() {
        ofstream file(bookFile);
        for (const auto& b : books) file << b.toFileString() << endl;
    }

    void saveMembers() {
        ofstream file(memberFile);
        for (const auto& m : members) file << m.toFileString() << endl;
    }

    int nextBookId() {
        int maxId = 0;
        for (const auto& b : books) maxId = max(maxId, b.id);
        return maxId + 1;
    }

    int nextMemberId() {
        int maxId = 0;
        for (const auto& m : members) maxId = max(maxId, m.id);
        return maxId + 1;
    }

    void addBook(const string& title, const string& author) {
        Book b(nextBookId(), title, author);
        books.push_back(b);
        saveBooks();
        cout << "Book added with ID " << b.id << endl;
    }

    void addMember(const string& name) {
        Member m(nextMemberId(), name);
        members.push_back(m);
        saveMembers();
        cout << "Member added with ID " << m.id << endl;
    }

    void listBooks() {
        if (books.empty()) { cout << "No books in library.\n"; return; }
        for (const auto& b : books) b.display();
    }

    void listMembers() {
        if (members.empty()) { cout << "No members registered.\n"; return; }
        for (const auto& m : members) m.display();
    }

    bool memberExists(int memberId) {
        for (const auto& m : members) if (m.id == memberId) return true;
        return false;
    }

    void issueBook(int bookId, int memberId) {
        if (!memberExists(memberId)) {
            cout << "Member not found.\n";
            return;
        }
        for (auto& b : books) {
            if (b.id == bookId) {
                if (b.isIssued) {
                    cout << "Book is already issued.\n";
                } else {
                    b.isIssued = true;
                    b.issuedToMemberId = memberId;
                    saveBooks();
                    cout << "Book issued successfully.\n";
                }
                return;
            }
        }
        cout << "Book not found.\n";
    }

    void returnBook(int bookId) {
        for (auto& b : books) {
            if (b.id == bookId) {
                if (!b.isIssued) {
                    cout << "Book was not issued.\n";
                } else {
                    b.isIssued = false;
                    b.issuedToMemberId = -1;
                    saveBooks();
                    cout << "Book returned successfully.\n";
                }
                return;
            }
        }
        cout << "Book not found.\n";
    }

    void searchByTitle(const string& keyword) {
        bool found = false;
        for (const auto& b : books) {
            if (b.title.find(keyword) != string::npos) {
                b.display();
                found = true;
            }
        }
        if (!found) cout << "No matching books found.\n";
    }
};

void showMenu() {
    cout << "\n===== Library Management System =====\n";
    cout << "1. Add Book\n";
    cout << "2. Add Member\n";
    cout << "3. List Books\n";
    cout << "4. List Members\n";
    cout << "5. Issue Book\n";
    cout << "6. Return Book\n";
    cout << "7. Search Book by Title\n";
    cout << "8. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    Library library;
    int choice;

    do {
        showMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                string title, author;
                cout << "Enter title: ";
                getline(cin, title);
                cout << "Enter author: ";
                getline(cin, author);
                library.addBook(title, author);
                break;
            }
            case 2: {
                string name;
                cout << "Enter member name: ";
                getline(cin, name);
                library.addMember(name);
                break;
            }
            case 3:
                library.listBooks();
                break;
            case 4:
                library.listMembers();
                break;
            case 5: {
                int bookId, memberId;
                cout << "Enter book ID: ";
                cin >> bookId;
                cout << "Enter member ID: ";
                cin >> memberId;
                library.issueBook(bookId, memberId);
                break;
            }
            case 6: {
                int bookId;
                cout << "Enter book ID: ";
                cin >> bookId;
                library.returnBook(bookId);
                break;
            }
            case 7: {
                string keyword;
                cout << "Enter title keyword: ";
                getline(cin, keyword);
                library.searchByTitle(keyword);
                break;
            }
            case 8:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while (choice != 8);

    return 0;
}
