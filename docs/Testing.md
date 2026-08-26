# Testing Suite for DreamOs64

__NOTE__ This feature is still in early development, so there is a lot of codoe repetition, and it can definetely be optimized, but since the focus is the kernel, these change will come slowly, with very low priority. 

Although, i don't love writing unit tests, i decided to give them a try, mostly as learning curve, on how to write something from scratch. 

So I started writing some "unit" tests, without using any existing framework. Not for everything of course, I tested whatever was doable using regular C compiler on a regular *nix system.

## How they are organized

There is no real structure on the tests, but the idea is to have a test file for every feature/lib we want to tests.

## How does they work

To the core the tests are simple C programs, that call the functions and chek the results.

### Testing with `pretty_assert_stat`

When testing, although we can use the `assert` functon from `assert.h`, I came out with a macro, cover some specific cases of my needs. 

The macro is defined in `test_stat.h`, it takes the following parameters, and the purpose is to make it reusable for all use cases.

* expected value
* returned value
* comparator (i.e. `==`, `<=`, etc) 
* pointer to the stats structure (that will be explained later) (struct type is: `test_stat_t`)
* Message about the test
* boolean value to tell the test suite if it should abort on first error or continue. `true` if we want it to continue on error (and update the structs passed), `false`, if we want to abort on the first error (in this case the stats struct is never updated).

Let's make an example: let's say we have a function `gimme_five(unsigned int number)`, that always return five, this is how the code for testing it looks like (the initialization part is skipped): 

```c
// We assume that we have the pointer to test_stats_t defind as stats
unsigned int returned_value = gimme_five(10);
pretty_assert_stat(5, returned_value, ==, stats, "Testing function gimme_five", true);
returned_value = gimme_five(5);
pretty_assert_stat(5, returned_value, ==, stats, "Testing function gimme_five", true);
```
The output will be:
```c
(test_gimme_five) Testing function gimme_five expected_value: 5 returned value:  5
(test_gimme_five) Testing function gimme_five expected_value: 5 returned value:  5
```

## Tests runner 

One of the features I was curious to built, was a runner for the tests, and collect stats, but since I was running single programs, there wasn't a trivial way so I started to implement my custom solution. 

The idea is pretty trivial: the runner will read the list of tests from a text files to be executed, spawn them using fork exec, and the tests will be executed as normal (but the tests will be aware that they have to send back the data), and once the data is collected the pipe will be used to send back the data. 

The runner will execute tests sequentially, waiting for the previous to finish, before starting the next. 

At the end of execution all results are collected in global stats. 

## How to add new tests

Tests are simple C program, so if we just want to add a new test to an existing one, we just write the code we need, like the example in the _Testing with `pretty_assert_stat`_ section.

If we want to test a new module, is better to create a new file, following the same structure of one of the existing tests. 

How to organize tests within files is totally arbitrary, anyway the best approach is to create a separate `function` for each component you want to test.

What we need for every test module is: 

* `main` function
* `prepare_tests()` although not mandatory is to keep all tests consistent. 
* `test_function`. 
* We need to include `test_stats.h`, `unistd.h`, `stdlib.h`
* We need the 3 followiing global variables: 
```c
bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests = 0; // This must be always initialized at 0
```

Inside the `main` we do the following steps: 

* Check if a pipe descriptor has been passed, if yes, this means that the tests has been launched through `tests_runner`, in this case we need to convert the parameter passed to main into int  `pipe_fd`:
```c
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
```
* Inside prepare tests we need to call the function:  `void test_init(unsigned int no_modules, test_runner_t **tests)` Where `no_modules` is the number of functions that will be called (each functions is considered a module), and `tests` is the pointer to te `test_runner_t` structure.

* The `test_runner_t` struct contains the following information: 
```c
typedef struct {
    test_stats_t stats;
    void (*handler)(unsigned int);
} test_runner_t;
```
* We need to create the test functions, each function must have the followiing signature: `void test_functionname(usngied int id);`
* Then we need to add each function to the tests struct, using the helpfer function: `void add_test(unsigned int module_idx, char *module_title, void (*handler)(unsigned int), test_runner_t *tests);` where `module_idx` is a numberical identifier for the module being tested, `module_title` is a string that can be used to give it a title, `handler` is the pointer to the function to run (pass just the function name) and `tests` is the pointer to the structure we want to add the module.
* After each `add_test` we need to increase the variable `number_of_tests` (or we need to set its value after the tests are added to the number of times we have called `add_test` function).
* Now we can esaily call all the functions from a while loop: 

```c
for (int j=0; j < number_of_tests; j++) {
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    test[j].handler(j);
    print_stats(tests[0].stats);
}
```
* Finally we need to send back this data, in case a pipe has been passed: 

```c
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
```

When passing the data to the runner we always need to send first the `number_of_tests` value, and then call send_stats for each test that has been executed (and this should match number_of_tests. 

* The compiled test binary needs to be added to the `tests/tests` file, with the format: `./filename.o`
* Update the makefile file with the new test added.

### Simple example

Let's imagine we want to write a simple series of tests for the function `unsigned int gimme_five(unsiged int number)` a function that always return five no matter what the value is. Here the example code for the test file:

```c
#include <stdio.h>
#include <stdlib.h>
#include <gimme_five.h>
#include <test_common.h>
#include <test_stats.h>
#include <unistd.h>

void test_gimme_five(unsigned int id);
void prepare_tests();

bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests = 0;

int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests();
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    tests[0].handler(0);
    print_stats(tests[0].stats);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
    return 0;
}

void prepare_tests() {
    printf("Testing GIMME FIVE functions  -\n");
    printf("===============================\n\n");
    test_init(1, &tests);
    add_test(1, "Testing Gimme five", test_gimme_five, tests);
    number_of_tests++;
}

void test_gimme_five(unsigned int id){
    unsigned int result = gimme_five(5);
    pretty_assert_stat(5, result, ==, tests[id].stats, "Testing gimme_five with 5", has_pipe);
    result = gimme_five(50);
    pretty_assert_stat(0, result, ==, tests[id].stats, "Testing gimme_five with 5", has_pipe);
}

```

## The Tests runner

Tests runner is a program that iterate through all the lines in the `tests/test` file and will launch every single test in it. At the end will print the result of each single tests, and the global result of all the tests passed and failed. If at least one test failed, it returns `-1` causing the `make tests` call ta fail. 
