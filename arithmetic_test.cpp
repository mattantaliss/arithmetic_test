// Create LaTeX source code for single-digit arithmetic tests. Output is stored
// in a '.tex' file which the user processes separately.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <utility>
#include <math.h>
#include <unistd.h>

// Prototypes
template <size_t rows, size_t cols>
void MakeTestPage(std::ofstream& output_file, int num_digits,
                  std::pair<int, int> (&numbers_table)[rows][cols],
                  std::string operation, bool include_solutions);

void UsageInformation(const char* program_name);

// Main
int main(int argc, char* argv[]) {
  // Initialize default values and an output filestream
  int num_tests = 60;
  std::string output_file = "tests.tex";
  std::string test_type = "a";
  std::ofstream file_out;

  // Process arguments
  // http://www.gnu.org/software/libc/manual/html_node/Getopt.html
  int curr_arg;
  while ((curr_arg = getopt(argc, argv, "hn:o:t:")) != -1) {
    switch (curr_arg) {
    case 'h':
      // Help
      // Print the usage message and exit
      UsageInformation(argv[0]);

      return 0;
    case 'n':
      // Set the number of tests to create
      {
        // Verify num_tests is an integer between 1 and 999; if not, print
        // an error message, print the usage message, and exit
        std::istringstream input(optarg);
        if (!(input >> num_tests && input.eof() && num_tests > 0 &&
            num_tests < 1000)) {
          std::cerr << "Error: num_tests (" << optarg << ") is not a positive ";
          std::cerr << "integer between 1 and 999.";
          std::cerr << std::endl;
          UsageInformation(argv[0]);

          return 1;
        }
      }

      break;
    case 'o':
      // Set the output file name (validity check is done later, when attempting
      // to start writing to the given file)
      {
        output_file = optarg;
        output_file += ".tex";
      }

      break;
    case 't':
      // Set the type of tests to create; if the test_type argument is invalid,
      // print an error message, print the usage message, and exit
      {
        switch (optarg[0]) {
        case 'a':
        case 'm':
        case 's':
        case 'd':
          if (strlen(optarg) > 1) {
            // Invalid argument
            std::cerr << "Error: test_type (" << optarg << ") is not one of ";
            std::cerr << "'a', 'm', 's', or 'd'." << std::endl;
            UsageInformation(argv[0]);

            return 1;
          } else {
            test_type = optarg;
          }

          break;
        default:
          {
            // Invalid argument
            std::cerr << "Error: test_type (" << optarg << ") is not one of ";
            std::cerr << "'a', 'm', 's', or 'd'." << std::endl;
            UsageInformation(argv[0]);

            return 1;
          }
        }
      }

      break;
    case '?':
      // Invalid option or missing argument; print an error message, print the
      // usage message, and exit
      switch (optopt) {
      case 'n':
      case 'o':
      case 't':
        std::cerr << "Error: option -" << optopt << " requires an argument.";
        std::cerr << std::endl;
        break;
      default:
        std::cerr << "Error: unknown option (" << optopt << ")." << std::endl;
      }

      UsageInformation(argv[0]);
      return 1;
    default:
      // Unknown error
      // Print an error message, print the usage message, and exit
      std::cerr << "Unknown error" << std::endl;
      UsageInformation(argv[0]);
      return 1;
    }
  }
  // Check for any left-over non-option arguments; if any exist, print an error
  // message, print the usage message, and exit
  if (optind < argc) {
    std::cerr << "Error: unused arguments (" << argv[optind];
    for (int i = optind + 1; i < argc; i++) {
      // Add remaining unused arguments to list
      std::cerr << ", " << argv[i];
    }
    std::cerr << ")" << std::endl;
    UsageInformation(argv[0]);
    return 1;
  }

  // Open the output file stream
  file_out.open(output_file);

  // Set up a table of pairs of digits
  // * For subtraction, non-negative differences will be enforced by swapping
  //   i and j when i < j. The effect here is that instead of only having 55 of
  //   the 100 possible problems included, repeated problems will exist.
  // * For division, a couple things need to change:
  //   (a) The dividend will be i * j and the divisor will be i.
  //   (b) Division by zero will be avoided by excluding the i == 0 cases when
  //       shuffling the table (-1 placeholders will be used in those cases).
  const int kNumDigits = 10;
  std::pair<int, int> table[kNumDigits][kNumDigits];
  for (int i = 0; i < kNumDigits; i++) {
    for (int j = 0; j < kNumDigits; j++) {
      if (test_type == "a" || test_type == "m") {
        table[i][j] = std::make_pair(i, j);
      } else if (test_type == "s") {
        if (i < j) {
          table[i][j] = std::make_pair(j, i);
        } else {
          table[i][j] = std::make_pair(i, j);
        }
      } else if (test_type == "d") {
        if (i == 0) {
          table[i][j] = std::make_pair(-1, -1);
        } else {
          table[i][j] = std::make_pair(i * j, i);
        }
      } else {
        // Unknown test_type (test_type is validated at the start, so this
        // should never be encountered)
        std::cerr << "Error: test_type (" << test_type << ") is not one of ";
        std::cerr << "'a', 'm', 's', or 'd'." << std::endl;
      }
    }
  }

  // Produce the tests and store them in the output file
  for (int n = 0; n < num_tests; n++) {
    // Check if the output file is open; if not, print an error message, print
    // the usage message, and exit
    if (!file_out.is_open()) {
      std::cerr << "Error: unable to open input file" << output_file << ".";
      std::cerr << std::endl;
      UsageInformation(argv[0]);

      return 1;
    } else {
      // Create the LaTeX source code
      if (n == 0) {
        // Preamble
        file_out << "\\documentclass[12pt, letterpaper]{article}\n";
        file_out << "\\usepackage[margin=1in]{geometry}\n";
        file_out << "\\usepackage{multicol}\n";
        file_out << "\\usepackage{setspace}\n";
        file_out << "\\usepackage{fancyhdr}\n";
        file_out << "\\pagestyle{fancy}\n";
        file_out << "\\renewcommand{\\headrulewidth}{0pt}\n";
        file_out << "\\fancyhf{}\n";

        // Document begin
        // First page(s) is(are) a scoring tracker, second page is a solutions
        // page, and all following pages are tests
        file_out << "\\begin{document}\n";
        file_out << "\\begin{multicols}{2}\n";
        file_out << "\\setlength{\\columnseprule}{0.5pt}\n";
        file_out << "{\\setstretch{1.5}\n";
        file_out << "\\noindent\n";

        // Score-tracking page:
        for (int m = 1; m < num_tests + 1; m++) {
          // Number of digits needed to dislay the number of tests included
          int num_digits_needed = floor(log10(num_tests)) + 1;

          // Number of digits in the current value of m
          int num_digits_curr_m = floor(log10(m)) + 1;

          // Buffer size = 11 (length of "\\phantom{}") + number of digits
          // needed to display the number of tests included + 1 (null char)
          int tot_buffer_size = 11 + num_digits_needed + 1;
          char buffer[tot_buffer_size];

          // Output score tracking lines for each test that will be generated
          if (num_digits_needed - num_digits_curr_m > 0) {
            file_out << "\\phantom{";
            snprintf(buffer, tot_buffer_size, "%0*d",
                     num_digits_needed - num_digits_curr_m, 0);
            file_out << buffer << "}";
          }
          snprintf(buffer, tot_buffer_size, "%d", m);
          file_out << buffer << ". Time: \\underline{\\hspace{6em}}";
          file_out << "\\quad Correct: \\underline{\\hspace{3em}}";
          if (m == num_tests) {
            file_out << "\\par\n";
          } else {
            file_out << "\\\\\n";
          }
        }

        file_out << "}\n"; // Closing \setstretch
        file_out << "\\end{multicols}\n";
        file_out << "\\newpage\n";

        // Solutions page
        MakeTestPage(file_out, kNumDigits, table, test_type, true);

        // Now that the preface pages are done, set up page numbering to apply
        // to the test pages
        file_out << "\\setcounter{page}{1}\n";
        file_out << "\\lfoot{\\framebox{\\makebox[\\totalheight]{\\thepage}}}\n";
      }

      // Regular test pages are generated here:
      // Randomly shuffle the table of digit pairs
      std::srand(time(NULL));
      if (test_type == "d") {
        std::random_shuffle(&table[0][0] + kNumDigits,
                            &table[0][0] + kNumDigits * kNumDigits);
      } else {
        std::random_shuffle(&table[0][0],
                            &table[0][0] + kNumDigits * kNumDigits);
      }

      // Create the test page
      MakeTestPage(file_out, kNumDigits, table, test_type, false);
    }
  }

  // Document end
  file_out << "\\end{document}";

  // Close output file
  file_out.close();

  return 0;
}

