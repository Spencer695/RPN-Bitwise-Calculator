#include <bitset>
#include <limits>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;



// test controls
#define MIN_VALUE 0
#define MAX_VALUE 100
uint8_t const table_width[] = {14, 18, 14, 18, 14, 18};

// test harness structs and params
#define VALUE_NULLPTR -999

enum command : uint16_t {
  cmd_enter = 0,
  cmd_clear,
  cmd_pop,
  cmd_top,
  cmd_left_shift,
  cmd_right_shift,
  cmd_or,
  cmd_and,
  cmd_add,
};
vector<string> command_name = {"cmd_enter", "cmd_clear",      "cmd_pop",
                               "cmd_top",   "cmd_left_shift", "cmd_right_shift",
                               "cmd_or",    "cmd_and",        "cmd_add"};
uint8_t const width = 16U;

//RPN calculator code start
shared_ptr<uint16_t> rpn_calc(command const cmd, uint16_t const value /*=0*/) {
  static vector<uint16_t> st;

  auto top_ptr = [&]() -> shared_ptr<uint16_t> {
    if (st.empty())
      return nullptr;
    return make_shared<uint16_t>(st.back());
  };

  auto need_n = [&](size_t n) -> bool { return st.size() >= n; };

  switch (cmd) {
  case cmd_enter: {
    st.push_back(static_cast<uint16_t>(value));
    return make_shared<uint16_t>(st.back());
  }

  case cmd_clear: {
    st.clear();
    return nullptr;
  }

  case cmd_pop: {
    if (st.empty())
      return nullptr;
    st.pop_back();
    return top_ptr();
  }

  case cmd_top: {
    return top_ptr();
  }

  case cmd_left_shift: {
    if (!need_n(2))
      return nullptr;
    uint16_t b = st.back();
    if (b >= width) return nullptr; // reject invalid counts without changing the stack
    st.pop_back();
    uint16_t a = st.back();
    st.pop_back();
    uint16_t amt = b;
    uint16_t r = static_cast<uint16_t>((static_cast<uint32_t>(a) << amt) & 0xFFFFu);
    st.push_back(r);
    return make_shared<uint16_t>(r);
  }

  case cmd_right_shift: {
    if (!need_n(2))
      return nullptr;
    uint16_t b = st.back();
    if (b >= width) return nullptr; // reject invalid counts without changing the stack
    st.pop_back();
    uint16_t a = st.back();
    st.pop_back();
    uint16_t amt = b;
    uint16_t r = static_cast<uint16_t>(a >> amt);
    st.push_back(r);
    return make_shared<uint16_t>(r);
  }

  case cmd_or: {
    if (!need_n(2))
      return nullptr;
    uint16_t b = st.back();
    st.pop_back();
    uint16_t a = st.back();
    st.pop_back();
    uint16_t r = static_cast<uint16_t>(a | b);
    st.push_back(r);
    return make_shared<uint16_t>(r);
  }

  case cmd_and: {
    if (!need_n(2))
      return nullptr;
    uint16_t b = st.back();
    st.pop_back();
    uint16_t a = st.back();
    st.pop_back();
    uint16_t r = static_cast<uint16_t>(a & b);
    st.push_back(r);
    return make_shared<uint16_t>(r);
  }

  case cmd_add: {
    if (!need_n(2))
      return nullptr;
    uint16_t b = st.back();
    st.pop_back();
    uint16_t a = st.back();
    st.pop_back();
    uint32_t sum = static_cast<uint32_t>(a) + static_cast<uint32_t>(b);
    if (sum > 0xFFFFu) {
      st.push_back(a);
      st.push_back(b);
      return nullptr;
    }

    uint16_t r = static_cast<uint16_t>(sum);
    st.push_back(r);
    return make_shared<uint16_t>(r);
  }
  }

  return nullptr;
}
//RPN calculator code ends

void header() {
  cout << left << setw(table_width[0]) << setfill(' ') << "pass/fail";
  cout << left << setw(table_width[1]) << setfill(' ') << "command";
  cout << left << setw(table_width[2]) << setfill(' ') << "value";
  cout << left << setw(table_width[3]) << setfill(' ') << "value bits";
  cout << left << setw(table_width[4]) << setfill(' ') << "result";
  cout << left << setw(table_width[5]) << setfill(' ') << "result bits" << endl;

  cout << left << setw(table_width[0]) << setfill(' ') << "--------";
  cout << left << setw(table_width[1]) << setfill(' ') << "--------";
  cout << left << setw(table_width[2]) << setfill(' ') << "--------";
  cout << left << setw(table_width[3]) << setfill(' ') << "--------";
  cout << left << setw(table_width[4]) << setfill(' ') << "--------";
  cout << left << setw(table_width[5]) << setfill(' ') << "--------" << endl;
}

