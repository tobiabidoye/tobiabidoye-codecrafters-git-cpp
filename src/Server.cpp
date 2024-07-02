#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <zlib.h>

//predeclared functions
//this function will assist in the opening of the catfile
void catfile(std::string & blobhash);

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here!\n";



     if (argc < 2) {
         std::cerr << "No command provided.\n";
         return EXIT_FAILURE;
     }

     std::string command = argv[1];

     if (command == "init") {
         try {
             std::filesystem::create_directory(".git");
             std::filesystem::create_directory(".git/objects");
             std::filesystem::create_directory(".git/refs");

             std::ofstream headFile(".git/HEAD");
             if (headFile.is_open()) {
                 headFile << "ref: refs/heads/main\n";
                 headFile.close();
             } else {
                 std::cerr << "Failed to create .git/HEAD file.\n";
                 return EXIT_FAILURE;
             }

             std::cout << "Initialized git directory\n";
         } catch (const std::filesystem::filesystem_error& e) {
             std::cerr << e.what() << '\n';
             return EXIT_FAILURE;
         }
     }else if(command == "cat-file"){

         if(argc != 4 || argv[2] != "-p"){
             //validating the number of arguments for the catfile command
             std::cerr << "Invalid arguments" << std::endl;
             return EXIT_FAILURE;
         }
         //there will be 4 arguments
         //constructing file path
         std::string blobHash = argv[3];
         catfile(blobHash);

     }else {
         std::cerr << "Unknown command " << command << '\n';
         return EXIT_FAILURE;
     }

     return EXIT_SUCCESS;
}

void catfile(std::string & blobHash){
    /*in an attempt to construct a filepath using a blobhash
     for example in this hash e88f7a929cd70b0274c4ea33b209c97fa845fdbc
     the first two characters e8 would be directory name and
     the object name would be the rest of the characters */

    std::string blobDir = blobHash.substr(0,2);
    std::string objName = blobHash.substr(2);
    std::string filePath = std::filesystem::path(".git")/"objects"/blobDir/objName;

    //now after all this we should open the file in binary for opening non txt files
    std::ifstream myfile(filePath, std::ios::binary);
    if(!myfile.is_open()){
        std::cout << "File could not be opened" << std::endl;
        return;
    }

    //extracting content from the binary file
    std::vector <char> binaryData;
    char fileContent;
    while(myfile.get(fileContent)){
        //storing extracted data into a vector
        binaryData.push_back(fileContent);
    }
    myfile.close();

    //extracted data is processed to a string then displayed

    std::string object_str{std::istreambuf_iterator<char>(myfile),
                           std::istreambuf_iterator<char>()};
    myfile.close();
    const auto object_content = object_str.substr(object_str.find('\0') + 1);
    std::cout << object_content << std::flush;
}