// Output usage information
void UsageInformation (const char* program_name) {
  std::cout << std::endl;
  std::cout << "usage: " << program_name << " [-h] [-n num_tests] ";
  std::cout << "[-o output_file] [-t test_type]\n\n";
  std::cout << "  -h              Print this message.\n";
  std::cout << "  -n num_tests    The number of tests to create.\n";
  std::cout << "                  num_tests must be an integer between 1 and ";
  std::cout << "999.\n";
  std::cout << "                  A scoring page fits 60 records.\n";
  std::cout << "                  Default value: 60\n";
  std::cout << "  -o output_file  The file in which to store the output.\n";
  std::cout << "                  \'.tex\' will automatically be added.\n";
  std::cout << "                  Default value: tests\n";
  std::cout << "  -t test_type    The type of test to create.\n";
  std::cout << "                  test_type is a single character indicating ";
  std::cout << "the type of\n";
  std::cout << "                  arithmetic operator to use in the tests:\n";
  std::cout << "                    'a' - Addition\n";
  std::cout << "                    'm' - Multiplication\n";
  std::cout << "                    's' - Subtraction\n";
  std::cout << "                    'd' - Division\n";
  std::cout << "                  Default value: a\n";
}

// Generate a test page, possibly with solutions
template <size_t rows, size_t cols>
void MakeTestPage(std::ofstream& output_file, int num_digits,
                  std::pair<int, int> (&numbers_table)[rows][cols],
                  std::string operation, bool include_solutions) {
  // When creating a division test, skip over the row of (-1, -1) pairs (which
  // are used to indicate division by zero)
  int start_row = 0;
  if (operation == "d") {
    start_row = 2;
  }

  // Output LaTeX source for the arithmetic problems. The problems are
  // laid out in a num_digits x num_digits table, but each problem actually
  // takes two rows (augend/multiplier/minued/dividend in one row,
  // addend/multiplicand/subtrahend/divisor in the next), and there is an empty
  // column between each problem for a grand total of 19 columns (= 10 problem
  // columns + 9 empty columns) in the table.
  output_file << "\\begin{tabular}{rrrrrrrrrrrrrrrrrrr}\n";

  for (int curr_row = start_row; curr_row < 2 * num_digits; curr_row++) {
    for (int curr_col = 0; curr_col < num_digits; curr_col++) {
      if (curr_row % 2 == 0) {
        // Augend/Multiplier/Minued/Dividend row
        output_file << numbers_table[curr_row / 2][curr_col].first;
      } else {
        // Addend/Multiplicand/Subtrahend/Divisor row
        // Output the operator
        if (operation == "a") {
          // Addition
          output_file << "$+$ ";
        } else if (operation == "m") {
          // Multiplication
          output_file << "$\\times$ ";
        } else if (operation == "s") {
          // Subtraction
          output_file << "$-$ ";
        } else if (operation == "d") {
          // Division
          output_file << "$\\div$ ";
        } else {
          // Unknown op (test_type is validated at the start, so this should
          // never be encountered)
          std::cerr << "Error: operation (" << operation << ") is not one of";
          std::cerr << " 'a', 'm', 's', or 'd'." << std::endl;
        }

        // Output the addend/multiplicand/subtrahend/divisor
        output_file << numbers_table[curr_row / 2][curr_col].second;
      }
      if (curr_col == num_digits - 1) {
        // At the end of the row; add new row
        output_file << "\\\\" << std::endl;
      } else {
        // Not at the end of the row; add column separators
        output_file << " & & ";
      }
    }

    // If the addend/multiplicand/subtrahend/divisor row was just added, then
    // add the horizontal separator lines and the solutions or a blank space
    // row (depending on incl_sols)
    if (curr_row % 2 != 0) {
      // Add lines separating addends/multiplicands/subtrahends/divisors and
      // sums/products/differences/quotients
      for (int col = 0; col < 10; col++) {
        output_file << "\\cline{" << 2 * col + 1 << "-" << 2 * col + 1 << "} ";
      }

      // Add solutions or empty row
      // (blank space for writing in the sums/products/differences/quotients)
      if (include_solutions) {
        for (int col = 0; col < 10; col++) {
          // Sums/Products/Differences/Quotients row
          if (operation == "a") {
            // Addition
            output_file << numbers_table[curr_row / 2][col].first +
              numbers_table[curr_row / 2][col].second;
          } else if (operation == "m") {
            // Multiplication
            output_file << numbers_table[curr_row / 2][col].first *
              numbers_table[curr_row / 2][col].second;
          } else if (operation == "s") {
            // Subtraction
            output_file << numbers_table[curr_row / 2][col].first -
              numbers_table[curr_row / 2][col].second;
          } else if (operation == "d") {
            // Division
            output_file << numbers_table[curr_row / 2][col].first /
            numbers_table[curr_row / 2][col].second;
          } else {
            // Unknown op (test_type is validated at the start, so this should
            // never be encountered)
            std::cerr << "Error: operation (" << operation << ") is not one of";
            std::cerr << " 'a', 'm', 's', or 'd'." << std::endl;
          }

          if (col == num_digits - 1) {
            // At the end of the row; add solution and new row
            output_file << "\\\\ \\\\";
          } else {
            // Not at the end of the row; add column separators
            output_file << " & & ";
          }
        }
      } else {
        output_file << "\\\\ \\\\";
      }

      // End the current line of LaTeX source
      output_file << std::endl;
    }
  }

  // End of current table and page
  output_file << "\\end{tabular}\n";
  output_file << "\\newpage\n";
}