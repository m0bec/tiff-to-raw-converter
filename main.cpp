#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iomanip>

struct TifTag{
    unsigned int TagId;
    unsigned int DataType;
    unsigned int DataCount;
    unsigned int DataOffset;

    TifTag(){
        TagId = 0;
        DataType = 0;
        DataCount = 0;
        DataOffset = 0;
    }
};

struct TifIfd{
    const unsigned int ENTRY_SIZE = 2;
    const unsigned int IDF_ID_SIZE = 2;
    const unsigned int IDF_TYPE_SIZE = 2;
    const unsigned int IDF_COUNT_SIZE = 4;
    const unsigned int IDF_OFFSET_SIZE = 4;
    const unsigned int NEXT_IDF_SIZE = 4;

    unsigned int NumDirEntries;
    std::vector<TifTag> TagList;
    unsigned int NextIFDOffset;

    TifIfd(){
        NumDirEntries = 0;
        NextIFDOffset = 0;
    }
};

void ReadBin(bool little_en_flag, unsigned int &read_point, unsigned char *read_file, unsigned int &target, unsigned int read_size){
    const int SHIFT = 8;
    for(int i = 0; i < read_size; i++){
        if(little_en_flag){
            target += static_cast<unsigned int>(read_file[read_point]) << (i*SHIFT);
        }
        else{
            target += static_cast<unsigned int>(read_file[read_point]) << ((read_size-1-i)*SHIFT);
        }
        ++read_point;
    }
}

int main(){
    const int BUFF_SIZE = 16;
    const int SHIFT_SIZE = 8;
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
    unsigned int array_point = 0;

    bool little_endian_flag;
    if(std::equal(file_ope,file_ope+sizeof(HEADER_DIRECT_RIGHT),
                    HEADER_DIRECT_RIGHT,HEADER_DIRECT_RIGHT+sizeof(HEADER_DIRECT_RIGHT))){
        little_endian_flag = false;
    }
    else if(std::equal(file_ope,file_ope+sizeof(HEADER_DIRECT_LEFT),
                        HEADER_DIRECT_LEFT,HEADER_DIRECT_LEFT+sizeof(HEADER_DIRECT_LEFT))){
        little_endian_flag = true;
    }
    else{
        std::cout << "This file is not tiff\n";
        return 0;
    }
    array_point += sizeof(HEADER_DIRECT_RIGHT);

    for(int i = 0; i < sizeof(TIFF_HEADER); i++){
        if(little_endian_flag)  header[i] = file_ope[array_point+sizeof(TIFF_HEADER)-1-i];
        else     header[i] = file_ope[array_point+i];
    }
    array_point += sizeof(TIFF_HEADER);

    if(std::equal(header,header+sizeof(TIFF_HEADER),
                    TIFF_HEADER,TIFF_HEADER+sizeof(TIFF_HEADER))){
        std::cout << "OK little tiff.\n";
    }
    else if(std::equal(header,header+sizeof(TIFF_BIG_HEADER),
                    TIFF_BIG_HEADER,TIFF_BIG_HEADER+sizeof(TIFF_BIG_HEADER))){
        std::cout << "Sorry Don't support big tiff.\n";
        return 0;
    }
        
    unsigned int in_idf_point = 0;
    ReadBin(little_endian_flag, array_point, file_ope, in_idf_point, HEADER_START_IFD);

    std::cout << "Start IFD : " << in_idf_point << std::endl;

    TifIfd tag_data;

    ReadBin(little_endian_flag, in_idf_point, file_ope, tag_data.NumDirEntries, tag_data.ENTRY_SIZE);

    std::cout << "IDF entry count : " << tag_data.NumDirEntries << std::endl;

    for(int i = 0; i < tag_data.NumDirEntries; i++){
        TifTag mem_tif_tag;

        ReadBin(little_endian_flag, in_idf_point, file_ope, mem_tif_tag.TagId, tag_data.IDF_ID_SIZE);
        ReadBin(little_endian_flag, in_idf_point, file_ope, mem_tif_tag.DataType, tag_data.IDF_TYPE_SIZE);
        ReadBin(little_endian_flag, in_idf_point, file_ope, mem_tif_tag.DataCount, tag_data.IDF_COUNT_SIZE);
        ReadBin(little_endian_flag, in_idf_point, file_ope, mem_tif_tag.DataOffset, tag_data.IDF_OFFSET_SIZE);

        std::cout << "IDF num : "
            << std::setw(6)
            << std::setfill('0')
            << mem_tif_tag.TagId
            << std::endl;

        tag_data.TagList.push_back(mem_tif_tag);
    }

    ReadBin(little_endian_flag, in_idf_point, file_ope, tag_data.NextIFDOffset, tag_data.NEXT_IDF_SIZE);

    std::cout << "Next IDF : "
        << std::setw(6)
        << std::setfill('0')
        << tag_data.NextIFDOffset
        << std::endl;

    std::cout << in_idf_point << std::endl;


    //test----------------------------------------------------
    std::string mem;
    for(int i = 0; i < 2; i++){
        mem += file_ope[i];
    }
    output_file.write(reinterpret_cast<char*>(file_ope),FILESIZE);
    //test fin--------------------------------------------------
}