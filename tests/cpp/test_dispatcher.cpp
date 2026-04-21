#include <catch2/catch_test_macros.hpp>
#include "dispatcher.h"

TEST_CASE("Dispatcher format selection", "[dispatcher]") {
    std::string input = "# Hello";

    SECTION("LaTeX format selection") {
        REQUIRE_NOTHROW(convert_markdown(input, "latex"));
    }

    SECTION("Typst format selection") {
        REQUIRE_NOTHROW(convert_markdown(input, "typst"));
    }

    SECTION("Invalid format throws exception") {
        REQUIRE_THROWS_AS(convert_markdown(input, "docx"), std::invalid_argument);
    }

}

TEST_CASE("Invalid markdown content throws exception", "[dispatcher]") {
    std::string invalid = "<html>not markdown</html>";

    REQUIRE_THROWS(convert_markdown(invalid, "latex"));
}
