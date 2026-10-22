# Validate a Choice I/O

Validates a user's input value, and only allows them to escape the
function if their input is within the defined bounds of the function.
Otherwise, it re-prompts the user for a valid input.

## Usage

``` r
getValidChoice(lb, ub)
```

## Arguments

- lb:

  An integer value representing an inclusive lower bound that
  getConsoleInput checks for error detection.

- ub:

  An integer value representing an inclusive upper bound that
  getConsoleInput checks for error detection.

## Value

The integer value that corresponds to the user's valid input.
