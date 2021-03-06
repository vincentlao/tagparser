#ifndef MP4TAG_H
#define MP4TAG_H

#include "./mp4tagfield.h"

#include "../fieldbasedtag.h"

namespace Media
{

class Mp4Atom;
class Mp4Tag;

struct TAG_PARSER_EXPORT Mp4ExtendedFieldId
{
    Mp4ExtendedFieldId(const char *mean = nullptr, const char *name = nullptr, bool updateOnly = false);
    Mp4ExtendedFieldId(KnownField field);

    operator bool() const;
    bool matches(const Mp4TagField &field) const;

    /// \brief mean parameter, usually Mp4TagExtendedMeanIds::iTunes
    const char *mean;
    /// \brief name parameter
    const char *name;
    /// \brief Whether only existing fields should be updated but *no* new extended field should be created
    bool updateOnly;
};

/*!
 * \brief Constructs a new instance with the specified parameter.
 */
inline Mp4ExtendedFieldId::Mp4ExtendedFieldId(const char *mean, const char *name, bool updateOnly) :
    mean(mean),
    name(name),
    updateOnly(updateOnly)
{}

/*!
 * \brief Returns whether valid parameter are assigned.
 */
inline Mp4ExtendedFieldId::operator bool() const
{
    return mean && name;
}

/*!
 * \brief Returns whether the current parameter match the specified \a field.
 */
inline bool Mp4ExtendedFieldId::matches(const Mp4TagField &field) const
{
    return field.mean() == mean && field.name() == name;
}

class TAG_PARSER_EXPORT Mp4TagMaker
{
    friend class Mp4Tag;

public:
    void make(std::ostream &stream);
    const Mp4Tag &tag() const;
    uint64 requiredSize() const;

private:
    Mp4TagMaker(Mp4Tag &tag);

    Mp4Tag &m_tag;
    std::vector<Mp4TagFieldMaker> m_maker;
    uint64 m_metaSize;
    uint64 m_ilstSize;
    bool m_omitPreDefinedGenre;
};

/*!
 * \brief Returns the associated tag.
 */
inline const Mp4Tag &Mp4TagMaker::tag() const
{
    return m_tag;
}

/*!
 * \brief Returns the number of bytes which will be written when making the tag.
 */
inline uint64 Mp4TagMaker::requiredSize() const
{
    return m_metaSize;
}

class TAG_PARSER_EXPORT Mp4Tag : public FieldMapBasedTag<Mp4TagField>
{
public:
    Mp4Tag();

    static constexpr TagType tagType = TagType::Mp4Tag;
    TagType type() const;
    const char *typeName() const;
    TagTextEncoding proposedTextEncoding() const;
    bool canEncodingBeUsed(TagTextEncoding encoding) const;

    uint32 fieldId(KnownField field) const;
    KnownField knownField(const uint32 &id) const;
    bool supportsField(KnownField field) const;
    using FieldMapBasedTag<Mp4TagField>::value;
    const TagValue &value(KnownField value) const;
    using FieldMapBasedTag<Mp4TagField>::values;
    std::vector<const TagValue *> values(KnownField field) const;
#ifdef LEGACY_API
    const TagValue &value(const std::string mean, const std::string name) const;
#endif
    const TagValue &value(const std::string &mean, const std::string &name) const;
    const TagValue &value(const char *mean, const char *name) const;
    using FieldMapBasedTag<Mp4TagField>::setValue;
    bool setValue(KnownField field, const TagValue &value);
    using FieldMapBasedTag<Mp4TagField>::setValues;
    bool setValues(KnownField field, const std::vector<TagValue> &values);
#ifdef LEGACY_API
    bool setValue(const std::string mean, const std::string name, const TagValue &value);
#endif
    bool setValue(const std::string &mean, const std::string &name, const TagValue &value);
    bool setValue(const char *mean, const char *name, const TagValue &value);
    using FieldMapBasedTag<Mp4TagField>::hasField;
    bool hasField(KnownField value) const;

    void parse(Mp4Atom &metaAtom);
    Mp4TagMaker prepareMaking();
    void make(std::ostream &stream);
};

/*!
 * \brief Constructs a new tag.
 */
inline Mp4Tag::Mp4Tag()
{}

inline TagType Mp4Tag::type() const
{
    return TagType::Mp4Tag;
}

inline const char *Mp4Tag::typeName() const
{
    return "MP4/iTunes tag";
}

inline TagTextEncoding Mp4Tag::proposedTextEncoding() const
{
    return TagTextEncoding::Utf8;
}

inline bool Mp4Tag::supportsField(KnownField field) const
{
    switch(field) {
    case KnownField::EncoderSettings:
        return true;
    default:
        return FieldMapBasedTag<Mp4TagField>::supportsField(field);
    }
}

/*!
 * \brief Returns the value of the field with the specified \a mean and \a name attributes.
 */
inline const TagValue &Mp4Tag::value(const std::string &mean, const std::string &name) const
{
    return value(mean.data(), name.data());
}

/*!
 * \brief Assigns the given \a value to the field with the specified \a mean and \a name attributes.
 */
inline bool Mp4Tag::setValue(const std::string &mean, const std::string &name, const TagValue &value)
{
    return setValue(mean.data(), name.data(), value);
}

}

#endif // MP4TAG_H
