#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <functional>
#include <ctime>

// A játéklogikát kezelő osztály
class Game {
private:
	class Field {
	public:
		bool playerHere, flagged, bomb, seen;
		unsigned int value;

		Field() : playerHere(false), flagged(false), bomb(false), seen(false), value(0) {}

		void here() { playerHere = true; }
		void notHere() { playerHere = false; }
		void flagHere() { flagged = !flagged; }
		void bombHere() { bomb = true; }
		void seenHere() { seen = true; }

		void displayField() {
			if (playerHere) {
				std::cout << "P ";
			}
			else if (seen) {
				if (bomb) std::cout << "* ";
				else {
					if (value == 0) std::cout << "  ";
					else { std::cout << value << " "; }
				}
			}
			else if (flagged) {
				std::cout << "F ";
			}
			else {
				std::cout << "X ";
			}
		}
	};

	int width, height;
	int playerX, playerY;
	int bombCount;
	Field** field;
	bool gameOver;
	bool gameWon;

	void placeBombs() {
		srand(static_cast<unsigned int>(time(0)));
		for (int i = 0; i < bombCount; i++) {
			int x, y;
			do {
				x = rand() % width;
				y = rand() % height;
			} while (field[y][x].bomb);
			field[y][x].bombHere();
		}
	}

	void calculateAround() {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (field[y][x].bomb) continue;
				int count = 0;
				for (int dy = -1; dy <= 1; dy++) {
					for (int dx = -1; dx <= 1; dx++) {
						int nx = x + dx;
						int ny = y + dy;
						if (isValidField(nx, ny) && field[ny][nx].bomb) {
							count++;
						}
					}
				}
				field[y][x].value = count;
			}
		}
	}

	void revealEmptyFields(int x, int y) {
		if (!isValidField(x, y) || field[y][x].bomb) return;

		field[y][x].seenHere();

		if (field[y][x].value == 0) {
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					int nx = x + dx;
					int ny = y + dy;
					if (isValidField(nx, ny) && !field[ny][nx].seen) {
						revealEmptyFields(nx, ny);
					}
				}
			}
		}
	}

	bool checkWin() {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (!field[y][x].bomb && !field[y][x].seen) {
					return false;
				}
			}
		}
		return true;
	}

public:
	Game(int w, int h, int bombs) :
		width(w),
		height(h),
		bombCount(bombs),
		playerX(w / 2),
		playerY(h / 2),
		gameOver(false),
		gameWon(false) {

		// Mező létrehozása
		field = new Field * [height];
		for (int i = 0; i < height; i++) {
			field[i] = new Field[width];
		}

		// Bombák elhelyezése és számértékek kiszámítása
		placeBombs();
		calculateAround();
	}

	~Game() {
		// Memória felszabadítása
		for (int i = 0; i < height; i++) {
			delete[] field[i];
		}
		delete[] field;
	}

	void display() {
		system("cls");
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (x == playerX && y == playerY) {
					field[y][x].here();
					field[y][x].displayField();
				}
				else {
					field[y][x].notHere();
					field[y][x].displayField();
				}
			}
			std::cout << std::endl;
		}
	}

	bool isValidField(int x, int y) {
		return x >= 0 && x < width && y >= 0 && y < height;
	}

	void movePlayer(int dx, int dy) {
		int newX = playerX + dx;
		int newY = playerY + dy;
		if (isValidField(newX, newY)) {
			playerX = newX;
			playerY = newY;
			display();
		}
	}

	void enterKey() {
		Field& current = field[playerY][playerX];
		if (current.seen || current.flagged) return;

		if (current.bomb) {
			gameOver = true;
			gameWon = false;
			current.seenHere();
			display();
			std::cout << "\n\n Vesztettél! Aknára léptél.\n";
			return;
		}

		current.seenHere();
		if (current.value == 0) {
			revealEmptyFields(playerX, playerY);
		}

		if (checkWin()) {
			gameOver = true;
			gameWon = true;
			display();
			std::cout << "\n\n Nyertél! Megtaláltad az összes aknát.\n";
			return;
		}

		display();
	}

	void flagKey() {
		field[playerY][playerX].flagHere();
		display();
	}

	bool isGameOver() const {
		return gameOver;
	}

	bool isGameWon() const {
		return gameWon;
	}

	// Játék futtatása
	void run(std::function<void()> returnToMenu) {
		display();

		while (!gameOver) {
			int input = _getch();
			switch (input) {
			case 72: // Fel nyíl
				movePlayer(0, -1);
				break;
			case 80: // Le nyíl
				movePlayer(0, 1);
				break;
			case 75: // Balra nyíl
				movePlayer(-1, 0);
				break;
			case 77: // Jobbra nyíl
				movePlayer(1, 0);
				break;
			case 27: // ESC
				std::cout << "\n\n Játék megszakítása...\n";
				gameOver = true;
				break;
			case 13: // ENTER
				enterKey();
				break;
			case 8: // BACKSPACE
				flagKey();
				break;
			}
		}

		// Ha vége a játéknak, várunk 5 másodpercet
		if (gameOver) {
			std::cout << " 5 másodperc múlva visszakerülsz a főmenübe!" << std::endl;
			for (int i = 5; i >= 0; i--) {
				std::cout << " " << i << "..";
				Sleep(1000);
			}
		}

		// Visszatérés a menübe
		returnToMenu();
	}
};

