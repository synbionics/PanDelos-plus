#include <fstream>
#include <stdexcept>
#include <ctime>
#include <mutex>


#ifndef FILE_WRITER_INCLUDE_GUARD
#define FILE_WRITER_INCLUDE_GUARD
// #define DEBUG 1

namespace utilities {

    class FileWriter {
        private:
            using mutex_t = std::mutex;
            std::string fileName_;
            mutex_t mutex_;
            std::string getCurrentDateTime() const {
                std::time_t t = std::time(0);
                std::tm* now = std::localtime(&t);
                std::string out = std::to_string(now->tm_year + 1900) + "_" + std::to_string(now->tm_mon + 1) + "_"
                + std::to_string(now->tm_mday) + "-";
                
                if(now->tm_hour < 10)
                    out += "0";
                out += std::to_string(now->tm_hour) + "_";
                
                if(now->tm_min < 10)
                    out += "0";
                out += std::to_string(now->tm_min);
                
                return out;
            }
        public:
            FileWriter(std::string path, std::string fileName, std::string extension, bool date);
            std::fstream openWrite();
            std::fstream openAppend();
            void close(std::fstream& stream);
            void write(std::string line, std::fstream& file);
            ~FileWriter();
    };

    FileWriter::FileWriter(std::string path, std::string fileName, std::string extension, bool date) {
        
        std::fstream file;
        
        fileName = path + fileName;

        if(date)
            fileName = fileName + "_" + getCurrentDateTime();
        
        fileName += extension;

        file.open(fileName, std::ios::app);

        if(!file.is_open()){
            file.open(fileName, std::ios::out);
            file.close();
            file.open(fileName, std::ios::app);
            if(!file.is_open())
                throw std::runtime_error("missing file");
        }

        file<<"";
        file.close();
        fileName_ = fileName;
    }

    std::fstream FileWriter::openAppend() {
        return std::fstream(fileName_, std::ios::app);
    }

    std::fstream FileWriter::openWrite() {
        return std::fstream(fileName_, std::ios::out);
    }

    void FileWriter::close(std::fstream& file) {
        file.close();
    }

    void FileWriter::write(std::string line, std::fstream& file) {
        {
            std::unique_lock<mutex_t> lock(mutex_);
            file<<line<<"\n";
        }
    }

    FileWriter::~FileWriter() {
    }

}
#endif