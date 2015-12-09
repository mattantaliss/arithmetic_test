# arithmetic_test
C++ program for creating LaTeX source for various arithmetic tests

This is a modified and compiled version of a few programs I wrote years ago for some friends' kids. This program generates LaTeX source that the user can then process separately to create a given packet of arithmetic tests. One packet of arithmetic tests contains (i) one or more score tracker pages (depending on the number of tests in the packet) to record the time taken on a given test and the number or percent of problems correct on that test, (ii) a solutions page for study/review, and (iii) a specified number of arithmetic tests which are randomly shuffled single-digit problems.

The user is able to dictate the number of tests included in the packet (default is 60), the output file name (default is "tests"; ".tex" is automatically added), and the test type. The test type can be addition ('a'), multiplication ('m'), subtraction ('s'), or division ('d'); the default is 'a'.

Each arithmetic test contains all valid combinations of two digits (i.e., [0-9] X [0-9]). This is straight-forward for addition and multiplication; all 100 combinations are included on each test. For subtraction, repeated problems are included to have a total of 100 problems on each test while ensuring non-negative answers. For division, the product of a given combination is the dividend, and 0 is not allowed as a divisor; each division test has only 90 problems.

The same thought process follows for the solutions page. Notably, for subtraction solutions, instead of showing only a lower- or upper-triangular matrix of problems and solutions, repeated problems are included. For division, only the 90 valid problems are included.

Example files:<br />
`tests.tex` - output produced by the program when default values are used<br />
`tests.pdf` - output produced after processing tests.tex
