#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


struct Instruction
{
    char write = ' ';
    char move = 0;
    unsigned int state = INT_MAX - 3;
};


class Tape
{
    int head;
    int firstchunk;
    std::vector<char*> chunks;

    void FillTape()
    {
        while (firstchunk * 256 > head)
        {
            firstchunk--;
            chunks.insert(chunks.begin(), new char[256]);
            for (int i = 0; i < 256; i++)
            {
                chunks.front()[i] = '0';
            }
        }

        while (head >= (firstchunk + chunks.size()) * 256)
        {
            chunks.push_back(new char[256]);
            for (int i = 0; i < 256; i++)
            {
                chunks.back()[i] = '0';
            }
        }
    }

public:
    Tape()
    {
        head = 0;
        firstchunk = 0;
    }

    Tape(std::string input, int start)
    {
        head = start;
        firstchunk = 0;

        if (input != "")
        {
            for (int i = 0; i < input.size(); i++)
            {
                if (i % 256 == 0)
                {
                    chunks.push_back(new char[256]);
                }

                chunks[i / 256][i % 256] = input[i];
            }

            for (int i = input.size(); i % 256 != 0; i++)
            {
                chunks.back()[i] = '0';
            }
        }
    }

    void MoveHead(int d)
    {
        head += d;
    }

    char Read()
    {
        FillTape();

        return chunks[std::floor(head / 256.f) - firstchunk][head % 256];
    }

    void Write(char c)
    {
        FillTape();  // is this necessary?

        chunks[std::floor(head / 256.f) - firstchunk][head % 256] = c;
    }

    void Print()
    {
        for (char* i : chunks)
        {
            for (int j = 0; j < 256; j++)
            {
                std::cout << i[j];
            }
        }
    }
};


int main()
{
    std::cout << "Enter TM filename: ";
    std::string filename;
    std::cin >> filename;

    std::cout << "Reading...";
    std::ifstream t(filename);

    if (t.fail())
    {
        std::cout << "File read failed. Exiting...";
        return 0;
    }

    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string string = buffer.str();

    std::vector<std::string> lines;
    int j = 0;
    for (int i = 0; i < string.size(); i++)
    {
        if (string[i] == '\n')
        {
            lines.push_back(string.substr(j, i - j));
            j = i + 1;
        }
    }
    lines.push_back(string.substr(j));
    std::string dict = lines.front();
    lines.erase(lines.begin());

    Instruction* instructions = new Instruction[256 * lines.size()]();

    for (int i = 0; i < dict.size(); i++)
    {
        if (dict[i] != ' ' and dict[i] != '|')
        {
            for (int j = 0; j < lines.size(); j++)
            {
                if (lines[j].size() <= i) continue;

                if (lines[j][i] != ' ' and lines[j][i] != '|')
                {
                    if (lines[j].substr(i, 4) == "halt" or lines[j].substr(i, 4) == "HALT")
                    {
                        instructions[256 * j + dict[i]].write = dict[i];
                        instructions[256 * j + dict[i]].move = lines[j][i + 1] == 0;
                        instructions[256 * j + dict[i]].state = INT_MAX;
                    }
                    else if (lines[j].substr(i, 4) == "TRUE" or lines[j].substr(i, 4) == "true")
                    {
                        instructions[256 * j + dict[i]].write = dict[i];
                        instructions[256 * j + dict[i]].move = lines[j][i + 1] == 0;
                        instructions[256 * j + dict[i]].state = INT_MAX - 1;
                    }
                    else if(lines[j].substr(i, 5) == "FALSE" or lines[j].substr(i, 5) == "false")
                    {
                        instructions[256 * j + dict[i]].write = dict[i];
                        instructions[256 * j + dict[i]].move = lines[j][i + 1] == 0;
                        instructions[256 * j + dict[i]].state = INT_MAX - 2;
                    }
                    else
                    {
                        instructions[256 * j + dict[i]].write = lines[j][i];
                        instructions[256 * j + dict[i]].move = lines[j][i + 1] == 'L' ? -1 : 1;
                        instructions[256 * j + dict[i]].state = std::stoi(lines[j].substr(i + 2));
                    }
                }
            }
        }
    }

    std::cout << "Done!\nHow would you like to run?\n c - Feed initialised to 0\n i - Feed initialised with text input\n f - Feed initialised with file input\nEnter selection: ";
    std::cin >> filename;

    int head = 0;

    if (filename == "i")
    {
        std::cout << "Please enter the input: ";
        std::cin >> filename;
        std::cout << "Please enter the starting position of the head: ";
        std::cin >> head;
    }
    else if (filename == "f")
    {
        std::cout << "Please enter the input filename: ";
        std::cin >> filename;

        std::cout << "Reading...";
        std::ifstream file(filename);

        if (file.fail())
        {
            std::cout << "File read failed. Exiting...";
            return 0;
        }

        std::stringstream buffer2;
        buffer2 << file.rdbuf();
        filename = buffer2.str();

        std::cout << "Please enter the starting position of the head: ";
        std::cin >> head;
    }
    else
    {
        filename = "";
    }

    Tape tape(filename, head);

    std::cout << "Tape initialised. Running program...";

    int state = 0;

    while (state < INT_MAX - 3)
    {
        char currentSymbol = tape.Read();
        tape.Write(instructions[state * 256 + currentSymbol].write);
        tape.MoveHead(instructions[state * 256 + currentSymbol].move);
        state = instructions[state * 256 + currentSymbol].state;
    }

    switch (state)
    {
    case INT_MAX - 3:
        std::cout << "Error: Turing Machine reached empty instruction. Exiting...\n";
        break;
    case INT_MAX - 2:
        std::cout << "Turing Machine has exited FALSE.\n";
        break;
    case INT_MAX - 1:
        std::cout << "Turing Machine has exited TRUE.\n";
        break;
    case INT_MAX:
        std::cout << "Turing Machine has exited.\n";
        break;
    }

    std::cout << "Feed:\n";
    tape.Print();
    std::cout << std::endl;
}
