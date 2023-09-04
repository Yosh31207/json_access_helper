# Access Helper for Boost.JSON

This helper generates functions that access inner data of [Boost.JSON](https://www.boost.org/doc/libs/release/libs/json/doc/html/index.html) value with predefined path and predefined type.

This helper is single-header library.

## Requirements

* Boost 1.82 or later
* C++ 17 or later

## Installation

Include `json_access_helper.hpp`.

## Motivation

You may want to avoid writing JSON data path in every source code because it makes the code more dependent on the applications's JSON data structure.

```C++
using boost::json::value;

void func() {
    value jv = read_json_from_file("app_config.json");
    int age = jv.at_pointer("/user/age").as_int64();
}

void another_func(const value& jv) {
    int age = jv.at_pointer("/user/age").as_int64();
}
```

In the case of above, you need to change every code that accesses `/user/age` if you changed the JSON structure of your application.

Writing JSON access functions like below may reduce the dependency on JSON structure.

```C++
using boost::json::value;

int read_age_from_json(const value& jv) {
    return jv.at_pointer("/user/age").as_int64();
}

int func() {
    // caller do not need to know the path to the data.
    int age = read_age_from_json(jv);
}
```

However, writing functions like above for every JSON data may be repetitive.

```C++
int read_age_from_json(const value& jv) {
    return jv.at_pointer("/user/age").as_int64();
}

void write_age_to_json(value& jv, int age) {
    jv.at_pointer("/user/age") = value(age);
}

std::string read_name_from_json(const value& jv) {
    return jv.at_pointer("/user/name").as_string();
}

void write_name_to_json(value& jv, const std::string& name) {
    jv.at_pointer("/user/name") = value(name);
}

// for every data you use in your application!
```

Using this helper utility, you can define JSON access function in simple way.

## Example

```C++
#include <boost/json.hpp>
#include <json_access_helper.hpp>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace json_access_helper {

// define (tag, type, path) to make helper functions
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserAge, int, "/user/age")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserName, string, "/user/name")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserSkills, vector<string>, "/user/skills")

}  // namespace json_access_helper

namespace my_app {

boost::json::value read_json_from_file(const std::string& file_path);

int main() {
    namespace jac = json_access_helper;
    namespace json = boost::json;

    json::value jv = read_json_from_file("app_config.json");

    // reads value from predefined path and converts it to predefined type.
    int age = jac::read(jv, jac::UserAge);

    // tries to read value from the predefined path and tries to convert it
    // to the predefined type.
    json::result<string> name_result = jac::try_read(jv, jac::UserName)
    if (name) {
        std::string name = *name_result;
    } else {
        std::cout << name_result.error();
    }

    // writes value to the predefined path
    jac::write(jv, jac::UserSkills, std::vector<std::string>{"C++", "C", "Python"});

    // writes value to the predefined path with creating intermediate elements.
    json::value new_jv = json::value();
    jac::emplace(jv, jac::UserSkills, std::vector<std::string>{"C++", "C", "Python"});

    return 0;
}

}  // namespace my_app
```

## How to Define the Functions

### In Case of Separating Declaration and Definition

Use `DECLARE_JSON_ACCESSOR` macro in header file and use `DEFINE_JSON_ACCESSOR` macro in cpp file.

The recommended way it to make a file to be included from declaration file and definition file as follows.

common.hpp

```C++
// Do not include any file and do not define any macro in this file!

MAKE_JSON_ACCESSOR(UserAge, int, "/user/age")
MAKE_JSON_ACCESSOR(UserName, string, "/user/name")
MAKE_JSON_ACCESSOR(UserSkills, vector<string>, "/user/skills")
```

my_json_accessor.hpp

```C++
#include <boost/json.hpp>
#include <json_access_helper.hpp>

// replace MAKE_JSON_ACCESSOR in common.hpp with DECLARE_JSON_ACCESSOR
#define MAKE_JSON_ACCESSOR DECLARE_JSON_ACCESSOR

// wrap common.hpp with the namespace where you want to define functions.
namespace json_accessor {
#include "common.hpp"
}

#undef MAKE_JSON_ACCESSOR
#undef DECLARE_JSON_ACCESSOR
#undef DEFINE_JSON_ACCESSOR
```

my_json_accessor.cpp

```C++
#include <boost/json.hpp>
#include <json_access_helper.hpp>

// replace MAKE_JSON_ACCESSOR in common.hpp with DEFINE_JSON_ACCESSOR
#define MAKE_JSON_ACCESSOR DEFINE_JSON_ACCESSOR

// wrap common.hpp with the namespace where you want to define functions.
namespace json_accessor {
#include "common.hpp"
}

#undef MAKE_JSON_ACCESSOR
#undef DECLARE_JSON_ACCESSOR
#undef DEFINE_JSON_ACCESSOR
```

### In Case of Defining Function in Header

Use `DECLARE_AND_DEFINE_JSON_ACCESSOR` macro.

my_json_accessor.hpp

```C++
#include <boost/json.hpp>
#include <json_access_helper.hpp>

// wrap the macro with the namespace where you want to define functions.
namespace json_accessor {

DECLARE_AND_DEFINE_JSON_ACCESSOR(UserAge, int, "/user/age")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserName, string, "/user/name")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserSkills, vector<string>, "/user/skills")

}

#undef MAKE_JSON_ACCESSOR
#undef DECLARE_JSON_ACCESSOR
#undef DEFINE_JSON_ACCESSOR
```

## Function Overload Resolution

This helper makes function for each path.

For example,

```C++
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserAge, int, "/user/age")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserName, string, "/user/name")
```

the code above defines two `read` functions like below.

```C++
// tag types
struct UserAgeT {};
struct UserNameT {};

// constants for each tag type
inline constexpr UserAgeT UserAge = {};
inline constexpr UserNameT UserName = {};

// generated functions
int    read(const boost::json::value& jv, UserAgeT);   // #1
string read(const boost::json::value& jv, UserNameT);  // #2
```

The function overload is resolved by the second argument.

```C++
json::value js = read_json_from_file("app_config.json");

auto age  = read(jv, UserAge);   // #1 is called
auto name = read(jv, UserName);  // #2 is called
```

## Function Reference

This helper generates the following functions.

```C++
Type read(const boost::json::value& jv, const Tag&);
boost::json::result<Type> try_read(const boost::json::value& jv, const Tag&);
bool write(boost::json::value& jv, const Tag&, const Type& value);
bool write(boost::json::value& jv, const Tag&, Type&& value);
bool write(boost::json::value& jv, const Tag&, nullptr_t value);
boost::json::value* emplace(boost::json::value& jv, const Tag&, const Type& value);
boost::json::value* emplace(boost::json::value& jv, const Tag&, Type&& value);
boost::json::value* emplace(boost::json::value& jv, const Tag&, nullptr_t value);
boost::json::value* reference(boost::json::value& jv, const Tag&);
const boost::json::value* reference(const boost::json::value& jv, const Tag&);
std::string_view path(const Tag&);
```

In the examples in this section, the followings are assumed to be predefined.

```C++
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserAge, int, "/user/age")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserName, string, "/user/name")
DECLARE_AND_DEFINE_JSON_ACCESSOR(UserSkills, vector<string>, "/user/skills")

using boost::json::value;
using std::string;
using std::vector;

value read_json_from_file(const std::string& path);
```

### read

Reads data from the specified path and converts it to the specified type. If failed, it throws exception defined in Boost.JSON.

Example:

```C++
value jv = read_json_from_file("app_config.json");
int    age  = read(json, UserAge);
string name = read(json, UserName);
```

Note:

This function uses `boost::json::value_to` for type conversion.

### try_read

Tries to read data from the specified path and tries to convert it to the specified type.

The result type of this function is `boost::json::result`.

Example:

```C++
value jv = read_json_from_file("app_config.json");

auto result = try_read(jv, UserAge);
if (result) {
    int age = *result;
} else {
    std::cout << result.error();
}
```

Note:

This function uses `boost::json::try_value_to` for type conversion.

### write

Writes value to the predefined path. This function fails if failed to access the existing `boost::json::value` in the path.

This function returns `true` if succeeded to write the value and returns `false` if failed.

You can pass the value of the predefined type or `nullptr`.

Example:

```C++
value jv = read_json_from_file("app_config.json");

std::string new_user_name = "Alice";
bool succeeded = write(jv, UserName, new_user_name);

// pass nullptr to make the value to null
succeeded = write(jv, UserName, nullptr);
```

Note:

This function uses `boost::json::value_from` for type conversion.

### emplace

Emplaces value to the path. This function may create the intermediate element to the path.

The return value is the reference of the created `boost::json::value&`.

You can pass the value of the predefined type or `nullptr`.

Example:

```C++
value jv = boost::json::value();  // make empty json

emplace(jv, UserName, std::string("Alice"));
emplace(jv, UserAge, 23);
emplace(jv, UserAge, std::vector<std::string>{"C++", "C", "Python"});

// pass nullptr to make the value to null
emplace(jv, UserAge, nullptr);
```

Note:

This function uses `boost::json::value_from` for type conversion.

### reference

Gets the pointer to the value in the predefined path. This function returns `nullptr` if filed to access.

```C++
value jv = read_json_from_file("app_config.json");

value* name_ref = reference(jv, UserName);
```

### path

Gets the predefined path.

```C++
assert(path(UserSkills) == "/user/skills");
```

## Tested Compiler

gcc 11.4.0
