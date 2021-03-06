#ifndef MEDIA_TAGTARGET_H
#define MEDIA_TAGTARGET_H

#include "./global.h"

#include <c++utilities/conversion/types.h>

#include <string>
#include <vector>
#include <functional>

namespace Media {

/*!
 * \brief The TagTargetLevel enum specifies tag target levels.
 */
enum class TagTargetLevel : unsigned char
{
    Unspecified,
    Shot,
    Subtrack,
    Track,
    Part,
    Album,
    Edition,
    Collection
};

TAG_PARSER_EXPORT const char *tagTargetLevelName(TagTargetLevel tagTargetLevel);

class TAG_PARSER_EXPORT TagTarget
{
public:
    typedef uint64 IdType;
    typedef std::vector<IdType> IdContainerType;

    TagTarget(uint64 level = 0, IdContainerType tracks = IdContainerType(), IdContainerType chapters = IdContainerType(), IdContainerType editions = IdContainerType(), IdContainerType attachments = IdContainerType());

    uint64 level() const;
    void setLevel(uint64 level);
    const std::string &levelName() const;
    void setLevelName(const std::string &levelName);
    const IdContainerType &tracks() const;
    IdContainerType &tracks();
    const IdContainerType &chapters() const;
    IdContainerType &chapters();
    const IdContainerType &editions() const;
    IdContainerType &editions();
    const IdContainerType &attachments() const;
    IdContainerType &attachments();
    bool isEmpty() const;
    void clear();
    std::string toString(const std::function<TagTargetLevel(uint64)> &tagTargetMapping) const;
    std::string toString(TagTargetLevel tagTargetLevel) const;
    bool operator ==(const TagTarget &other) const;

private:
    uint64 m_level;
    std::string m_levelName;
    IdContainerType m_tracks;
    IdContainerType m_chapters;
    IdContainerType m_editions;
    IdContainerType m_attachments;
};

/*!
 * \brief Constructs a new TagTarget with the specified \a level, \a track, \a chapter,
 *        \a edition and \a attachment.
 */
inline TagTarget::TagTarget(uint64 level, IdContainerType tracks, IdContainerType chapters, IdContainerType editions, IdContainerType attachments) :
    m_level(level),
    m_tracks(tracks),
    m_chapters(chapters),
    m_editions(editions),
    m_attachments(attachments)
{}

/*!
 * \brief Returns the level.
 */
inline uint64 TagTarget::level() const
{
    return m_level ? m_level : 50;
}

/*!
 * \brief Sets the level.
 */
inline void TagTarget::setLevel(uint64 level)
{
    m_level = level;
}

/*!
 * \brief Returns the level name.
 */
inline const std::string &TagTarget::levelName() const
{
    return m_levelName;
}

/*!
 * \brief Sets the level name.
 */
inline void TagTarget::setLevelName(const std::string &levelName)
{
    m_levelName = levelName;
}

/*!
 * \brief Returns the tracks.
 */
inline const TagTarget::IdContainerType &TagTarget::tracks() const
{
    return m_tracks;
}

/*!
 * \brief Returns the tracks.
 */
inline TagTarget::IdContainerType &TagTarget::tracks()
{
    return m_tracks;
}

/*!
 * \brief Returns the chapters.
 */
inline const TagTarget::IdContainerType &TagTarget::chapters() const
{
    return m_chapters;
}

/*!
 * \brief Returns the chapters.
 */
inline TagTarget::IdContainerType &TagTarget::chapters()
{
    return m_chapters;
}

/*!
 * \brief Returns the editions.
 */
inline const TagTarget::IdContainerType &TagTarget::editions() const
{
    return m_editions;
}

/*!
 * \brief Returns the editions.
 */
inline TagTarget::IdContainerType &TagTarget::editions()
{
    return m_editions;
}

/*!
 * \brief Returns the attachments.
 */
inline const TagTarget::IdContainerType &TagTarget::attachments() const
{
    return m_attachments;
}

/*!
 * \brief Returns the attachments.
 */
inline TagTarget::IdContainerType &TagTarget::attachments()
{
    return m_attachments;
}

/*!
 * \brief Returns an indication whether the target is empty.
 */
inline bool TagTarget::isEmpty() const
{
    return m_level == 0
            && m_levelName.empty()
            && m_tracks.empty()
            && m_chapters.empty()
            && m_editions.empty()
            && m_attachments.empty();
}

/*!
 * \brief Clears the TagTarget.
 */
inline void TagTarget::clear()
{
    m_level = 0;
    m_levelName.clear();
    m_tracks.clear();
    m_chapters.clear();
    m_editions.clear();
    m_attachments.clear();
}

/*!
 * \brief Returns whether the tag targets are equal.
 * \remarks Targets where only the level name differs are considered equal though.
 */
inline bool TagTarget::operator ==(const TagTarget &other) const
{
    return level() == other.level()
            && m_tracks == other.m_tracks
            && m_chapters == other.m_chapters
            && m_editions == other.m_editions
            && m_attachments == other.m_attachments;
}

/*!
 * \brief Returns the string representation of the current instance.
 * \remarks Uses the specified \a tagTargetMapping function to map the assigned level()
 *          to a TagTargetLevel if no levelName() is assigned.
 */
inline std::string TagTarget::toString(const std::function<TagTargetLevel(uint64)> &tagTargetMapping) const
{
    return toString(tagTargetMapping ? tagTargetMapping(this->level()) : TagTargetLevel::Unspecified);
}

}

#endif // MEDIA_TAGTARGET_H
