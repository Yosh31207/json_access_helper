#ifndef JSON_ACCESS_HELPER_HPP_
#define JSON_ACCESS_HELPER_HPP_

#include <string_view>

#include <boost/json.hpp>

#define DECLARE_JSON_ACCESSOR(Tag, Type, Key)                                               \
    struct Tag##T {};                                                                       \
    inline constexpr Tag##T Tag = {};                                                       \
    Type read(const boost::json::value& jv, const Tag##T&);                                 \
    boost::json::result<Type> try_read(const boost::json::value& jv, const Tag##T&);        \
    bool write(boost::json::value& jv, const Tag##T&, const Type& value);                   \
    bool write(boost::json::value& jv, const Tag##T&, Type&& value);                        \
    bool write(boost::json::value& jv, const Tag##T&, nullptr_t value);                     \
    boost::json::value& emplace(boost::json::value& jv, const Tag##T&, const Type& value);  \
    boost::json::value& emplace(boost::json::value& jv, const Tag##T&, Type&& value);       \
    boost::json::value& emplace(boost::json::value& jv, const Tag##T&, nullptr_t value);    \
    boost::json::value* reference(boost::json::value& jv, const Tag##T&);                   \
    const boost::json::value* reference(const boost::json::value& jv, const Tag##T&);       \
    std::string_view path(const Tag##T&);

#define DEFINE_JSON_ACCESSOR(Tag, Type, Key)                                                \
    struct Tag##T {};                                                                       \
    constexpr auto Tag##Path = boost::json::string_view(Key);                               \
    Type read(const boost::json::value& jv, const Tag##T&) {                                \
        return boost::json::value_to<Type>(jv.at_pointer(Tag##Path));                       \
    }                                                                                       \
    boost::json::result<Type> try_read(const boost::json::value& jv, const Tag##T&) {       \
        boost::json::error_code ec;                                                         \
        auto ref = jv.find_pointer(Tag##Path, ec);                                          \
        if (!ref) {                                                                         \
            return ec;                                                                      \
        }                                                                                   \
        return boost::json::try_value_to<Type>(*ref);                                       \
    }                                                                                       \
    bool write(boost::json::value& jv, const Tag##T&, const Type& value) {                  \
        boost::json::error_code ec;                                                         \
        auto ref = jv.find_pointer(Tag##Path, ec);                                          \
        if (!ref) {                                                                         \
            return false;                                                                   \
        }                                                                                   \
        *ref = boost::json::value_from(value);                                              \
        return true;                                                                        \
    }                                                                                       \
    bool write(boost::json::value& jv, const Tag##T&, Type&& value) {                       \
        boost::json::error_code ec;                                                         \
        auto ref = jv.find_pointer(Tag##Path, ec);                                          \
        if (!ref) {                                                                         \
            return false;                                                                   \
        }                                                                                   \
        *ref = boost::json::value_from(std::move(value));                                   \
        return true;                                                                        \
    }                                                                                       \
    bool write(boost::json::value& jv, const Tag##T&, nullptr_t) {                          \
        boost::json::error_code ec;                                                         \
        auto ref = jv.find_pointer(Tag##Path, ec);                                          \
        if (!ref) {                                                                         \
            return false;                                                                   \
        }                                                                                   \
        *ref = nullptr;                                                                     \
        return true;                                                                        \
    }                                                                                       \
    boost::json::value& emplace(boost::json::value& jv, const Tag##T&, const Type& value) { \
        return jv.set_at_pointer(Tag##Path, boost::json::value_from(value));                \
    }                                                                                       \
    boost::json::value& emplace(boost::json::value& jv, const Tag##T&, Type&& value) {      \
        return jv.set_at_pointer(Tag##Path, boost::json::value_from(std::move(value)));     \
    }                                                                                       \
    boost::json::value& emplace(boost::json::value& jv, const Tag##T&, nullptr_t) {         \
        return jv.set_at_pointer(Tag##Path, boost::json::value(nullptr));                   \
    }                                                                                       \
    boost::json::value* reference(boost::json::value& jv, const Tag##T&) {                  \
        boost::json::error_code ec;                                                         \
        return jv.find_pointer(Tag##Path, ec);                                              \
    }                                                                                       \
    const boost::json::value* reference(const boost::json::value& jv, const Tag##T&) {      \
        boost::json::error_code ec;                                                         \
        return jv.find_pointer(Tag##Path, ec);                                              \
    }                                                                                       \
    std::string_view path(const Tag##T&) {                                                  \
        return Tag##Path;                                                                   \
    }

#define DECLARE_AND_DEFINE_JSON_ACCESSOR(Tag, Type, Key)                                    \
    DEFINE_JSON_ACCESSOR(Tag, Type, Key)                                                    \
    inline constexpr Tag##T Tag = {};

#endif  // JSON_ACCESS_HELPER_HPP_