class MenuSystem {
private:
	// Nehézségi beállítások
	enum class Difficulty { EASY, MEDIUM, HARD, CUSTOM };
	Difficulty selectedDifficulty;
	unsigned int bombCount;
	const int BOARD_WIDTH = 10;
	const int BOARD_HEIGHT = 10;

	// Segédfüggvények
	void clearScreen() {
		system("cls");
	}

	void countdown(const std::string& message) {
		std::cout << message << std::endl;
		for (int i = 5; i >= 0; i--) {
			std::cout << ' ' << i << "..";
			Sleep(1000);
		}
		clearScreen();
	}

	int waitForValidInput(const std::vector<int>& validInputs) {
		while (true) {
			int input = _getch();
			for (int validInput : validInputs) {
				if (input == validInput) {
					return input;
				}
			}
		}
	}

	// Menüoldalak
	void showMainMenu() {
		clearScreen();
		std::cout << "==============================================================\n"
			<< "              Üdvözöllek az AKNAVADÁSZ játékban!\n"
			<< "==============================================================\n"
			<< "\n Kérlek válassz az alábbiak közül:\n\n"
			<< " Ismertető - F1\n"
			<< " Nehézség beállítása - F2\n"
			<< " Irányítások - F3\n"
			<< " Kreditek - F4\n\n"
			<< " JÁTÉK - ENTER\n\n\n\n"
			<< " Kilépés - ESC\n"
			<< "==============================================================\n"
			<< " <-                         főmenü                         ->" << std::endl;

		std::vector<int> validInputs = { 13, 59, 60, 61, 62, 27, 75, 77, 80 };
		int input = waitForValidInput(validInputs);

		switch (input) {
		case 13: // ENTER
			startGame();
			break;
		case 59: // F1
			showInfo();
			break;
		case 60: // F2
			showDifficulty();
			break;
		case 61: // F3
			showControls();
			break;
		case 62: // F4
			showCredits();
			break;
		case 27: // ESC
			showQuitConfirmation();
			break;
		case 75: // BALRA NYÍL
			showCredits();
			break;
		case 77: // JOBBRA NYÍL
			showInfo();
			break;
		case 80: // LEFELÉ NYÍL
			showCredits();
			break;
		}
	}

