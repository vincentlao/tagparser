#include "./helper.h"

#include "../mediafileinfo.h"
#include "../abstracttrack.h"
#include "../tag.h"

#include <c++utilities/io/catchiofailure.h>
#include <c++utilities/tests/testutils.h>
using namespace TestUtilities;

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdio>

using namespace std;
using namespace Media;
using namespace IoUtilities;
using namespace TestUtilities::Literals;

using namespace CPPUNIT_NS;

/*!
 * \brief The MediaFileInfoTests tests convenience methods provided by MediaFileInfo.
 * \remarks It only tests parsing a simple Mkv/Mp4. Parsing different formats with different settings is tested in OverallTests.
 */
class MediaFileInfoTests : public TestFixture {
    CPPUNIT_TEST_SUITE(MediaFileInfoTests);
    CPPUNIT_TEST(testInitialStatus);
    CPPUNIT_TEST(testFileSystemMethods);
    CPPUNIT_TEST(testParsingUnsupportedFile);
    CPPUNIT_TEST(testFullParseAndFurtherProperties);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testInitialStatus();
    void testFileSystemMethods();
    void testParsingUnsupportedFile();
    void testPartialParsingAndTagCreationOfMp4File();

    void testFullParseAndFurtherProperties();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MediaFileInfoTests);

void MediaFileInfoTests::setUp()
{
}

void MediaFileInfoTests::tearDown()
{
}

void MediaFileInfoTests::testInitialStatus()
{
    const MediaFileInfo file;
    CPPUNIT_ASSERT(!file.areTagsSupported());
    CPPUNIT_ASSERT(!file.areTracksSupported());
    CPPUNIT_ASSERT(!file.areChaptersSupported());
    CPPUNIT_ASSERT(!file.areAttachmentsSupported());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.containerParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.tagsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.tracksParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.chaptersParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.attachmentsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ContainerFormat::Unknown, file.containerFormat());
}

void MediaFileInfoTests::testFileSystemMethods()
{
    MediaFileInfo file("/usr/bin/unsupported.bin");
    CPPUNIT_ASSERT_EQUAL("/usr/bin"s, file.containingDirectory());
    CPPUNIT_ASSERT_EQUAL("unsupported.bin"s, file.fileName());
    CPPUNIT_ASSERT_EQUAL("unsupported"s, file.fileName(true));
    CPPUNIT_ASSERT_EQUAL("/usr/bin/unsupported"s, file.pathWithoutExtension());
    CPPUNIT_ASSERT_EQUAL(".bin"s, file.extension());
    CPPUNIT_ASSERT_EQUAL(static_cast<uint64>(0), file.size());
    file.reportPathChanged(testFilePath("unsupported.bin"));
    file.open(true);
    CPPUNIT_ASSERT(file.isOpen());
    CPPUNIT_ASSERT(file.isReadOnly());
    CPPUNIT_ASSERT_EQUAL(static_cast<uint64>(41), file.size());
}

void MediaFileInfoTests::testParsingUnsupportedFile()
{
    MediaFileInfo file(testFilePath("unsupported.bin"));
    file.parseContainerFormat();
    file.parseTags();
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotSupported, file.containerParsingStatus());
    // NOTE: parsing tags of unsupported container is actually supported: there is nothing to do
    // but maybe not what one would expect?
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.tagsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.tracksParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.chaptersParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.attachmentsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ContainerFormat::Unknown, file.containerFormat());
    file.invalidate();
}

