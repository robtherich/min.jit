/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"     // required unit test header
#include "min.jit.testgen.cpp"    // need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

SCENARIO("object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    GIVEN("An instance of our object") {

        test_wrapper<min_jit_testgen> an_instance;
        min_jit_testgen&              my_object = an_instance;

        // check that default attr values are correct
        REQUIRE((my_object.greeting == symbol("hello world")));

        // now proceed to testing various sequences of events

		// not used in this example but leaving here for reference
        /*WHEN("a 'bang' is received") {
            my_object.bang();
            THEN("our greeting is produced at the outlet") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE((output.size() == 1));
                REQUIRE((output[0].size() == 1));
                REQUIRE((output[0][0] == symbol("hello world")));
            }
        }*/
    }
}