	void showInfo() {
		clearScreen();
		std::cout << "==============================================================\n"
			<< "                           ISMERTETŐ\n"
			<< "==============================================================\n"
			<< "\n A játék célja, hogy a 10x10-es pályán megtaláld és megjelöld\n"
			<< " az összes aknát, miközben elkerülöd a rájuk lépést. Minden\n"
			<< " lépés után felfedezheted a mezőket, amelyek száma megmutatja,\n"
			<< " hány bomba található a szomszédos mezőn.\n"
			<< " Használd a megérzéseidet és a logikádat, hogy nyerj!\n\n"
			<< " Főbb funkciók :\n"
			<< " - Többféle nehézségi szint.\n"
			<< " - Dinamikus pályagenerálás és bombaszám.\n"
			<< " - Egyszerű, intuitív billentyűzetes irányítás.\n\n"
			<< " Készen állsz a kihívásra?\n\n\n\n"
			<< " Főmenü - ESC\n"
			<< "==============================================================\n"
			<< " <-                        ismertető                        ->" << std::endl;

		std::vector<int> validInputs = { 27, 77, 75 };
		int input = waitForValidInput(validInputs);

		switch (input) {
		case 27: // ESC
			showMainMenu();
			break;
		case 77: // JOBBRA NYÍL
			showDifficulty();
			break;
		case 75: // BALRA NYÍL
			showMainMenu();
			break;
		}
	}

	void selectedDifficultyCheck(unsigned int& bombCount) {
		if (bombCount == 10) selectedDifficulty = Difficulty::EASY;
		else if (bombCount == 25) selectedDifficulty = Difficulty::MEDIUM;
		else if (bombCount == 50) selectedDifficulty = Difficulty::HARD;
		else { selectedDifficulty = Difficulty::CUSTOM; }
	}

	void showDifficulty() {
		clearScreen();
		std::string difficultyText;
		switch (selectedDifficulty) {
		case Difficulty::EASY:
			difficultyText = "könnyű";
			break;
		case Difficulty::MEDIUM:
			difficultyText = "közepes";
			break;
		case Difficulty::HARD:
			difficultyText = "nehéz";
			break;
		case Difficulty::CUSTOM:
			difficultyText = "egyedi";
			break;
		}
		std::cout << "==============================================================\n"
			<< "                           NEHÉZSÉG\n"
			<< "==============================================================\n"
			<< "\n\n\n"
			<< " Nehézség - " << difficultyText << "\n"
			<< "  <- A     D ->\n\n"
			<< " Tábla mérete - 10 X 10\n\n"
			<< " Bombák száma - " << bombCount << " db\n"
			<< " (fel) W     S (le)\n\n\n\n"
			<< " Főmenü - ESC\n"
			<< "==============================================================\n"
			<< " <-                        nehézség                        ->" << std::endl;

		bool end = false;
		while (!end) {
			std::vector<int> validInputs = { 27, 77, 75, 100, 97, 119, 115 };
			int input = waitForValidInput(validInputs);

			switch (input) {
			case 27: // ESC
				end = true;
				showMainMenu();
				break;
			case 77: // JOBBRA NYÍL
				end = true;
				showControls();
				break;
			case 75: // BALRA NYÍL
				end = true;
				showInfo();
				break;
			case 100: // d
				if (selectedDifficulty == Difficulty::EASY) {
					selectedDifficulty = Difficulty::MEDIUM; bombCount = 25;
				}
				else if (selectedDifficulty == Difficulty::MEDIUM) {
					selectedDifficulty = Difficulty::HARD; bombCount = 50;
				}
				else {
					selectedDifficulty = Difficulty::EASY; bombCount = 10;
				}
				end = true; showDifficulty();
				break;
			case 97: // a
				if (selectedDifficulty == Difficulty::MEDIUM) {
					selectedDifficulty = Difficulty::EASY; bombCount = 10;
				}
				else if (selectedDifficulty == Difficulty::HARD) {
					selectedDifficulty = Difficulty::MEDIUM; bombCount = 25;
				}
				else {
					selectedDifficulty = Difficulty::HARD; bombCount = 50;
				}
				end = true;	showDifficulty();
				break;
			case 119: // w
				if (bombCount < 100) {
					bombCount++;
					selectedDifficultyCheck(bombCount);
					end = true;	showDifficulty();
				}
				// Ha már 100-nál van, nem csinál semmit
				break;
			case 115: // s
				if (bombCount > 0) {
					bombCount--;
					selectedDifficultyCheck(bombCount);
					end = true; showDifficulty();
				}
				// Ha már 0-nál van, nem csinál semmit
				break;
			}
		}
	}

