#include <iostream>
#include <string>
#include <fstream>
#include <vector>

int main(){
    const int BUFF_SIZE = 16;
    std::string input_name, output_name;

    input_name = "test.tif";
    output_name = "raw_ima.tif";

    std::ifstream input_file(input_name,std::ios::binary);
    std::ofstream output_file(output_name,std::ios::binary);

    if(!input_file){
        std::cout << "not input file\n";
        return 0;
    }

    if(!output_file){
        std::cout << "not output file\n";
        return 0;
    }

    input_file.seekg(0,std::ios::end);
    const int filesize = input_file.tellg();
    input_file.clear();
    input_file.seekg(0,std::ios::beg);

    char file_ope[filesize];
    std::cout << filesize << std::endl;
    char buf[BUFF_SIZE];
    for(int i = 0; i < filesize; i+=BUFF_SIZE){
        input_file.read(buf,BUFF_SIZE);
        for(int j = 0; j < BUFF_SIZE; j++){
            file_ope[i+j] = buf[j];
        }
    }
    input_file.read(buf,filesize%BUFF_SIZE);
    for(int i = 0; i < filesize%BUFF_SIZE; i++){
        file_ope[filesize-filesize%BUFF_SIZE+i] = buf[i];
    }

    //test
    std::string mem;
    for(int i = 0; i < 2; i++){
        mem += file_ope[i];
    }
    output_file.write(file_ope,filesize);
}