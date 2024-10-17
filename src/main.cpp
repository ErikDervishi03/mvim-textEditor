#include "../include/mvimStarter.hpp"  // Include your mvimStarter class header

int main(int argc, char* argv[])
{

  if (argc == 1)
  {
    mvimStarter mvimStarter;
    mvimStarter.run();
    return 0;
  }

  const char* filename = nullptr;    // To hold the filename
  bool benchmark = false;             // Flag for benchmarking mode

  // Parse command line arguments
  for (int i = 1; i < argc; ++i)
  {
    if (filename == nullptr)
    {
      // The first non-option argument should be the filename
      filename = argv[i];
    }
    else if (std::string(argv[i]) == "-b")
    {
      benchmark = true;        // Enable benchmarking
    }
  }

  // Create an instance of mvimStarter, passing the filename and benchmark flag
  mvimStarter mvimStarter(filename, benchmark);

  // Run the mvimStarter (this will only happen if not in benchmark mode)
  if (!benchmark)
  {
    mvimStarter.run();
  }

  return 0;    // Exit the program successfully
}
