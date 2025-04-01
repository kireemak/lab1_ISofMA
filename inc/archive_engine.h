#ifndef ARCHIVE_ENGINE_H
#define ARCHIVE_ENGINE_H

#include <string>

int restoreFile(const std::string* archivePath);
int archiveFile(const std::string* source);
#endif // ARCHIVE_ENGINE_H

