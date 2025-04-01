#include "../inc/archive_engine.h"
#include "../inc/file_access.h"
#include <string>
#include <zip.h> // Requires libzip package
#include <fstream>

#define OPERATION_SUCCESS 0
#define PACKAGE_CREATION_FAILED 1
#define PACKAGE_ACCESS_FAILED 1

int restoreFile(const std::string* packagePath) {
    int errorCode = 0;
    zip_t* package = zip_open(packagePath->c_str(), 0, &errorCode);
    if (!package) return PACKAGE_ACCESS_FAILED;

    // Read permissions
    zip_stat_t metaInfo;
    zip_stat_init(&metaInfo);
    zip_stat(package, "access_rules.dat", 0, &metaInfo);
    
    uint8_t rulesData;
    zip_file_t* rulesFile = zip_fopen(package, "access_rules.dat", 0);
    zip_fread(rulesFile, &rulesData, sizeof(uint8_t));
    zip_fclose(rulesFile);

    FileAccessProfile currentRules;
    currentRules.loadFromByte(rulesData);

    // Restore original file
    std::string outputFile = packagePath->substr(0, packagePath->find(".access.pkg"));
    zip_file_t* dataFile = zip_fopen(package, "original_data.dat", 0);
    
    std::ofstream outputStream(outputFile, std::ios::binary);
    const size_t bufferSize = 8192;
    char dataBuffer[bufferSize];
    zip_int64_t bytesProcessed;
    
    while ((bytesProcessed = zip_fread(dataFile, dataBuffer, bufferSize)) > 0) {
        outputStream.write(dataBuffer, bytesProcessed);
    }
    
    zip_fclose(dataFile);
    zip_close(package);
    
    //FileAccessProfile readOnly;
    //readOnly.read = true;
    //readOnly.write = false;
    //readOnly.execute = false;
    //applyAccessRules(outputFile, readOnly);

    // Apply restored rules
    applyAccessRules(outputFile, currentRules);
    return OPERATION_SUCCESS;
}

int archiveFile(const std::string* source) {
    FileAccessProfile rules = fetchAccessRules(*source);
    
    int errorCode = 0;
    std::string archiveName = *source + ".access.pkg";
    zip_t* package = zip_open(archiveName.c_str(), ZIP_CREATE | ZIP_EXCL, &errorCode);
    if (!package) return PACKAGE_CREATION_FAILED;

    // Add permissions metadata
    uint8_t encodedRules = rules.toFlags();
    zip_source_t* rulesSource = zip_source_buffer(package, &encodedRules, sizeof(uint8_t), 0);
    zip_file_add(package, "access_rules.dat", rulesSource, ZIP_FL_ENC_UTF_8);

    // Add file contents
    zip_source_t* contentSource = zip_source_file(package, source->c_str(), 0, 0);
    zip_file_add(package, "original_data.dat", contentSource, ZIP_FL_ENC_UTF_8);

    zip_close(package);
    return OPERATION_SUCCESS;
}