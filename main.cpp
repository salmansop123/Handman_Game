#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

class Game {
public:
    virtual void play(string& badge) = 0;
    virtual ~Game() {}
};

class Hangman : public Game {
public:
    void play(string& badge) override {
        vector<string> words = loadWords("words.txt");
        if (words.empty()) {
            cout << "Word list is empty. Cannot play Hangman." << endl;
            return;
        }

        string word = words[rand() % words.size()];
        string guessedWord(word.size(), '_');
        string guessedLetters;
        int attempts = 6;

        cout << "Welcome to Hangman! Guess the word, one letter at a time." << endl;
        while (attempts > 0 && guessedWord != word) {
            displayGameState(guessedWord, guessedLetters, attempts);
            displayHangman(attempts);
            char guess;
            cout << "Enter your guess: ";
            cin >> guess;
            guess = tolower(guess);

            if (guessedLetters.find(guess) != string::npos) {
                cout << "You already guessed that letter." << endl;
                continue;
            }

            guessedLetters += guess;

            if (word.find(guess) != string::npos) {
                for (size_t i = 0; i < word.size(); ++i) {
                    if (word[i] == guess) {
                        guessedWord[i] = guess;
                    }
                }
                cout << "Correct guess!" << endl;
            } else {
                attempts--;
                cout << "Wrong guess! Attempts remaining: " << attempts << endl;
            }
        }

        if (guessedWord == word) {
            cout << "Congratulations! You guessed the word: " << word << endl;
            badge = "Hangman Badge";
        } else {
            cout << "Game Over! The word was: " << word << endl;
            displayHangman(0);
        }
    }

private:
    vector<string> loadWords(const string& filename) {
        vector<string> words;
        ifstream file(filename);
        string word;
        while (file >> word) {
            words.push_back(word);
        }
        return words;
    }

    void displayGameState(const string& guessedWord, const string& guessedLetters, int attempts) {
        cout << "\nWord: " << guessedWord << endl;
        cout << "Guessed Letters: " << guessedLetters << endl;
        cout << "Attempts Remaining: " << attempts << endl;
    }

    void displayHangman(int attempts) {
        string hangman[] = {
            "  +---+\n"
            "  |   |\n"
            "  O   |\n"
            " /|\\  |\n"
            " / \\  |\n"
            "      |\n"
            "=========\n",

            "  +---+\n"
            "  |   |\n"
            "  O   |\n"
            " /|\\  |\n"
            " /    |\n"
            "      |\n"
            "=========\n",

            "  +---+\n"
            "  |   |\n"
            "  O   |\n"
            " /|\\  |\n"
            "      |\n"
            "      |\n"
            "=========\n",

            "  +---+\n"
            "  |   |\n"
            "  O   |\n"
            " /|   |\n"
            "      |\n"
            "      |\n"
            "=========\n",

            "  +---+\n"
            "  |   |\n"
            "  O   |\n"
            "  |   |\n"
            "      |\n"
            "      |\n"
            "=========\n",

            "  +---+\n"
            "  |   |\n"
            "  O   |\n"
            "      |\n"
            "      |\n"
            "      |\n"
            "=========\n",

            "  +---+\n"
            "  |   |\n"
            "      |\n"
            "      |\n"
            "      |\n"
            "      |\n"
            "=========\n"
        };

        cout << hangman[6 - attempts] << endl;
    }
};


class GameSystem {
private:
    map<string, string> users;
    map<string, vector<string>> badges;
    vector<Game*> games;

    void loadUsersFromFile() {
        ifstream file("signUp.txt");
        string username, password;
        while (file >> username >> password) {
            users[username] = password;
            badges[username] = {};
        }
        file.close();
    }

    void saveUserToFile(const string& username, const string& password) {
        ofstream file("signUp.txt", ios::app);
        file << username << " " << password << endl;
        file.close();
    }

public:
    GameSystem() {
        srand(static_cast<unsigned>(time(0)));
        games.push_back(new Hangman());
        loadUsersFromFile();
    }

    ~GameSystem() {
        for (Game* game : games) {
            delete game;
        }
    }

    void signUp() {
        string username, password;
        cout << "Sign Up\nUsername: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (users.count(username)) {
            cout << "Username already exists!" << endl;
        } else {
            users[username] = password;
            badges[username] = {};
            saveUserToFile(username, password);
            cout << "Sign up successful!" << endl;
        }
    }

    void signIn() {
        string username, password;
        cout << "Sign In\nUsername: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (users.count(username) && users[username] == password) {
            cout << "Login successful!" << endl;
            gameMenu(username);
        } else {
            cout << "Invalid credentials!" << endl;
        }
    }

    void gameMenu(const string& username) {
        while (true) {
            cout << "\nGame Menu:\n";
            cout << "1. Play\n2. View My Badges\n3. View Leaderboard\n4. Exit\n";
            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                playGame(username);
                break;
            case 2:
                viewBadges(username);
                break;
            case 3:
                viewLeaderboard();
                break;
            case 4:
                return;
            default:
                cout << "Invalid option. Try again." << endl;
            }
        }
    }

    void playGame(const string& username) {
        string badge;
        int randomIndex = rand() % games.size();
        games[randomIndex]->play(badge);

        if (!badge.empty()) {
            badges[username].push_back(badge);
            cout << "You earned a " << badge << "!" << endl;
        }
    }

    void viewBadges(const string& username) {
        cout << "\nYour Badges:\n";
        for (const string& badge : badges[username]) {
            cout << "- " << badge << endl;
        }
    }

    void viewLeaderboard() {
        cout << "\nLeaderboard:\n";
        for (const auto& user : badges) {
            cout << user.first << " has " << user.second.size() << " badges." << endl;
        }
    }
};

int main() {
    GameSystem system;

    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Sign Up\n2. Sign In\n3. Exit\n";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            system.signUp();
            break;
        case 2:
            system.signIn();
            break;
        case 3:
            cout << "Exiting..." << endl;
            return 0;
        default:
            cout << "Invalid option. Try again." << endl;
        }
    }
}
