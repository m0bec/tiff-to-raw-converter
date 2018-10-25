#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

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
    const unsigned int FILESIZE = input_file.tellg();
    input_file.clear();
    input_file.seekg(0,std::ios::beg);

    unsigned char file_ope[FILESIZE];
    std::cout << FILESIZE << std::endl;
    input_file.read(reinterpret_cast<char*>(file_ope),FILESIZE);

    const unsigned char HEADER_DIRECT_RIGHT[2] = {0x4d,0x4d};
    const unsigned char HEADER_DIRECT_LEFT[2] = {0x49, 0x49};
    const unsigned char TIFF_HEADER[2] = {0x00,0x2A};
    const unsigned char TIFF_BIG_HEADER[2] = {0x00,0x2B};
    const unsigned int HEADER_START_IFD = 4;

    unsigned char header[8];
    unsigned long long mem_number;
    unsigned long long array_point = 0;

    if(std::equal(file_ope,file_ope+sizeof(HEADER_DIRECT_RIGHT),
                    HEADER_DIRECT_RIGHT,HEADER_DIRECT_RIGHT+sizeof(HEADER_DIRECT_RIGHT))){
        std::cout << "Sorry. Don't support this tiff file.\n";
        return 0;
    }
    else if(std::equal(file_ope,file_ope+sizeof(HEADER_DIRECT_LEFT),
                        HEADER_DIRECT_LEFT,HEADER_DIRECT_LEFT+sizeof(HEADER_DIRECT_LEFT))){
       
        array_point += sizeof(HEADER_DIRECT_RIGHT);

        for(int i = 0; i < sizeof(TIFF_HEADER); i++){
            header[i] = file_ope[array_point+sizeof(TIFF_HEADER)-1-i];
        }

        if(std::equal(header,header+sizeof(TIFF_HEADER),
                        TIFF_HEADER,TIFF_HEADER+sizeof(TIFF_HEADER))){
            std::cout << "OK little tiff.\n";
        }
        else if(std::equal(header,header+sizeof(TIFF_BIG_HEADER),
                        TIFF_BIG_HEADER,TIFF_BIG_HEADER+sizeof(TIFF_BIG_HEADER))){
            std::cout << "Sorry Don't support big tiff.\n";
            return 0;
        }


        array_point += sizeof(TIFF_HEADER);
        unsigned int start_ifd_point = 0;
        for(int i = 0; i < HEADER_START_IFD;i++){
            start_ifd_point += file_ope[array_point+i]<<(i*8);
        }

        array_point += HEADER_START_IFD;
        std::cout << start_ifd_point << std::endl;
    }

    //test----------------------------------------------------
    std::string mem;
    for(int i = 0; i < 2; i++){
        mem += file_ope[i];
    }
    output_file.write(reinterpret_cast<char*>(file_ope),FILESIZE);
    //test fin--------------------------------------------------
}