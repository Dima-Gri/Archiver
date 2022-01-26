#include <string>
#include <vector>

class Treatment {
public:
    bool CheckFiles(const std::vector<std::string>& file);
    bool CheckFile(const std::string& names);
    void PrintInfo();
    Treatment(std::vector<std::string_view>& arguments);
    enum class Status{ DeArchive, Archive, Error, Info };
    std::vector<std::string> GetFiles();
    std::string GetArchiveName();
    std::string GetOutput();
    Treatment::Status GetStatus();
    
private:
    std::vector<std::string> files_;
    std::string archive_name_;
    std::string output_;
    Treatment::Status status_;
};
