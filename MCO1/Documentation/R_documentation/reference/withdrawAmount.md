# Withdraw Amount

Prompts the user to input a numeric/double value that will be subtracted
from their current held balance.

## Usage

``` r
withdrawAmount(accountName, balance, globalCurrency)
```

## Arguments

- accountName:

  The string value that represents the current user's account name from
  main.

- balance:

  A numeric value that represents the user's currently held balance from
  main.

- globalCurrency:

  A vector that contains values representing each global currency.

## Value

The new balance value after being evaluated by the function.
