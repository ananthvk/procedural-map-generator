#include "engine.hpp"
#include <filesystem>
using namespace logger;

#ifndef DATA_FOLDER
#define DATA_FOLDER "data"
#endif

auto main(int argc, char *argv[]) -> int
{
    try
    {
        Engine engine(std::filesystem::path(DATA_FOLDER));
        engine.run();
    }
    catch (confparse::parse_error &e)
    {
        error("Error occured: Config file corrupt - {}", e.what());
        return 1;
    }
    catch (std::exception &e)
    {
        error("Error occured: - {}", e.what());
        return 2;
    }

    return 0;
}