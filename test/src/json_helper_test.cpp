#include "json_access_helper.hpp"

#include <string>
#include <vector>

#include <boost/json.hpp>
#include <gtest/gtest.h>

namespace json = boost::json;
using std::string;
using std::vector;

#define MAKE_JSON_ACCESSOR DECLARE_AND_DEFINE_JSON_ACCESSOR

namespace json_accessor_test_impl {

MAKE_JSON_ACCESSOR(UserName,  string,         "/user/name")
MAKE_JSON_ACCESSOR(UserAge,   int,            "/user/age")
MAKE_JSON_ACCESSOR(UserLangs, vector<string>, "/user/languages")

}  // namespace json_accessor_test_impl

namespace {

const auto template_json = json::value {
    {"user", {
        {"name", "Alice"},
        {"age",  23},
        {"languages", json::array{"C++", "Python", "Haskell", "Rust"}},
    }},
};

using json_accessor_test_impl::reference;
using json_accessor_test_impl::read;
using json_accessor_test_impl::try_read;
using json_accessor_test_impl::write;
using json_accessor_test_impl::emplace;
using json_accessor_test_impl::path;

namespace tag = json_accessor_test_impl;

TEST(JsonAccessor, Read) {
    auto json_1 = template_json;
    auto json_2 = json::value();

    EXPECT_EQ(read(json_1, tag::UserName), "Alice");
    EXPECT_EQ(read(json_1, tag::UserAge),   23);
    EXPECT_EQ(read(json_1, tag::UserLangs), (vector<string>{"C++", "Python", "Haskell", "Rust"}));

    // throws exception if any error occurs
    EXPECT_ANY_THROW(read(json_2, tag::UserName));
    EXPECT_ANY_THROW(read(json_2, tag::UserAge));
    EXPECT_ANY_THROW(read(json_2, tag::UserLangs));
}

TEST(JsonAccessor, TryRead) {
    auto json_1 = template_json;
    auto json_2 = json::value();

    auto name_1 = try_read(json_1, tag::UserName);
    auto age_1  = try_read(json_1, tag::UserAge);
    auto lang_1 = try_read(json_1, tag::UserLangs);

    EXPECT_TRUE(name_1);
    EXPECT_TRUE(age_1);
    EXPECT_TRUE(lang_1);
    EXPECT_EQ(*name_1, "Alice");
    EXPECT_EQ(*age_1,  23);
    EXPECT_EQ(*lang_1, (vector<string>{"C++", "Python", "Haskell", "Rust"}));

    auto name_2 = try_read(json_2, tag::UserName);
    auto age_2  = try_read(json_2, tag::UserAge);
    auto lang_2 = try_read(json_2, tag::UserLangs);

    EXPECT_FALSE(name_2);
    EXPECT_FALSE(age_2);
    EXPECT_FALSE(lang_2);
}

TEST(JsonAccessor, Write) {
    auto json_1 = template_json;
    auto json_2 = json::value();

    bool name_1_result = write(json_1, tag::UserName,  "SuperAlice");
    bool age_1_result  = write(json_1, tag::UserAge,   100);
    bool lang_1_result = write(json_1, tag::UserLangs, vector<string>{"GoLang", "Elixir", "TypeScript"});

    EXPECT_TRUE(name_1_result);
    EXPECT_TRUE(age_1_result);
    EXPECT_TRUE(lang_1_result);
    EXPECT_EQ(json_1.at("user").at("name"),      json::value("SuperAlice"));
    EXPECT_EQ(json_1.at("user").at("age"),       json::value(100));
    EXPECT_EQ(json_1.at("user").at("languages"), (json::array{"GoLang", "Elixir", "TypeScript"}));

    // write null value
    bool name_1_result_2 = write(json_1, tag::UserName,  nullptr);
    bool age_1_result_2  = write(json_1, tag::UserAge,   nullptr);
    bool lang_1_result_2 = write(json_1, tag::UserLangs, nullptr);

    EXPECT_TRUE(name_1_result_2);
    EXPECT_TRUE(age_1_result_2);
    EXPECT_TRUE(lang_1_result_2);
    EXPECT_TRUE(json_1.at("user").at("name").is_null());
    EXPECT_TRUE(json_1.at("user").at("age").is_null());
    EXPECT_TRUE(json_1.at("user").at("languages").is_null());

    // returns false if the key does not exist.
    auto name_2_result = write(json_2, tag::UserName,  "SuperAlice");
    auto age_2_result  = write(json_2, tag::UserAge,   100);
    auto lang_2_result = write(json_2, tag::UserLangs, vector<string>{"GoLang", "Elixir", "TypeScript"});

    EXPECT_FALSE(name_2_result);
    EXPECT_FALSE(age_2_result);
    EXPECT_FALSE(lang_2_result);

    auto name_2_result_2 = write(json_2, tag::UserName,  nullptr);
    auto age_2_result_2  = write(json_2, tag::UserAge,   nullptr);
    auto lang_2_result_2 = write(json_2, tag::UserLangs, nullptr);

    EXPECT_FALSE(name_2_result_2);
    EXPECT_FALSE(age_2_result_2);
    EXPECT_FALSE(lang_2_result_2);
}

TEST(JsonAccessor, Emplace) {
    auto json_1 = template_json;
    auto json_2 = json::value();
    auto json_3 = json::value();

    // same result as write if the key already exists.
    emplace(json_1, tag::UserName,  "SuperAlice");
    emplace(json_1, tag::UserAge,   100);
    emplace(json_1, tag::UserLangs, vector<string>{"GoLang", "Elixir", "TypeScript"});
    EXPECT_EQ(json_1.at("user").at("name"),      json::value("SuperAlice"));
    EXPECT_EQ(json_1.at("user").at("age"),       json::value(100));
    EXPECT_EQ(json_1.at("user").at("languages"), (json::array{"GoLang", "Elixir", "TypeScript"}));

    // writes null value
    emplace(json_1, tag::UserName,  nullptr);
    emplace(json_1, tag::UserAge,   nullptr);
    emplace(json_1, tag::UserLangs, nullptr);
    EXPECT_TRUE(json_1.at("user").at("name").is_null());
    EXPECT_TRUE(json_1.at("user").at("age").is_null());
    EXPECT_TRUE(json_1.at("user").at("languages").is_null());

    // creates intermediate element if the corresponding element does not exist.
    emplace(json_2, tag::UserName,  "SuperAlice");
    emplace(json_2, tag::UserAge,   100);
    emplace(json_2, tag::UserLangs, vector<string>{"GoLang", "Elixir", "TypeScript"});
    EXPECT_EQ(json_2.at("user").at("name"),      json::value("SuperAlice"));
    EXPECT_EQ(json_2.at("user").at("age"),       json::value(100));
    EXPECT_EQ(json_2.at("user").at("languages"), (json::array{"GoLang", "Elixir", "TypeScript"}));

    // writes null value
    emplace(json_3, tag::UserName,  nullptr);
    emplace(json_3, tag::UserAge,   nullptr);
    emplace(json_3, tag::UserLangs, nullptr);
    EXPECT_TRUE(json_3.at("user").at("name").is_null());
    EXPECT_TRUE(json_3.at("user").at("age").is_null());
    EXPECT_TRUE(json_3.at("user").at("languages").is_null());
}

TEST(JsonAccessor, Reference) {
    auto json_1 = template_json;
    auto json_2 = json::value();

    auto name_ref_1 = reference(json_1, tag::UserName);
    auto age_ref_1  = reference(json_1, tag::UserAge);
    auto lang_ref_1 = reference(json_1, tag::UserLangs);
    EXPECT_EQ(name_ref_1, &json_1.at("user").at("name"));
    EXPECT_EQ(age_ref_1,  &json_1.at("user").at("age"));
    EXPECT_EQ(lang_ref_1, &json_1.at("user").at("languages"));

    *name_ref_1 = json::value(10);
    EXPECT_EQ(json_1.at("user").at("name"), json::value(10));

    auto name_ref_2 = reference(json_2, tag::UserName);
    auto age_ref_2  = reference(json_2, tag::UserAge);
    auto lang_ref_2 = reference(json_2, tag::UserLangs);
    EXPECT_EQ(name_ref_2, nullptr);
    EXPECT_EQ(age_ref_2,  nullptr);
    EXPECT_EQ(lang_ref_2, nullptr);
}

TEST(JsonAccessor, Path) {
    EXPECT_EQ(path(tag::UserName),  "/user/name");
    EXPECT_EQ(path(tag::UserAge),   "/user/age");
    EXPECT_EQ(path(tag::UserLangs), "/user/languages");
}

}  // namespace
