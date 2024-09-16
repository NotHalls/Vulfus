// we are adding all the includes to make sure all of them are correctly set up
#include "Application.hpp"

// we need these for the base c++ functions in the code
// well you know that already.
#include <iostream>
#include <cstdlib>
#include <exception>



int main()
{
    Application APP;

#ifdef VF_DEBUG
    std::cout << "Running In Debug Mode" << std::endl;
#endif

    // basicly we run the application process and check for errors if there are any with exception handelling
    try
    {
        APP.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // i think EXIT_ looks cooler than just plain 0 or 1
    return EXIT_SUCCESS;
}