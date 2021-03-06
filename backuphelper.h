#ifndef BACKUPHELPER_H
#define BACKUPHELPER_H

#include "./global.h"

#include <c++utilities/io/nativefilestream.h>

namespace Media {

class MediaFileInfo;

namespace BackupHelper {

TAG_PARSER_EXPORT std::string &backupDirectory();
TAG_PARSER_EXPORT void restoreOriginalFileFromBackupFile(const std::string &originalPath, const std::string &backupPath, IoUtilities::NativeFileStream &originalStream, IoUtilities::NativeFileStream &backupStream);
TAG_PARSER_EXPORT void createBackupFile(const std::string &originalPath, std::string &backupPath, IoUtilities::NativeFileStream &originalStream, IoUtilities::NativeFileStream &backupStream);
TAG_PARSER_EXPORT void handleFailureAfterFileModified(MediaFileInfo &mediaFileInfo, const std::string &backupPath, IoUtilities::NativeFileStream &outputStream, IoUtilities::NativeFileStream &backupStream, const std::string &context = "making file");

}

}

#endif // BACKUPHELPER_H