void MediaFileInfoTests::testPartialParsingAndTagCreationOfMp4File()
{
    MediaFileInfo file(testFilePath("mtx-test-data/aac/he-aacv2-ps.m4a"));
    file.open(true);
    file.parseContainerFormat();
    file.parseTags();
    file.parseAttachments();
    file.close();
    try {
        file.parseTracks();
        CPPUNIT_FAIL("expected std::ios_base::failure because file has been closed");
    } catch(...) {
        catchIoFailure();
    }
    CPPUNIT_ASSERT(file.areTagsSupported());
    CPPUNIT_ASSERT(file.areTracksSupported());
    CPPUNIT_ASSERT(!file.areChaptersSupported());
    CPPUNIT_ASSERT(!file.areAttachmentsSupported());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.containerParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.tagsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.tracksParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotParsedYet, file.chaptersParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::NotSupported, file.attachmentsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(0_st, file.trackCount());
    CPPUNIT_ASSERT_EQUAL(ContainerFormat::Mp4, file.containerFormat());
    CPPUNIT_ASSERT_EQUAL(NotificationList({
                                              Notification(NotificationType::Information,
                                              "Parsing attachments is not implemented for the container format of the file.",
                                              "parsing attachments")
                                          }), file.gatherRelatedNotifications());
    CPPUNIT_ASSERT_EQUAL(NotificationType::Information, file.worstNotificationTypeIncludingRelatedObjects());

    // create/remove tag
    CPPUNIT_ASSERT_EQUAL(0_st, file.matroskaTags().size());
    CPPUNIT_ASSERT(!file.id3v1Tag());
    CPPUNIT_ASSERT_EQUAL(0_st, file.id3v2Tags().size());
    CPPUNIT_ASSERT(!file.vorbisComment());
    CPPUNIT_ASSERT(!file.mp4Tag());
    // NOTE: Maybe it should not be possible to create ID3 tags for MP4 file? It will be ignored anyways.
    CPPUNIT_ASSERT(file.createId3v1Tag());
    CPPUNIT_ASSERT(file.id3v1Tag());
    CPPUNIT_ASSERT(file.createId3v2Tag());
    CPPUNIT_ASSERT_EQUAL(1_st, file.id3v2Tags().size());
    CPPUNIT_ASSERT(!file.createVorbisComment());
    CPPUNIT_ASSERT(!file.vorbisComment());
    CPPUNIT_ASSERT(!file.removeVorbisComment());
    file.createAppropriateTags();
    CPPUNIT_ASSERT(file.mp4Tag());
}

void MediaFileInfoTests::testFullParseAndFurtherProperties()
{
    MediaFileInfo file(testFilePath("matroska_wave1/test1.mkv"));
    file.open(true);
    file.parseEverything();
    // calling parse methods twice should not do anything (and hence can not fail anymore because the file has already been closed)
    file.close();
    file.parseEverything();
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.containerParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.tagsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.tracksParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.chaptersParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ParsingStatus::Ok, file.attachmentsParsingStatus());
    CPPUNIT_ASSERT_EQUAL(ContainerFormat::Matroska, file.containerFormat());

    // general info
    CPPUNIT_ASSERT(file.container());
    CPPUNIT_ASSERT(file.areTagsSupported());
    CPPUNIT_ASSERT(file.hasAnyTag());
    CPPUNIT_ASSERT_EQUAL(1_st, file.tags().size());
    CPPUNIT_ASSERT_EQUAL(1_st, file.matroskaTags().size());
    CPPUNIT_ASSERT(!file.mp4Tag());
    CPPUNIT_ASSERT(!file.vorbisComment());
    CPPUNIT_ASSERT(file.areTracksSupported());
    CPPUNIT_ASSERT_EQUAL(2_st, file.trackCount());
    CPPUNIT_ASSERT(file.areChaptersSupported());
    CPPUNIT_ASSERT_EQUAL(0_st, file.chapters().size());
    CPPUNIT_ASSERT(file.areAttachmentsSupported());
    CPPUNIT_ASSERT_EQUAL(0_st, file.attachments().size());

    // notifications
    CPPUNIT_ASSERT(!file.hasNotifications());
    CPPUNIT_ASSERT(!file.haveRelatedObjectsNotifications());
    CPPUNIT_ASSERT_EQUAL(NotificationList(), file.gatherRelatedNotifications());
    CPPUNIT_ASSERT_EQUAL(NotificationType::None, file.worstNotificationTypeIncludingRelatedObjects());
    file.container()->addNotification(NotificationType::Warning, "warning", "test");
    CPPUNIT_ASSERT(file.haveRelatedObjectsNotifications());
    CPPUNIT_ASSERT_EQUAL(NotificationType::Warning, file.worstNotificationTypeIncludingRelatedObjects());
    file.tags().back()->addNotification(NotificationType::Critical, "error", "test");
    CPPUNIT_ASSERT_EQUAL(NotificationType::Critical, file.worstNotificationTypeIncludingRelatedObjects());
    CPPUNIT_ASSERT(file.haveRelatedObjectsNotifications());
    CPPUNIT_ASSERT_EQUAL(2_st, file.gatherRelatedNotifications().size());

    // track info / available languages
    file.tracks().back()->setLanguage("eng");
    CPPUNIT_ASSERT_EQUAL(unordered_set<string>({"eng"}), file.availableLanguages());
    CPPUNIT_ASSERT_EQUAL(unordered_set<string>({}), file.availableLanguages(MediaType::Text));
    CPPUNIT_ASSERT_EQUAL("ID: 2422994868, type: Video"s, file.tracks()[0]->label());
    CPPUNIT_ASSERT_EQUAL("ID: 3653291187, type: Audio, language: \"eng\""s, file.tracks()[1]->label());
    CPPUNIT_ASSERT_EQUAL("MS-MPEG-4-480p / MP3-2ch-eng"s, file.technicalSummary());
}
