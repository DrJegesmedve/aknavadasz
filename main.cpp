#include <iostream>
#include <windows.h> // setlocale és console törlés miatt
#include <conio.h> // Billenytűzet bekérések miatt
#include <ctime> // Random miatt

int diff; unsigned int selectedDiff = 1; unsigned int globalBombCount = 25; // Lejjebb kell globálisan
void info(); int difficulty(); void controls(); void credits(); int main();

void vissza() {
    std::cout << " 5.."; Sleep(1000);
    std::cout << " 4.."; Sleep(1000);
    std::cout << " 3.."; Sleep(1000);
    std::cout << " 2.."; Sleep(1000);
    std::cout << " 1.."; Sleep(1000);
    std::cout << " 0.."; system("cls");
}

class Map {
private:
    int width, height;
    int playerX, playerY;

    class Field {
    public:
        bool playerHere, flagged, bomb, seen;
        unsigned int value;

        Field() : playerHere(false), flagged(false), bomb(false), seen(false), value(0) {}

        void here() { playerHere = true; }
        void notHere() { playerHere = false; }
        void flagHere() { flagged = !flagged; } // Megfordítja az aktuálist
        void bombHere() { bomb = true; }
        void seenHere() { seen = true; }

        void displayField() {
            if (playerHere) {
                std::cout << "P "; // Aktuális pozíció jelző
            }
            else if (seen) {
                if (bomb) std::cout << "* "; // Felfedezett bomba
                else {
                    if (value == 0) std::cout << "  "; // Mező körüli bombák száma
                    else { std::cout << value << " "; }
                }
            }
            else if (flagged) {
                std::cout << "F "; // Megjelölt mező
            }
            else {
                std::cout << "X "; // "Érintetlen" mező
            }
        }
    };

    Field** field;

    void placeBombs(int bombCount) { // Bombák véletlenszerű elhelyezése
        srand(time(0));
        for (int i = 0; i < bombCount; i++) {
            int x, y;
            do {
                x = rand() % width;
                y = rand() % height;
            } while (field[y][x].bomb);
            field[y][x].bombHere();
        }
    }

