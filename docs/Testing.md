# Testing Suite for DreamOs64

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

If we want to test a new module. We need to create 

## The Tests runner

