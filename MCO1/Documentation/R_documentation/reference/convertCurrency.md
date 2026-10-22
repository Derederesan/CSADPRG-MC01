# Convert Currency

Converts a source currency amount into a target currency amount by
multiplying it by a factor of the rates of the source divided by the
rates of the target. No validation, divison by zero may occur.

## Usage

``` r
convertCurrency(amount, source, target, globalCurrency)
```

## Arguments

- amount:

  A double value that can represent any amount of currency recorded in
  data.

- source:

  Either an integer, or name that is passed as a source index to specify
  the exact global currency that corresponds to param amount.

- target:

  Either an integer, or name that is passed as a target index to specify
  the exact global currency in the vector, used for the conversion.

- globalCurrency:

  A vector that contains values representing each global currency.

## Value

The target currency amount.
