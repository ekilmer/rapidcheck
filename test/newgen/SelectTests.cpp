#include <catch.hpp>
#include <rapidcheck-catch.h>

#include "rapidcheck/newgen/Select.h"

#include "util/Meta.h"
#include "util/Util.h"
#include "util/TypeListMacros.h"
#include "util/GenUtils.h"

using namespace rc;
using namespace rc::test;
using namespace rc::detail;

struct ElementOfTests
{
    template<typename T>
    static void exec()
    {
        // TODO should be in standard library of generators!
        static const auto nonEmpty = gen::suchThat<T>([] (const T &x) {
            return !x.empty();
        });

        templatedProp<T>(
            "all generated elements are elements of the container",
            [](const GenParams &params) {
                T elements = *nonEmpty;
                const auto gen = newgen::elementOf(elements);
                const auto value = gen(params.random, params.size).value();
                RC_ASSERT(std::find(begin(elements), end(elements), value) !=
                          end(elements));
            });

        templatedProp<T>(
            "all elements are eventually generated",
            [](const GenParams &params) {
                T elements = *nonEmpty;
                const auto gen = newgen::elementOf(elements);
                std::set<typename T::value_type> all(begin(elements),
                                                     end(elements));
                std::set<typename T::value_type> generated;
                Random r(params.random);
                while (all != generated)
                    generated.insert(gen(r.split(), params.size).value());
                RC_SUCCEED("All values generated");
            });

        TEMPLATED_SECTION(T, "throws GenerationFailure on empty container") {
            T container;
            REQUIRE_THROWS_AS(newgen::elementOf(container)(Random(0), 0),
                              GenerationFailure);
        }
    }
};

TEST_CASE("newgen::elementOf") {
    meta::forEachType<ElementOfTests,
                      std::vector<char>,
                      std::string>();
}

TEST_CASE("newgen::element") {
    prop("all generated elements are arguments",
         [](const GenParams &params) {
             const auto gen = newgen::element(std::string("foo"),
                                              std::string("bar"),
                                              std::string("baz"));
             const auto x = gen(params.random, params.size).value();
             RC_ASSERT((x == "foo") || (x == "bar") || (x == "baz"));
         });

    prop("all generated elements are arguments",
         [](const GenParams &params) {
             const auto gen = newgen::element(std::string("foo"),
                                              std::string("bar"),
                                              std::string("baz"));
             std::set<std::string> all{"foo", "bar", "baz"};
             std::set<std::string> generated;
             Random r(params.random);
             while (all != generated)
                 generated.insert(gen(r.split(), params.size).value());
             RC_SUCCEED("All values generated");
         });
}
