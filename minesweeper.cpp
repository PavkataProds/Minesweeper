#include <iostream>
#include <thread>
using namespace std;

const size_t MINIMUM_MATRIX_SIZE = 5;       //don't change bellow 5 or generateMines function won't work,
//since it also generates a 0 in the start square
const size_t MAXIMUM_MATRIX_SIZE = 10;
const int BOMB_SIGN = -1;                   //bomb identifier in the hidden matrix
const unsigned int STALL_MULTIPLIER = 1;    //seconds after a message

//commands
const char OPEN_COMMAND[50] = "open";
const char MARK_COMMAND[50] = "mark";
const char UNMARK_COMMAND[50] = "unmark";

//design
const char UNMARKED_POS = '#';
const char MARKED_POS = '@';
const char BOMB_POS = 'B';

bool isInteger(char* input)
{
    while (*input != '\0')
    {
        if (*input < '0' || *input > '9') return false;
        input++;
    }
    return true;

}
int stringToInt(const char* number)
{
    int answer = 0;
    while (*number != '\0')
    {
        if (*number > '9' || *number < '0') return -1;
        answer *= 10;
        answer += *number - '0';
        number++;
    }
    return answer;
}
void inputInRange(size_t& input, int min, int max) //a controlled input for size and mines
{
    bool notInRange = true;
    char numberChar[20];
    do
    {
        cin >> numberChar;
        if (isInteger(numberChar)) input = stringToInt(numberChar);
        else
        {
            cout << "Input isn't valid. Enter a number: ";
            continue;
        }
        notInRange = input < min || input > max;
        if (notInRange)
        {
            cout << "Input out of range. Enter a new number: ";
        }
    } while (notInRange);
}
void fillIntMatrix(int field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], size_t size, int element)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            field[i][j] = element;
        }
    }
}void fillCharMatrix(char field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], size_t size, char element)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            field[i][j] = element;
        }
    }
}
//void printIntMatrix(int field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], size_t size)     //prints the hidden matrix
//{
//    for (size_t i = 0; i < size; i++)
//    {
//        for (size_t j = 0; j < size; j++)
//        {
//            cout << field[i][j] << " ";
//            if (field[i][j] >= 0) cout << " ";
//        }
//        cout << endl;
//    }
//}
void printMatrix(char field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            cout << field[i][j] << " ";
        }
        cout << endl;
    }
}
int absolute(int value)
{
    return value < 0 ? -value : value;
}
void generateRandomCoordinates(size_t& x, size_t& y, size_t  size)
{
    x = rand() % size;
    y = rand() % size;
}
void generateMines(int field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], size_t size, int bombCount, int bombCoords[3 * MAXIMUM_MATRIX_SIZE][2], int firstX, int firstY)
{
    size_t counter = 0, x, y;
    while (counter < bombCount)
    {
        generateRandomCoordinates(x, y, size);
        if (field[x][y] == BOMB_SIGN || (absolute(firstX - x) <= 1 && absolute(firstY - y) <= 1)) continue;
        //if it generates a bomb over another bomb or near the first move, generate another!
        //That's why the minimal matrix size should be 5

        field[x][y] = BOMB_SIGN;
        bombCoords[counter][0] = x;
        bombCoords[counter][1] = y;
        counter++;
    }
}
void generateBoxBombCount(int field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], size_t size, int bombCount, int bombCoords[3 * MAXIMUM_MATRIX_SIZE][2])
{
    size_t bombCounter = 0;
    while (bombCounter < bombCount)
    {
        int x = bombCoords[bombCounter][0];
        int y = bombCoords[bombCounter][1];

        bool isOnTheTop = x - 1 < 0, isOnTheBottom = x + 1 >= size;
        bool isOnTheLeft = y - 1 < 0, isOnTheRight = y + 1 >= size;
        if (!isOnTheTop)
        {
            if (field[x - 1][y] != BOMB_SIGN) field[x - 1][y]++;
            if (!isOnTheLeft && field[x - 1][y - 1] != BOMB_SIGN) field[x - 1][y - 1]++;
            if (!isOnTheRight && field[x - 1][y + 1] != BOMB_SIGN) field[x - 1][y + 1]++;
        }
        if (!isOnTheBottom)
        {
            if (field[x + 1][y] != BOMB_SIGN) field[x + 1][y]++;
            if (!isOnTheLeft && field[x + 1][y - 1] != BOMB_SIGN) field[x + 1][y - 1]++;
            if (!isOnTheRight && field[x + 1][y + 1] != BOMB_SIGN) field[x + 1][y + 1]++;
        }
        if (!isOnTheLeft && field[x][y - 1] != BOMB_SIGN) field[x][y - 1]++;
        if (!isOnTheRight && field[x][y + 1] != BOMB_SIGN) field[x][y + 1]++;

        bombCounter++;
    }
}
char intToChar(int number)
{
    if (number > 9 || number < 0) return '!';
    return '0' + number;
}
void revealZeros(char interfaceMatrix[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], int field[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE],
    int x, int y, size_t size, size_t& countOfOpenSquares) //if the player finds a zero it reveals all neighbour zeros and numbers using recurion
{
    interfaceMatrix[x][y] = intToChar(field[x][y]);
    countOfOpenSquares++;
    bool isOnTheTop = x - 1 < 0, isOnTheBottom = x + 1 >= size;
    bool isOnTheLeft = y - 1 < 0, isOnTheRight = y + 1 >= size;
    if (!isOnTheTop)
    {
        if (!field[x - 1][y] && interfaceMatrix[x - 1][y] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x - 1, y, size, countOfOpenSquares);
        else if (interfaceMatrix[x - 1][y] == UNMARKED_POS)
        {
            interfaceMatrix[x - 1][y] = intToChar(field[x - 1][y]);
            countOfOpenSquares++;
        }

        if (!isOnTheLeft && !field[x - 1][y - 1] && interfaceMatrix[x - 1][y - 1] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x - 1, y - 1, size, countOfOpenSquares);
        else if (!isOnTheLeft && interfaceMatrix[x - 1][y - 1] == UNMARKED_POS)
        {
            interfaceMatrix[x - 1][y - 1] = intToChar(field[x - 1][y - 1]);
            countOfOpenSquares++;
        }

        if (!isOnTheRight && !field[x - 1][y + 1] && interfaceMatrix[x - 1][y + 1] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x - 1, y + 1, size, countOfOpenSquares);
        else if (!isOnTheRight && interfaceMatrix[x - 1][y + 1] == UNMARKED_POS)
        {
            interfaceMatrix[x - 1][y + 1] = intToChar(field[x - 1][y + 1]);
            countOfOpenSquares++;
        }
    }
    if (!isOnTheBottom)
    {
        if (!field[x + 1][y] && interfaceMatrix[x + 1][y] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x + 1, y, size, countOfOpenSquares);
        else if (interfaceMatrix[x + 1][y] == UNMARKED_POS)
        {
            interfaceMatrix[x + 1][y] = intToChar(field[x + 1][y]);
            countOfOpenSquares++;
        }

        if (!isOnTheLeft && !field[x + 1][y - 1] && interfaceMatrix[x + 1][y - 1] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x + 1, y - 1, size, countOfOpenSquares);
        else if (!isOnTheLeft && interfaceMatrix[x + 1][y - 1] == UNMARKED_POS)
        {
            interfaceMatrix[x + 1][y - 1] = intToChar(field[x + 1][y - 1]);
            countOfOpenSquares++;
        }

        if (!isOnTheRight && !field[x + 1][y + 1] && interfaceMatrix[x + 1][y + 1] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x + 1, y + 1, size, countOfOpenSquares);
        else if (!isOnTheRight && interfaceMatrix[x + 1][y + 1] == UNMARKED_POS)
        {
            interfaceMatrix[x + 1][y + 1] = intToChar(field[x + 1][y + 1]);
            countOfOpenSquares++;
        }
    }
    if (!isOnTheLeft && !field[x][y - 1] && interfaceMatrix[x][y - 1] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x, y - 1, size, countOfOpenSquares);
    else if (!isOnTheLeft && interfaceMatrix[x][y - 1] == UNMARKED_POS)
    {
        interfaceMatrix[x][y - 1] = intToChar(field[x][y - 1]);
        countOfOpenSquares++;
    }
    if (!isOnTheRight && !field[x][y + 1] && interfaceMatrix[x][y + 1] == UNMARKED_POS) revealZeros(interfaceMatrix, field, x, y + 1, size, countOfOpenSquares);
    else if (!isOnTheRight && interfaceMatrix[x][y + 1] == UNMARKED_POS)
    {
        interfaceMatrix[x][y + 1] = intToChar(field[x][y + 1]);
        countOfOpenSquares++;
    }
}
void commandInput(char interfaceMatrix[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE], int hiddenMatrix[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE],
    size_t size, size_t& countOfOpenSquares, bool& firstMove, bool& gameLost, size_t bombCount, int bombCoords[3 * MAXIMUM_MATRIX_SIZE][2])
{   //doesn't validate any input and checks for possible mistakes by the user, executes 3 commands defined as constants

    char command[50];
    char coordXChar[10];
    char coordYChar[10];
    cout << "Enter a command ([command] [x coordinate] [y coordinate]): ";
    cin >> command >> coordXChar >> coordYChar;

    if (!isInteger(coordXChar) || !isInteger(coordYChar))
    {
        cout << "Invalid coordinates." << endl;
        this_thread::sleep_for(1000ms * STALL_MULTIPLIER);
        return;
    }
    int coordX = stringToInt(coordXChar);
    int coordY = stringToInt(coordYChar);
    if (coordX >= size || coordY >= size)
    {
        cout << "Out of range." << endl;
        this_thread::sleep_for(1000ms * STALL_MULTIPLIER);
        return;
    }
    if (!strcmp(command, OPEN_COMMAND))
    {
        if (interfaceMatrix[coordX][coordY] != UNMARKED_POS)
        {
            cout << "Can't open square." << endl;
            this_thread::sleep_for(1000ms * STALL_MULTIPLIER);
            return;
        }
        else if (firstMove)
        {
            generateMines(hiddenMatrix, size, bombCount, bombCoords, coordX, coordY);
            generateBoxBombCount(hiddenMatrix, size, bombCount, bombCoords);
            firstMove = false;
        }
        else if (hiddenMatrix[coordX][coordY] == BOMB_SIGN)
        {
            gameLost = true;
            return;
        }
        if (!hiddenMatrix[coordX][coordY]) //== 0
        {
            revealZeros(interfaceMatrix, hiddenMatrix, coordX, coordY, size, countOfOpenSquares);
        }
        else
        {
            interfaceMatrix[coordX][coordY] = intToChar(hiddenMatrix[coordX][coordY]);
            countOfOpenSquares++;
        }

        return;
    }
    else if (!strcmp(command, MARK_COMMAND))
    {
        if (interfaceMatrix[coordX][coordY] == UNMARKED_POS) interfaceMatrix[coordX][coordY] = MARKED_POS;
        else
        {
            cout << "Square can't be marked." << endl;
            this_thread::sleep_for(1000ms * STALL_MULTIPLIER);
        }
        return;
    }
    else if (!strcmp(command, UNMARK_COMMAND))
    {
        if (interfaceMatrix[coordX][coordY] == MARKED_POS) interfaceMatrix[coordX][coordY] = UNMARKED_POS;
        else
        {
            cout << "Square isn't marked." << endl;
            this_thread::sleep_for(1000ms * STALL_MULTIPLIER);
        }
        return;
    }
    else
    {
        cout << "Invalid command." << endl;
        this_thread::sleep_for(1000ms * STALL_MULTIPLIER);
        return;
    }
}
void showBombs(char interfaceMatrix[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE],
    int bombCount, int bombCoords[3 * MAXIMUM_MATRIX_SIZE][2])
{
    for (size_t i = 0; i < bombCount; i++)
    {
        interfaceMatrix[bombCoords[i][0]][bombCoords[i][1]] = BOMB_POS;
    }
}

