This directory provides some tests for the sample-compiler as an example of how to script some tests for your compiler.

See `runTests.sh` for details

The folder names here correspond to the stages of your compiler. Each folder contains an `input` and `expected` directory. The test script executes your compiler with each input and asserts that it matches the expected output.