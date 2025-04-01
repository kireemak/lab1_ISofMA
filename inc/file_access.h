#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include <string>

struct FileAccessProfile {
    bool execute;
    bool write;
    bool read;
    const uint8_t EXEC_MASK = 0b100;
    const uint8_t WRITE_MASK = 0b010;
    const uint8_t READ_MASK = 0b001;
    uint8_t toFlags();
    void loadFromByte(uint8_t data);
};

void applyAccessRules(const std::string& targetFile, FileAccessProfile profile);
FileAccessProfile fetchAccessRules(const std::string& targetFile);

#endif // FILE_ACCESS_H