void print_row(bool const test_success, command const cmd, int32_t const value,
               shared_ptr<uint16_t> top_of_stack) {
  // print results
  string const pass_fail = test_success ? "PASS" : "FAIL";
  cout << left << setw(table_width[0]) << setfill(' ') << pass_fail;
  cout << left << setw(table_width[1]) << setfill(' ') << command_name[cmd];
  if (value == VALUE_NULLPTR) {
    cout << left << setw(table_width[2]) << setfill(' ') << " ";
    cout << left << setw(table_width[3]) << setfill(' ') << " ";
  } else {
    cout << left << setw(table_width[2]) << setfill(' ') << value;
    cout << left << setw(table_width[3]) << setfill(' ')
         << bitset<width>(value);
  }

  if (top_of_stack) {
    cout << left << setw(table_width[4]) << setfill(' ') << *top_of_stack;
    cout << left << setw(table_width[5]) << setfill(' ')
         << bitset<width>(*top_of_stack) << endl;
  } else {
    cout << left << setw(table_width[4]) << setfill(' ') << " ";
    cout << left << setw(table_width[5]) << setfill(' ') << " " << endl;
  }
}

vector<string> split(string const &s, string const &delimiter) {
  vector<string> tokens;
  size_t pos = 0;
  size_t start = 0;
  string token;
  while (pos != string::npos) {
    pos = s.find(",", start);
    token = s.substr(start, pos - start);
    tokens.push_back(token);
    start = pos + delimiter.length();
  }

  return tokens;
}

void init_command_map(unordered_map<string, command> &command_map) {
  for (size_t i = 0; i < command_name.size(); i++) {
    string const cmd = command_name[i];
    command_map[cmd] = static_cast<command>(i);
  }
}

bool parse_csv_line(string const line,
                    unordered_map<string, command> command_map,
                    command &input_cmd, uint16_t &input_value,
                    int32_t &answer_value) {
  try {
    vector<string> tokens = split(line, ",");
    if (tokens.size() != 3) {
      cout << "ERROR: Expected command,value,expected columns" << endl;
      return false;
    }
    if (command_map.count(tokens[0]) != 1) {
      cout << "ERROR: Invalid command cmd=" << tokens[0] << endl;
      return false;
    }
    input_cmd = command_map.at(tokens[0]);
    size_t used_value = 0, used_answer = 0;
    const long long parsed_value = stoll(tokens[1], &used_value);
    const long long parsed_answer = stoll(tokens[2], &used_answer);
    if (used_value != tokens[1].size() || used_answer != tokens[2].size() ||
        parsed_value < 0 || parsed_value > numeric_limits<uint16_t>::max() ||
        (parsed_answer != VALUE_NULLPTR &&
         (parsed_answer < 0 || parsed_answer > numeric_limits<uint16_t>::max()))) {
      cout << "ERROR: Invalid or out-of-range numeric field" << endl;
      return false;
    }
    input_value = static_cast<uint16_t>(parsed_value);
    answer_value = static_cast<int32_t>(parsed_answer);
  } catch (exception const &e) {
    cout << "ERROR: Unable to parse input csv file, line=" << line << endl;
    cout << "ERROR: exception e=" << e.what() << endl;
    return false;
  }

  return true;
}

bool test(string const &input_path) {
  // open input file
  ifstream input_file(input_path);
  if (!input_file.is_open()) {
    cout << "ERROR: Unable to find and open the file " << input_path
         << endl;
    cout << "       Pass a valid CSV path as the first command-line argument"
         << endl;
    return false;
  }

  // init command map
  unordered_map<string, command> command_map;
  init_command_map(command_map);

  // print header
  header();

  bool all_test_pass = true;
  uint16_t pass = 0;
  string line;
  size_t row = 0;
  while (getline(input_file, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    // cout << "line " << row << ":" << line << endl;
    if (row > 0) {
      // parse csv line
      command input_cmd;
      uint16_t input_value;
      int32_t input_answer;
      bool parse_success = parse_csv_line(line, command_map, input_cmd,
                                          input_value, input_answer);
      if (!parse_success) {
        return false;
      }

      // set answer value
      shared_ptr<uint16_t> answer;
      if (input_answer == VALUE_NULLPTR) {
        answer = nullptr;
      } else {
        answer = make_shared<uint16_t>(input_answer);
      }

      // do the command
      shared_ptr<uint16_t> result = rpn_calc(input_cmd, input_value);

      // test the results
      bool test_success = false;
      bool both_null = answer == nullptr && result == nullptr;
      bool both_same_value = answer && result && (*answer == *result);
      if (both_null || both_same_value) {
        pass += 1;
        test_success = true;
      }

      // update all tests
      all_test_pass = all_test_pass && test_success;

      // print row
      print_row(test_success, input_cmd, input_value, result);
    }
    row += 1;
  }

  // close input file
  input_file.close();

  // summarize results
  cout << "-------------------------------------------" << endl;
  if (all_test_pass) {
    cout << "SUCCESS ";
  } else {
    cout << "FAILURE ";
  }
  const size_t num_tests = row > 0 ? row - 1 : 0;
  cout << pass << "/" << num_tests << " passed" << endl;
  cout << "-------------------------------------------" << endl;

  return all_test_pass && row > 1;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    cerr << "Usage: rpn [test-cases.csv]" << endl;
    return 1;
  }
  const string input_path = argc == 2 ? argv[1] : "tests/rpn-cases.csv";
  if (!test(input_path)) {
    return -1;
  }
  return 0;
}
