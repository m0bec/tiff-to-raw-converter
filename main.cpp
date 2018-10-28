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
    unsigned long long array_point = 0;
    unsigned int start_ifd_point = 0;

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
        
        for(int i = 0; i < HEADER_START_IFD;i++){
            start_ifd_point += file_ope[array_point+i]<<(i*SHIFT_SIZE);
        }

        array_point += HEADER_START_IFD;
        std::cout << start_ifd_point << std::endl;
    }

    TifIfd tag_data;

    for(int i = 0; i < tag_data.ENTRY_SIZE; i++){
        tag_data.NumDirEntries +=
            static_cast<unsigned int>(file_ope[start_ifd_point+i])<<(i*SHIFT_SIZE);
    }

    std::cout << "IDF entry count : " << tag_data.NumDirEntries << std::endl;

    unsigned int in_idf_point = start_ifd_point + tag_data.ENTRY_SIZE;

    for(int i = 0; i < tag_data.NumDirEntries; i++){
        TifTag mem_tif_tag;
        for(int j = 0; j < tag_data.IDF_ID_SIZE; j++){
            mem_tif_tag.TagId += 
                static_cast<unsigned int>(file_ope[in_idf_point])<<(j*SHIFT_SIZE);
            ++in_idf_point;
        }
        for(int j = 0; j < tag_data.IDF_TYPE_SIZE; j++){
            mem_tif_tag.DataType +=
                static_cast<unsigned int>(file_ope[in_idf_point])<<(j*SHIFT_SIZE);
            ++in_idf_point;
        }
        for(int j = 0; j < tag_data.IDF_COUNT_SIZE; j++){
            mem_tif_tag.DataCount +=
                static_cast<unsigned int>(file_ope[in_idf_point])<<(j*SHIFT_SIZE);
            ++in_idf_point;
        }
        for(int j = 0; j < tag_data.IDF_OFFSET_SIZE; j++){
            mem_tif_tag.DataOffset +=
                static_cast<unsigned int>(file_ope[in_idf_point])<<(j*SHIFT_SIZE);
            ++in_idf_point;
        }

        std::cout << "IDF num : "
            << std::setw(6)
            << std::setfill('0')
            << mem_tif_tag.TagId
            << std::endl;

        tag_data.TagList.push_back(mem_tif_tag);
    }
    for(int i = 0; i < tag_data.NEXT_IDF_SIZE; i++){
        tag_data.NextIFDOffset +=
            static_cast<unsigned int>(file_ope[in_idf_point])<<(i*SHIFT_SIZE);
        ++in_idf_point;
    }

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