	void showControls() {
		clearScreen();
		std::cout << "==============================================================\n"
			<< "                           IRÁNYÍTÁS\n"
			<< "==============================================================\n"
			<< "\n Billentyűkiosztás:\n\n\n"
			<< " Mozgás:\n\n"
			<< " Fel - Felfelé nyíl\n"
			<< " Le - Lefelé nyíl\n"
			<< " Balra - Balra nyíl\n"
			<< " Jobbra - Jobbra nyíl\n\n"
			<< " Akciók:\n\n"
			<< " Mező feltárása - ENTER\n"
			<< " Mező megjelölése - BACKSPACE\n\n\n\n"
			<< " Főmenü - ESC\n"
			<< "==============================================================\n"
			<< " <-                        irányítás                        ->" << std::endl;

		std::vector<int> validInputs = { 27, 77, 75 };
		int input = waitForValidInput(validInputs);

		switch (input) {
		case 27: // ESC
			showMainMenu();
			break;
		case 77: // JOBBRA NYÍL
			showCredits();
			break;
		case 75: // BALRA NYÍL
			showDifficulty();
			break;
		}
	}

	void showCredits() {
		clearScreen();
		std::cout << "==============================================================\n"
			<< "                           KREDITEK\n"
			<< "==============================================================\n"
			<< "\n Készítette: DrJegesmedve\n\n"
			<< " Fejlesztő - DrJegesmedve\n"
			<< " Designer - DrJegesmedve\n\n"
			<< " Tesztelő - cnek\n"
			<< " Tesztelő - Szasza\n"
			<< " Tesztelő - dr.frustrumassae\n"
			<< " Tesztelő - K.O.\n\n\n\n"
			<< " Főmenü - ESC\n"
			<< "==============================================================\n"
			<< " <-                        kreditek                        ->" << std::endl;

		std::vector<int> validInputs = { 27, 72, 77, 75 };
		int input = waitForValidInput(validInputs);

		switch (input) {
		case 27: // ESC
			showMainMenu();
			break;
		case 72: // FELFELÉ NYÍL
			showMainMenu();
			break;
		case 77: // JOBBRA NYÍL
			showMainMenu();
			break;
		case 75: // BALRA NYÍL
			showControls();
			break;
		}
	}

	void showQuitConfirmation() {
		clearScreen();
		std::cout << "==============================================================\n"
			<< "                           KILÉPSZ?\n"
			<< "==============================================================\n"
			<< "\n\n Biztosan ki szeretnél lépni? Ha igen, nyomj ENTER-t!\n\n\n\n"
			<< " Főmenü - ESC\n"
			<< "==============================================================\n"
			<< " <-                        kilépés                        ->" << std::endl;

		std::vector<int> validInputs = { 27, 13 };
		int input = waitForValidInput(validInputs);

		switch (input) {
		case 27: // ESC
			showMainMenu();
			break;
		case 13: // ENTER
			clearScreen();
			std::cout << " Várakozás a program bezárására\n 3.."; Sleep(1000);
			std::cout << " 2.. "; Sleep(1000);
			std::cout << " 1..\n"; Sleep(1000);
			exit(0);
			break;
		}
	}

	void startGame() {
		// Játék indítása a beállított bombaszámmal
		clearScreen();
		std::cout << " Játék indítása " << bombCount << " db bombával...\n";
		Sleep(1000);

		// Játék objektum létrehozása
		Game game(BOARD_WIDTH, BOARD_HEIGHT, bombCount);

		// Játék futtatása és callback a menübe visszatéréshez
		game.run([this]() { this->showMainMenu(); });
	}

public:
	MenuSystem() : selectedDifficulty(Difficulty::MEDIUM), bombCount(25) {
		// Alapértelmezett beállítások
	}

	// Publikus interface a menü indításához
	void run() {
		setlocale(LC_ALL, "hun");
		showMainMenu();
	}
};

// Főprogram
int main() {
	MenuSystem menuSystem;
	menuSystem.run();
	return 0;
}
