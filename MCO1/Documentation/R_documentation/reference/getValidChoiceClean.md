# Validate a Choice I/O Without Prints

Validates a user's input value, and only allows them to escape the
function if their input is within the defined bounds of the function.
Otherwise, it re-prompts the user for a valid input. Does not print any
prompt text to guide the user. This function is called to use readLines
present in getConsoleInput for specific text formatting.

## Usage

``` r
getValidChoiceClean(lb, ub, promptText = "")
```

## Arguments

- lb:

  An integer value representing an inclusive lower bound that
  getConsoleInput checks for error detection.

- ub:

  An integer value representing an inclusive upper bound that
  getConsoleInput checks for error detection.

- promptText:

  An empty string that is passed into getConsoleInput for text
  formatting purposes.

## Value

The integer value that corresponds to the user's input.
