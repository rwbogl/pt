This is a collection of programs to find good probable primality tests. The
tests are higher-order generalizations of Fermat's primality test, similar to
the [Perrin primality
test](https://mathworld.wolfram.com/PerrinPseudoprime.html). For more
information, see the brilliant paper [Lots and Lots of Perrin-Type Primality
Tests and Their Pseudo-Primes](https://arxiv.org/abs/2307.16069).

The programs can be downloaded and compiled as follows.

```
$ git clone git@github.com:rwbogl/pt.git
$ cd pt
$ make
```

You may need to install a GNU GMP development package. In Ubuntu this is done
by executing the command `apt install libgmp-dev`.

## ptf

`ptf` finds good probable primality tests. It does a brute force search over
a specified range of tests and reports those which have first pseudoprimes
larger than a preset limit. Here is an example.

```
$ ./ptf 1000 0 0 0
2023-09-07T13:19:46-0400
ptf: checking 1000 recurrences starting from [0, 0, 0]
MAX_COEFF is 9
SEARCH_LIMIT is 100000000
REPORT_LIMIT is 100000
checking recurrence 0
report: 271441 [0, 1, 1]
hit order barrier at [10, 0, 0]
2023-09-07T13:19:47-0400
```

Try `ptf --help` for more detailed instructions.

## ptc

`ptc` computes the pseudoprimes of a specific primality test up to a preset
limit. Here is an example.

```
$ ./ptc 1 1600000 1 1 0 1 0 0 4
2023-09-07T13:24:10-0400
ptc: checking terms 1 to 1600001
checking term 100000
checking term 200000
checking term 300000
checking term 400000
checking term 500000
checking term 600000
checking term 700000
checking term 800000
checking term 900000
checking term 1000000
checking term 1100000
checking term 1200000
checking term 1300000
checking term 1400000
checking term 1500000
counterexample: 1531398
checking term 1600000
2023-09-07T13:24:19-0400
```

Try `ptc --help` for more detailed instructions.