int main()
{
    srand(time(0)); //makes sure a random value is used every time

    size_t size;
    size_t bombCount;
    cout << "Enter size (" << MINIMUM_MATRIX_SIZE << " - " << MAXIMUM_MATRIX_SIZE << "): ";
    inputInRange(size, MINIMUM_MATRIX_SIZE, MAXIMUM_MATRIX_SIZE);

    cout << "Count of mines" << "(1 - " << 3 * size << "): ";
    inputInRange(bombCount, 1, 3 * size);

    int hiddenMatrix[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE];
    int bombCoords[3 * MAXIMUM_MATRIX_SIZE][2]; // 2-dimensional (matrix)

    fillIntMatrix(hiddenMatrix, size, 0);
    //printIntMatrix(hiddenMatrix, size);

    char interfaceMatrix[MAXIMUM_MATRIX_SIZE][MAXIMUM_MATRIX_SIZE];
    fillCharMatrix(interfaceMatrix, size, UNMARKED_POS);

    bool gameWon = false, gameLost = false;
    bool firstMove = true;
    size_t countOfOpenSquares = 0;

    while (!gameLost && !gameWon)
    {
        system("CLS");
        printMatrix(interfaceMatrix, size);
        commandInput(interfaceMatrix, hiddenMatrix, size, countOfOpenSquares, firstMove, gameLost, bombCount, bombCoords);
        gameWon = countOfOpenSquares >= (size * size) - bombCount;
    }
    system("CLS");
    showBombs(interfaceMatrix, bombCount, bombCoords);
    printMatrix(interfaceMatrix, size);
    if (gameLost)
    {
        cout << "You exploded! :(" << endl;
    }
    else if (gameWon)
    {
        cout << "You won! What a clearance!" << endl;
    }

    return 0;
}