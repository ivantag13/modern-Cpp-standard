#include <iostream>
#include <cstdio>

class FileHandler
{
    FILE *f;

public:
    FileHandler(const char *filename)
    {
        f = fopen(filename, "r");
        if (!f)
            throw std::runtime_error("Cannot open file");
    }

    ~FileHandler()
    {
        if (f)
            fclose(f);
        std::cout << "File closed.\n";
    }
};

int main()
{
    try
    {
        FileHandler fh("data.txt");
        // use file here...
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }
} // fh destroyed → ~FileHandler() called → fclose()
