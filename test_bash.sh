#!/bin/bash

# Shell Parser Test Script
# Tests various parsing scenarios against GNU Bash behavior
# Usage: ./test_parser.sh [your_minishell_executable]

MINISHELL=${1:-"./minishell"}
TEST_COUNT=0
PASS_COUNT=0
FAIL_COUNT=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test result tracking
declare -a FAILED_TESTS=()

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}   Shell Parser Test Suite${NC}"
    echo -e "${BLUE}================================${NC}"
    echo "Testing against: $MINISHELL"
    echo "Reference: GNU Bash"
    echo
}

print_test_category() {
    echo -e "${YELLOW}--- $1 ---${NC}"
}

run_test() {
    local test_name="$1"
    local input="$2"
    local description="$3"
    
    ((TEST_COUNT++))
    
    echo -n "Test $TEST_COUNT: $test_name - "
    
    # Get bash output (we'll mainly check if it parses without syntax error)
    bash_exit_code=0
    bash -n -c "$input" 2>/dev/null || bash_exit_code=$?
    
    # Get minishell parsing result
    minishell_exit_code=0
    if [ -x "$MINISHELL" ]; then
        echo "$input" | timeout 2s "$MINISHELL" >/dev/null 2>/dev/null || minishell_exit_code=$?
    else
        echo -e "${RED}SKIP (minishell not found)${NC}"
        return
    fi
    
    # Compare results (focusing on whether parsing succeeds or fails)
    if [[ ($bash_exit_code -eq 0 && $minishell_exit_code -eq 0) || 
          ($bash_exit_code -ne 0 && $minishell_exit_code -ne 0) ]]; then
        echo -e "${GREEN}PASS${NC}"
        ((PASS_COUNT++))
    else
        echo -e "${RED}FAIL${NC}"
        echo "  Input: '$input'"
        echo "  Description: $description"
        echo "  Bash exit: $bash_exit_code, Minishell exit: $minishell_exit_code"
        FAILED_TESTS+=("$test_name: $input")
        ((FAIL_COUNT++))
    fi
}

# Basic Command Tests
print_test_category "Basic Commands"
run_test "simple_command" "echo hello" "Simple command with argument"
run_test "command_only" "ls" "Command without arguments"
run_test "multiple_args" "echo hello world 123" "Command with multiple arguments"
run_test "empty_command" "" "Empty input"
run_test "whitespace_only" "   " "Whitespace only"

# Quote Tests
print_test_category "Quote Parsing"
run_test "single_quotes" "echo 'hello world'" "Single quoted string"
run_test "double_quotes" "echo \"hello world\"" "Double quoted string"
run_test "mixed_quotes" "echo 'hello' \"world\"" "Mixed quote types"
run_test "nested_quotes" "echo \"It's working\"" "Single quote inside double quotes"
run_test "nested_quotes_reverse" "echo 'He said \"hello\"'" "Double quote inside single quotes"
run_test "unclosed_single" "echo 'unclosed" "Unclosed single quote"
run_test "unclosed_double" "echo \"unclosed" "Unclosed double quote"
run_test "empty_quotes" "echo ''" "Empty single quotes"
run_test "empty_double_quotes" "echo \"\"" "Empty double quotes"

# Variable Expansion Tests
print_test_category "Variable Expansion"
run_test "env_var" "echo \$HOME" "Environment variable expansion"
run_test "undefined_var" "echo \$UNDEFINED_VAR" "Undefined variable"
run_test "exit_status" "echo \$?" "Exit status variable"
run_test "var_in_quotes" "echo \"\$HOME\"" "Variable in double quotes"
run_test "var_in_single_quotes" "echo '\$HOME'" "Variable in single quotes (no expansion)"
run_test "multiple_vars" "echo \$HOME \$USER \$PWD" "Multiple variables"
run_test "var_concatenation" "echo \$HOME/bin" "Variable concatenation"
run_test "dollar_only" "echo \$" "Dollar sign without variable name"
run_test "invalid_var_name" "echo \$123invalid" "Invalid variable name"

