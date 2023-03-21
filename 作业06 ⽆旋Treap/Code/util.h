#include <string>
#include <fstream>
#include <stdexcept>

#define PATH_BUILDER(GENRE) \
    static inline std::string GENRE##_path(int32_t round) { \
    return std::string("./data/"#GENRE"-") + std::to_string(round) + std::string(".txt"); \
} \

PATH_BUILDER(input);
PATH_BUILDER(output);
PATH_BUILDER(answer);

static inline std::string trim(const std::string& s) {
    int left = 0, right = s.size() - 1;
    while (left <= right && isspace(s[left])) left ++ ;
    while (left <= right && isspace(s[right])) right -- ;
    return s.substr(left, right - left + 1);
}

class JudgeFile {
public:
    JudgeFile(const std::string& path, std::ios_base::openmode mode)
        : file(path, mode) {
        if (!file.is_open()) {
            throw std::runtime_error("ERROR: Failed to open file: " + path);
        }
    }

    ~JudgeFile() {
        file.close();
    }

    std::fstream& get() {
        return file;
    }

private:
    std::fstream file;
};