    void calculateAround() { // Mező körüli bombák kiszámítása
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

    void revealEmptyFields(int x, int y) { // Nullák és a a körülötte lévők megmutatása
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
    Map(int w, int h, int bombs) : width(w), height(h), playerX(w / 2), playerY(h / 2) {
        field = new Field * [height];
        for (int i = 0; i < height; i++) {
            field[i] = new Field[width];
        }
        placeBombs(bombs);
        calculateAround();
    }

    ~Map() {
        for (int i = 0; i < height; i++) {
            delete[] field[i];
        }
        delete[] field;
    }

    void display() { // Tábla megjelenítése
        system("cls"); // Windows specifikus konzol törlés 
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

    void movePlayer(int dx, int dy) { // Mozgás
        int newX = playerX + dx;
        int newY = playerY + dy;
        if (isValidField(newX, newY)) {
            playerX = newX;
            playerY = newY;
            display();
        }
    }

    void enterKey() { // Megnéz
        Field& current = field[playerY][playerX];
        if (current.seen || current.flagged) return;

        if (current.bomb) {
            std::cout << "\n\n Vesztettél! Aknára léptél.\n 5 másodperc múlva visszakerülsz a főmenübe!" << std::endl;
            vissza(); main();
        }

        current.seenHere();
        if (current.value == 0) {
            revealEmptyFields(playerX, playerY);
        }

        if (checkWin()) {
            std::cout << "Nyertél! Megtaláltad az összes aknát.\n5 másodperc múlva visszakerülsz a főmenübe!" << std::endl;
            vissza(); main();
        }

        display();
    }

    void backspaceKey() { // Megjelölés
        field[playerY][playerX].flagHere();
        display();
    }
};

int Game() {
    const unsigned int mapWidth = 10;
    const unsigned int mapHeight = 10;

    Map map(mapWidth, mapHeight, globalBombCount);

    map.display();

    while (true) {
        int input = _getch();
        switch (input) {
        case 72: // Felfelé nyíl
            map.movePlayer(0, -1);
            break;
        case 80: // Lefelé nyíl
            map.movePlayer(0, 1);
            break;
        case 75: // Balra nyíl nyíl
            map.movePlayer(-1, 0);
            break;
        case 77: // Jobbra nyíl
            map.movePlayer(1, 0);
            break;
        case 27: // ESC
            std::cout << "\n\n Játék megszakítása...\n 5 másodperc múlva visszakerülsz a főmenübe!" << std::endl;
            vissza(); main();
        case 13: // ENTER
            map.enterKey();
            break;
        case 8: // BACKSPACE
            map.backspaceKey();
            break;
        default:
            continue;
        }
    }
    return 0;
}

void info() {
    system("cls");
    std::cout << "==============================================================\n" <<
        "                           ISMERTETŐ\n" <<
        "==============================================================\n" <<
        "\n A játék célja, hogy a 10x10-es pályán megtaláld és megjelöld\n az összes aknát," <<
        " miközben elkerülöd a rájuk lépést. Minden\n lépés után felfedezheted a mezőket," <<
        " amelyek száma megmutatja,\n hány bomba található a szomszédos mezőn.\n" <<
        " Használd a megérzéseidet és a logikádat, hogy nyerj!\n\n" <<
        " Főbb funkciók :\n - Többféle nehézségi szint.\n - Dinamikus pályagenerálás és bombaszám.\n" <<
        " - Egyszerű, intuitív billentyűzetes irányítás.\n\n Készen állsz a kihívásra?\n\n\n\n Főmenü - ESC\n" <<
        "==============================================================\n <-                        ismertető                        ->" << std::endl;
    bool end = false;
    while (!end) {
        int input = _getch();
        switch (input) {
        case 27: // ESC
            end = true; system("cls"); main(); break;
        case 77: // JOBBRA NYÍL
            diff = difficulty(); break;
        case 75: // BALRA NYÍL
            end = true; system("cls"); main(); break;
        default:
            continue;
        }
    }
}

int difficulty() {
    system("cls");
    std::string difficultyLvl[4] = { "könnyű", "közepes", "nehéz", "egyedi" };
    if (selectedDiff == 0) globalBombCount = 10;
    else if (selectedDiff == 1) globalBombCount = 25;
    else if (selectedDiff == 2) globalBombCount = 50;

    if (globalBombCount == 10) selectedDiff = 0;
    else if (globalBombCount == 25) selectedDiff = 1;
    else if (globalBombCount == 50) selectedDiff = 2;
    else { selectedDiff = 3; }

    std::cout << "==============================================================\n" <<
        "                           NEHÉZSÉG\n" <<
        "==============================================================\n" <<
        "\n\n\n Nehézség - " << difficultyLvl[selectedDiff] << "\n  <- A     D ->\n\n Tábla mérete - 10 X 10\n\n Bombák száma - " <<
        globalBombCount << " db\n (fel) W     S (le)\n\n\n\n Főmenü - ESC\n" <<
        "==============================================================\n <-                        nehézség                        ->" << std::endl;
    bool end = false;
    while (!end) {
        int input = _getch();
        switch (input) {
        case 27: // ESC
            end = true; system("cls"); main(); break;
        case 77: // JOBBRA NYÍL
            controls(); break;
        case 75: // BALRA NYÍL
            info(); break;
        case 100: // d
            if (selectedDiff == 0 or selectedDiff == 1) selectedDiff++;
            else { selectedDiff = 0; } difficulty(); break;
        case 97: // a
            if (selectedDiff == 1 or selectedDiff == 2) selectedDiff--;
            else { selectedDiff = 2; } difficulty(); break;
        case 119: // w
            if (globalBombCount < 100) {
                globalBombCount++; selectedDiff = 3; difficulty(); break;
            }
            else { break; }
        case 115: // s
            if (globalBombCount > 0) {
                globalBombCount--; selectedDiff = 3; difficulty(); break;
            }
            else { break; }
        default:
            continue;
        }
    }
    return 3;
}

void controls() {
    system("cls");
    std::cout << "==============================================================\n" <<
        "                           IRÁNYÍTÁS\n" <<
        "==============================================================\n" <<
        "\n Billentyűkiosztás:\n\n\n Mozgás:\n\n Fel - Felfelé nyíl\n Le - lefelé nyíl\n Balra - Balra nyíl\n Jobbra - Jobbra nyíl\n\n Akciók:\n\n Mező feltárása - ENTER\n Mező megjelölése - BACKSPACE\n\n\n\n Főmenü - ESC\n" <<
        "==============================================================\n <-                        irányítás                        ->" << std::endl;
    bool end = false;
    while (!end) {
        int input = _getch();
        switch (input) {
        case 27: // ESC
            end = true; system("cls"); main(); break;
        case 77: // JOBBRA NYÍL
            credits(); break;
        case 75: // BALRA NYÍL
            diff = difficulty(); break;
        default:
            continue;
        }
    }
}

void credits() {
    system("cls");
    std::cout << "==============================================================\n" <<
        "                           KREDITEK\n" <<
        "==============================================================\n" <<
        "\n Készítette: DrJegesmedve\n\n Fejlesztő - DrJegesmedve\n Designer - DrJegesmedve\n\n Tesztelő - cnek\n Tesztelő - Szasza\n Tesztelő - dr.frustrumassae\n Tesztelő - K.O.\n\n\n\n Főmenü - ESC\n" <<
        "==============================================================\n <-                        kreditek                        ->" << std::endl;
    bool end = false;
    while (!end) {
        int input = _getch();
        switch (input) {
        case 27: // ESC
            end = true; system("cls"); main(); break;
        case 72: // FELFELÉ NYÍL
            end = true; system("cls"); main(); break;
        case 77: // JOBBRA NYÍL
            end = true; system("cls"); main(); break;
        case 75: // BALRA NYÍL
            controls(); break;
        default:
            continue;
        }
    }
}

void quit() {
    system("cls");
    std::cout << "==============================================================\n" <<
        "                           KILÉPSZ?\n" <<
        "==============================================================\n" <<
        "\n\n Biztosan ki szeretnél lépni? Ha igen, nyomj ENTER-t!\n\n\n\n Főmenü - ESC\n" <<
        "==============================================================\n <-                        kilépés                        ->" << std::endl;
    bool end = false;
    while (!end) {
        int input = _getch();
        switch (input) {
        case 27: // ESC
            end = true; system("cls"); main(); break;
        case 13: // ENTER
            system("cls");
            std::cout << " Várakozás a program bezárására\n 3.."; Sleep(1000);
            std::cout << " 2.. "; Sleep(1000);
            std::cout << "1..\n"; Sleep(1000);
            exit(0); break;
        default:
            continue;
        }
    }
}

int main() {
    setlocale(LC_ALL, "hun");
    std::cout << "==============================================================\n" <<
        "              Üdvözöllek az AKNAVADÁSZ játékban!\n" << "==============================================================\n" <<
        "\n Kérlek válassz az alábbiak közül:\n\n Ismertető - F1\n Nehézség beállítása - F2\n Irányítások - F3\n Kreditek - F4\n\n JÁTÉK - ENTER\n\n\n\n Kilépés - ESC\n" <<
        "==============================================================\n <-                         főmenü                         ->" << std::endl;
    // Input handler
    bool end = false;
    while (!end) {
        int input = _getch();
        switch (input) {
        case 13: // ENTER
            Game(); break; // Játék indítás a kiválasztott beállításokkal
        case 59: // F1
            info(); system("cls"); main(); break;
        case 60: // F2
            diff = difficulty(); system("cls"); main(); break;
        case 61: // F3
            controls(); system("cls"); main(); break;
        case 62: // F4
            credits(); system("cls"); main(); break;
        case 27: // ESC
            quit(); system("cls"); main(); break;
        case 75: // BALRA NYÍL
            credits(); system("cls"); main(); break;
        case 77: // JOBBRA NYÍL
            info(); system("cls"); main(); break;
        case 80: // LEFELÉ NYÍL
            credits(); system("cls"); main(); break;
        default:
            continue;
        }
    }
    

    return 0;
}