# Redirection Tests
print_test_category "Redirection"
run_test "output_redirect" "echo hello > /tmp/test.txt" "Output redirection"
run_test "append_redirect" "echo hello >> /tmp/test.txt" "Append redirection"
run_test "input_redirect" "cat < /etc/passwd" "Input redirection"
run_test "heredoc" "cat << EOF\nhello\nEOF" "Here document"
run_test "multiple_redirects" "echo hello > file1 > file2" "Multiple output redirections"
run_test "redirect_no_file" "echo hello >" "Redirection without filename"

# Pipe Tests
print_test_category "Pipes"
run_test "simple_pipe" "echo hello | cat" "Simple pipe"
run_test "multiple_pipes" "echo hello | cat | wc -l" "Multiple pipes"
run_test "pipe_with_redirect" "echo hello | cat > /tmp/test.txt" "Pipe with redirection"
run_test "empty_pipe" "echo hello |" "Pipe without right command"
run_test "pipe_start" "| echo hello" "Pipe at start"

# Special Characters Tests
print_test_category "Special Characters"
run_test "semicolon" "echo hello; echo world" "Semicolon separator"
run_test "ampersand" "echo hello &" "Background process"
run_test "logical_and" "echo hello && echo world" "Logical AND"
run_test "logical_or" "echo hello || echo world" "Logical OR"
run_test "parentheses" "(echo hello)" "Parentheses grouping"
run_test "backslash_escape" "echo hello\\ world" "Backslash escape"
run_test "wildcard" "echo *.txt" "Wildcard pattern"

# Complex Parsing Tests
print_test_category "Complex Parsing"
run_test "complex_quotes" "echo \"Hello 'world' from \$USER\"" "Complex quote nesting"
run_test "var_in_complex" "echo \"\$HOME is my home directory\"" "Variable in complex string"
run_test "multiple_redirects_complex" "echo hello > file1 2>&1" "Complex redirection"
run_test "pipe_and_redirect" "ls -la | grep test > results.txt" "Pipe and redirection"
run_test "heredoc_with_vars" "cat << EOF\nHello \$USER\nEOF" "Heredoc with variables"
run_test "command_substitution" "echo \$(date)" "Command substitution"
run_test "arithmetic_expansion" "echo \$((2 + 3))" "Arithmetic expansion"

# Edge Cases
print_test_category "Edge Cases"
run_test "very_long_command" "echo $(printf 'a%.0s' {1..1000})" "Very long command"
run_test "many_spaces" "echo     hello     world" "Multiple spaces"
run_test "tabs_and_spaces" "echo	hello world" "Tabs and spaces"
run_test "special_chars" "echo !@#$%^&*()" "Special characters"
run_test "unicode" "echo 'héllo wørld'" "Unicode characters"
run_test "newline_in_quotes" "echo 'hello\nworld'" "Newline in quotes"

# Syntax Error Tests
print_test_category "Syntax Errors"
run_test "unmatched_paren" "echo (hello" "Unmatched parenthesis"
run_test "invalid_redirect" "echo hello >>" "Invalid redirection"
run_test "multiple_pipes" "echo hello || | cat" "Invalid pipe combination"
run_test "invalid_var" "echo \$" "Invalid variable syntax"
run_test "bad_heredoc" "cat << " "Incomplete heredoc"

# Performance Tests
print_test_category "Performance"
run_test "many_args" "echo $(printf 'arg%d ' {1..100})" "Many arguments"
run_test "deep_quotes" "echo \"$(printf '\"%.0s' {1..50})hello$(printf '\"%.0s' {1..50})\"" "Deeply nested quotes"

