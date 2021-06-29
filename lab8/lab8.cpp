#include <iostream>
#include <optional>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>
#include "BMP library/EasyBMP.h"
#include "BlurTask.h"
#include "ThreadsPool.h"
#include <chrono>

struct  Args
{
    std::string mode;
    std::string inputDir;
    std::string outputDir;
    int threadsNumber;
    int blocksNumber;
};


std::optional<Args> ParseArgs(int argc, char* argv[])
{
    if (argc < 6)
    {
        std::cout << "Usage: {lab8.exe} {mode} {input directory} {output directory} {threads number} {blocks number}\n";
        return std::nullopt;
    }

    Args args;

    args.threadsNumber = atoi(argv[4]);
    args.blocksNumber = atoi(argv[5]);

    if (args.blocksNumber < args.threadsNumber)
    {
        std::cout << "Number of blocks mustn't be less then number of threads\n";
        return std::nullopt;
    }

    args.mode = argv[1];
    args.inputDir = argv[2];
    args.outputDir = argv[3];

    return args;
}

std::vector<std::filesystem::path> GetFile(std::string dirName)
{

    std::vector<std::filesystem::path> fileNames;
    for (auto& p : std::filesystem::directory_iterator(dirName))
    {
        if (p.path().extension().string() == ".bmp")
        {
            fileNames.push_back(p.path());
        }
    }

    return fileNames;
}

std::vector<BlurParams> GetBlurParams(BMP& bmp, int blocksNumber)
{
    std::vector<BlurParams> blurParams;
    int stripeWidth = bmp.TellWidth() / blocksNumber;
    int remainingPixels = bmp.TellWidth() % blocksNumber;
    int currentWidth = 0;
    for (int i = 0; i < blocksNumber; i++)
    {
        int width;
        if (i != blocksNumber - 1)
        {
            width = currentWidth + stripeWidth;
        }
        else
        {
            width = currentWidth + stripeWidth + remainingPixels;
        }
        BlurParams params = { bmp, currentWidth, width };
        blurParams.push_back(params);
        currentWidth = width;
    }

    return blurParams;
}

void Run(std::optional<Args> args, std::vector<ITask*> tasks)
{
    if (args->mode == "1")
    {
        ThreadsPool pool(tasks, args->threadsNumber);
        pool.Execute();
    }
    else if (args->mode == "2")
    {
        HANDLE* handles = new HANDLE[args->blocksNumber];
        for (int i = 0; i < args->blocksNumber; i++)
        {
            handles[i] = CreateThread(NULL, 0, &ThreadProc, tasks[i], 0, NULL);
        }

        WaitForMultipleObjects(args->blocksNumber, handles, true, INFINITE);
    }
    else
    {
        std::cout << "Invalid mode number\n";
    }
    
}

int main(int argc, char* argv[])
{
    auto startTime = std::chrono::high_resolution_clock::now();

    auto args = ParseArgs(argc, argv);

    if (!args)
    {
        return 1;
    }

   auto files = GetFile(args->inputDir);

   for (auto& file : files)
   {
       BMP image;
       image.ReadFromFile(file.string().c_str());

       auto blurParams = GetBlurParams(image, args->blocksNumber);
       
       std::vector<ITask*> tasks;

       for (auto i = 0; i < blurParams.size(); ++i)
       {
           tasks.push_back(new BlurTask(3, &blurParams[i]));
       }

       Run(args, tasks);

       std::string output = args->outputDir + "/" + file.filename().string();
       image.WriteToFile(output.c_str());
   }

   auto endTime = std::chrono::high_resolution_clock::now();
   const auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
   std::cout << "runtime = " << runtime << std::endl;
}
