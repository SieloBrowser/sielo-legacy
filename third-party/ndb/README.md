# Neuroshok Database Interface

| Master | Unix ![](https://travis-ci.org/ads00/ndb.svg?branch=master)|
|--------|------------------------------------------------------------|
| Dev    | Unix ![](https://travis-ci.org/ads00/ndb.svg?branch=dev)   | 

**ndb** is a generic interface to connect to any databases. 
Querys are written in pure C++ and can be executed on different database engines just by changing one parameter.
Database model is defined and accessible compile time. 

# Features
- **Header only** interface
- Full **C++** and **generic** querys
- Support **any** type of **databases**
- **Easy** to add **new engines**
- **Compile-time** generation for **SQL expressions**
- **Customizable** **types** conversion 
- **Field access by C++** structure
- **Database generation** with macros

## Current support
- Expressions based on **SQL** and **BSON**
- **Sqlite**, **MySQL** and **MongoDB**.

# How to use
Choose the branch to clone (master or dev)

```sh
ads@NK:/home/ads$ git clone --recurse-submodules -b [branch] https://github.com/ads00/ndb.git
```
## Build (optional)
Create build directory
```sh
ads@NK:/home/ads/ndb$ mkdir build && cd build
```

### Bundled engines
Choose engines with option `-DNDB_ENGINE_[name]=ON` (ex: `-DNDB_ENGINE_SQLITE=ON`)

### Options
`-DNDB_BUILD_TEST=ON`
`-DNDB_BUILD_EXAMPLE=ON`

```sh
ads@NK:/home/ads/ndb/build$ cmake -DNDB_ENGINE_SQLITE=ON [options] -DCMAKE_BUILD_TYPE=Release ..
ads@NK:/home/ads/ndb/build$ make
(optional) ads@NK:/home/ads/ndb/build$ make test
```

## Integration
### CMake
```cmake 
add_subdirectory(${PATH_TO_NDB}/ndb ${THIRD_PARTY_ROOT}/ndb/cmake-build)
target_link_libraries(my_target lib_ndb)
```
> Remember, it's importent to enable options like engines **before** adding the subdirectory. Indeed, you need to include libraries of the engine you use.

### Manual
Add paths to ndb headers and your customs engine builds

# Overview
## Database
```cpp
ndb_table(movie,
          ndb_field_id,
          ndb_field(name, std::string, ndb::size<255>)
)
ndb_table(music,
          ndb_field_id,
          ndb_field(image, std::string, ndb::size<255>)
)
ndb_model(collection, movie, music)

ndb_project(my_project,
            ndb_database(libray, collection, ndb::sqlite),
            ndb_database(mongo_library, collection, ndb::mongo)
)
```

## Querys
```cpp
ndb::query<dbs::zeta>() << ( movie.id, movie.image ); // get
ndb::query<dbs::zeta>() << ( movie.id == a && movie.name == b ); // get by condition
ndb::query<dbs::zeta>() >> (( movie.name = "updated") << ( movie.id == 3 )); // update by condition
ndb::query<dbs::zeta>() + ( movie.id = 3, movie.name = "test" ); // add
ndb::query<dbs::zeta>() - ( movie.id == 3 ); // del
```
## Example
An example with a *libray* database using a *collection* model and a *movie* table
```cpp
#include "my_database.h"

// aliases
namespace dbs
{
    using libray = ndb::databases::my_project::libray_;
    using mongo_library = ndb::databases::my_project::mongo_library_;
}

int main()
{
    // alias
    const auto& movie = ndb::models::collection.movie;
    
    // initialize sqlite and mongo
    ndb::initializer<ndb::sqlite, ndb::mongo> init;
    
    // connect to database library (sqlite is used)
    ndb::connect<dbs::library>();
    
    // connect to database mongo_library (mongo is used)
    ndb::connect<dbs::mongo_library>();  

    // add a movie with specified movie.name and movie.duration (table is deduced compile time)
    ndb::query<dbs::library>() + (movie.name = "Interstellar", movie.duration = 2.49_h) );
    
    // get all fields from movie where duration is less than 2H30
    for (auto& line : ndb::query<dbs::library>() << (movie.duration <= 3.30_h))
    {
        std::cout << "movie.name : " << line[movie.name] << std::endl;
        std::cout << "movie.duration : " << line[movie.duration] << std::endl; 
    }

    return 0;
}
```

# Documentation
[Example](https://github.com/ads00/ndb/tree/dev/example)


# Contribution
ads00

Jonathan Poelen ([@jonathanpoelen](https://github.com/jonathanpoelen))