print_summary() {
    echo
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}        Test Summary${NC}"
    echo -e "${BLUE}================================${NC}"
    echo "Total tests: $TEST_COUNT"
    echo -e "Passed: ${GREEN}$PASS_COUNT${NC}"
    echo -e "Failed: ${RED}$FAIL_COUNT${NC}"
    
    if [ $FAIL_COUNT -gt 0 ]; then
        echo
        echo -e "${RED}Failed tests:${NC}"
        for test in "${FAILED_TESTS[@]}"; do
            echo -e "  ${RED}✗${NC} $test"
        done
    fi
    
    echo
    if [ $FAIL_COUNT -eq 0 ]; then
        echo -e "${GREEN}All tests passed! 🎉${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed. Check your parser implementation.${NC}"
        exit 1
    fi
}

# Additional utility functions
create_test_files() {
    echo "Creating test files..."
    mkdir -p test_env
    echo "test content" > test_env/test.txt
    echo "line1" > test_env/multiline.txt
    echo "line2" >> test_env/multiline.txt
    export TEST_VAR="test_value"
    export COMPLEX_VAR="hello world"
}

cleanup_test_files() {
    echo "Cleaning up test files..."
    rm -rf test_env
    rm -f /tmp/test.txt file1 file2 results.txt 2>/dev/null
}

# Interactive mode for debugging specific tests
interactive_mode() {
    echo -e "${BLUE}Interactive Mode${NC}"
    echo "Enter commands to test (type 'quit' to exit):"
    
    while true; do
        echo -n "test> "
        read -r input
        
        if [ "$input" = "quit" ]; then
            break
        fi
        
        if [ -n "$input" ]; then
            echo "Testing: $input"
            echo -n "Bash: "
            if bash -n -c "$input" 2>/dev/null; then
                echo -e "${GREEN}OK${NC}"
            else
                echo -e "${RED}Syntax Error${NC}"
            fi
            
            echo -n "Your shell: "
            if echo "$input" | "$MINISHELL" >/dev/null 2>&1; then
                echo -e "${GREEN}OK${NC}"
            else
                echo -e "${RED}Error${NC}"
            fi
            echo
        fi
    done
}

