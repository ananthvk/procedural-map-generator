#include "engine.hpp"

using namespace logger;

#ifndef CONFIG_FILE_PATH
const char *CONFIG_FILE_PATH = "config.txt";
#endif

auto main(int argc, char *argv[]) -> int
{
    try
    {
        Engine engine(CONFIG_FILE_PATH);
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