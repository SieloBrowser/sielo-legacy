#include "../test.hpp"

#include <ndb/initializer.hpp>

int main(int argc, char** argv)
{
    // initialize engines
    ndb::initializer<TEST_ENGINE> init;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}