# Main execution
main() {
    print_header
    
    # Check if minishell exists
    if [ ! -x "$MINISHELL" ]; then
        echo -e "${RED}Error: Minishell executable not found at $MINISHELL${NC}"
        echo "Usage: $0 [path_to_minishell]"
        exit 1
    fi
    
    # Set up test environment
    create_test_files
    
    # Run all tests
    print_test_category "Basic Commands"
    run_test "simple_command" "echo hello" "Simple command with argument"
    run_test "command_only" "ls" "Command without arguments"
    run_test "multiple_args" "echo hello world 123" "Command with multiple arguments"
    run_test "empty_command" "" "Empty input"
    run_test "whitespace_only" "   " "Whitespace only"

    print_test_category "Quote Parsing"
    run_test "single_quotes" "echo 'hello world'" "Single quoted string"
    run_test "double_quotes" "echo \"hello world\"" "Double quoted string"
    run_test "mixed_quotes" "echo 'hello' \"world\"" "Mixed quote types"
    run_test "nested_quotes" "echo \"It's working\"" "Single quote inside double quotes"
    run_test "nested_quotes_reverse" "echo 'He said \"hello\"'" "Double quote inside single quotes"
    run_test "unclosed_single" "echo 'unclosed" "Unclosed single quote"
    run_test "unclosed_double" "echo \"unclosed" "Unclosed double quote"
    run_test "empty_quotes" "echo ''" "Empty single quotes"
    run_test "empty_double_quotes" "echo \"\"" "Empty double quotes"

    print_test_category "Variable Expansion"
    run_test "env_var" "echo \$HOME" "Environment variable expansion"
    run_test "undefined_var" "echo \$UNDEFINED_VAR" "Undefined variable"
    run_test "exit_status" "echo \$?" "Exit status variable"
    run_test "var_in_quotes" "echo \"\$HOME\"" "Variable in double quotes"
    run_test "var_in_single_quotes" "echo '\$HOME'" "Variable in single quotes (no expansion)"
    run_test "multiple_vars" "echo \$HOME \$USER \$PWD" "Multiple variables"
    run_test "var_concatenation" "echo \$HOME/bin" "Variable concatenation"
    run_test "dollar_only" "echo \$" "Dollar sign without variable name"
    run_test "invalid_var_name" "echo \$123invalid" "Invalid variable name"

    print_test_category "Redirection"
    run_test "output_redirect" "echo hello > /tmp/test.txt" "Output redirection"
    run_test "append_redirect" "echo hello >> /tmp/test.txt" "Append redirection"
    run_test "input_redirect" "cat < /etc/passwd" "Input redirection"
    run_test "multiple_redirects" "echo hello > file1 > file2" "Multiple output redirections"
    run_test "redirect_no_file" "echo hello >" "Redirection without filename"

    print_test_category "Pipes"
    run_test "simple_pipe" "echo hello | cat" "Simple pipe"
    run_test "multiple_pipes" "echo hello | cat | wc -l" "Multiple pipes"
    run_test "pipe_with_redirect" "echo hello | cat > /tmp/test.txt" "Pipe with redirection"
    run_test "empty_pipe" "echo hello |" "Pipe without right command"
    run_test "pipe_start" "| echo hello" "Pipe at start"

    print_test_category "Special Characters"
    run_test "semicolon" "echo hello; echo world" "Semicolon separator"
    run_test "ampersand" "echo hello &" "Background process"
    run_test "logical_and" "echo hello && echo world" "Logical AND"
    run_test "logical_or" "echo hello || echo world" "Logical OR"
    run_test "parentheses" "(echo hello)" "Parentheses grouping"
    run_test "backslash_escape" "echo hello\\ world" "Backslash escape"
    run_test "wildcard" "echo *.txt" "Wildcard pattern"

    print_test_category "Complex Parsing"
    run_test "complex_quotes" "echo \"Hello 'world' from \$USER\"" "Complex quote nesting"
    run_test "var_in_complex" "echo \"\$HOME is my home directory\"" "Variable in complex string"
    run_test "multiple_redirects_complex" "echo hello > file1 2>&1" "Complex redirection"
    run_test "pipe_and_redirect" "ls -la | grep test > results.txt" "Pipe and redirection"
    run_test "command_substitution" "echo \$(date)" "Command substitution"
    run_test "arithmetic_expansion" "echo \$((2 + 3))" "Arithmetic expansion"

    print_test_category "Edge Cases"
    run_test "many_spaces" "echo     hello     world" "Multiple spaces"
    run_test "tabs_and_spaces" "echo	hello world" "Tabs and spaces"
    run_test "special_chars" "echo !@#$%^&*()" "Special characters"
    run_test "unicode" "echo 'héllo wørld'" "Unicode characters"

    print_test_category "Syntax Errors"
    run_test "unmatched_paren" "echo (hello" "Unmatched parenthesis"
    run_test "invalid_redirect" "echo hello >>" "Invalid redirection"
    run_test "multiple_pipes" "echo hello || | cat" "Invalid pipe combination"
    run_test "bad_heredoc" "cat << " "Incomplete heredoc"
    
    # Clean up and show results
    cleanup_test_files
    print_summary
}

# Check command line arguments
if [ "$1" = "-i" ] || [ "$1" = "--interactive" ]; then
    interactive_mode
    exit 0
fi

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo "Usage: $0 [options] [minishell_path]"
    echo "Options:"
    echo "  -i, --interactive    Run in interactive mode"
    echo "  -h, --help          Show this help"
    echo ""
    echo "Examples:"
    echo "  $0                  # Test ./minishell"
    echo "  $0 /path/to/shell   # Test specific shell"
    echo "  $0 -i               # Interactive testing mode"
    exit 0
fi

# Run main function
main