#include "../inc/file_access.h"
#include <filesystem>
#include <iostream>
#ifdef _WIN32
#include <aclapi.h>
#include <windows.h>
#else
#include <sys/stat.h>
#endif

uint8_t FileAccessProfile::toFlags() {
    uint8_t result = 0;
    if (read) result |= 0b001;
    if (write) result |= 0b010;
    if (execute) result |= 0b100;
    return result;
}

void FileAccessProfile::loadFromByte(uint8_t data) {
    write = (data & WRITE_MASK) != 0;
    execute = (data & EXEC_MASK) != 0;
    read = (data & READ_MASK) != 0;
}

void applyAccessRules(const std::string& targetFile, FileAccessProfile profile) {
    std::filesystem::perms prms = std::filesystem::perms::none;
#ifdef _WIN32
    PSID pOwnerSid = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;
    if (GetNamedSecurityInfo(targetFile.c_str(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
        &pOwnerSid, nullptr, nullptr, nullptr, &pSD) != ERROR_SUCCESS) {
        return;
    }
    DWORD dwAccessMask = 0;
    if (profile.read) dwAccessMask |= GENERIC_READ | FILE_GENERIC_READ;
    if (profile.write) dwAccessMask |= GENERIC_WRITE | FILE_GENERIC_WRITE;
    if (profile.execute) dwAccessMask |= GENERIC_EXECUTE | FILE_GENERIC_EXECUTE;
    EXPLICIT_ACCESS ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = dwAccessMask;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
    ea.Trustee.ptstrName = (LPTSTR)pOwnerSid;
    PACL pNewDacl = nullptr;
    if (SetEntriesInAcl(1, &ea, nullptr, &pNewDacl) != ERROR_SUCCESS) {
        LocalFree(pSD);
        return;
    }
    DWORD dwResult = SetNamedSecurityInfo(const_cast<LPSTR>(targetFile.c_str()),
	SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, pNewDacl, nullptr);
    LocalFree(pNewDacl);
    LocalFree(pSD);
#else
    if (profile.execute) prms |= std::filesystem::perms::owner_exec;
    if (profile.read) prms |= std::filesystem::perms::owner_read;
    if (profile.write) prms |= std::filesystem::perms::owner_write;
    std::filesystem::path path(targetFile);
    std::filesystem::permissions(path, prms);
#endif
}

FileAccessProfile fetchAccessRules(const std::string& targetFile) {
    FileAccessProfile profile;

#ifdef _WIN32
    PSID pOwnerSid = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;
    if (GetNamedSecurityInfo(targetFile.c_str(), SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
        &pOwnerSid, nullptr, nullptr, nullptr, &pSD) != ERROR_SUCCESS) {
        return profile;
    }
    PACL pDacl = nullptr;
    BOOL daclPresent = FALSE;
    BOOL daclDefaulted = FALSE;
    if (!GetSecurityDescriptorDacl(pSD, &daclPresent, &pDacl, &daclDefaulted) || !daclPresent) {
        LocalFree(pSD);
        return profile;
    }
    for (DWORD i = 0; i < pDacl->AceCount; ++i) {
        ACCESS_ALLOWED_ACE* pAce;
        if (!GetAce(pDacl, i, (LPVOID*)&pAce)) {continue;}
        if (EqualSid(pOwnerSid, &pAce->SidStart)) {
            profile.execute = (pAce->Mask & GENERIC_EXECUTE) || (pAce->Mask & FILE_GENERIC_EXECUTE);
            profile.write = (pAce->Mask & GENERIC_WRITE) || (pAce->Mask & FILE_GENERIC_WRITE);
            profile.read = (pAce->Mask & GENERIC_READ) || (pAce->Mask & FILE_GENERIC_READ);
            break;
        }
    }
    LocalFree(pSD);
#else
    std::filesystem::perms permissions = std::filesystem::status(targetFile).permissions();
    profile.execute = (permissions & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
    profile.write = (permissions & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
    profile.read = (permissions & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
#endif
    return profile;
}
