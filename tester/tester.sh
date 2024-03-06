#!/bin/bash

#set -e
#set -u
#set -o pipefail

readonly PROG=$(basename $0)

#	/*------------------------------------------------------------*/
#	/*--- CONFIG                                               ---*/
#	/*--- (Most of these can be changed with arguments)        ---*/
#	/*------------------------------------------------------------*/

readonly          DEFAULT_PROGRAM="None"
readonly             DEFAULT_MODE="args"
readonly     DEFAULT_INPUT_SUFFIX="in"
readonly  DEFAULT_INPUT_DIRECTORY="infiles"
readonly DEFAULT_OUTPUT_DIRECTORY="outfiles"
readonly          DEFAULT_TIMEOUT=2

# readonly       OK_COLOR=$(tput setaf 2) # green
# readonly    ERROR_COLOR=$(tput setaf 1) # red
# readonly BOLD_UNDERLINE=$(tput bold)$(tput smul)
# readonly       NO_COLOR=$(tput sgr0)
readonly       OK_COLOR="\e[32m"
readonly    ERROR_COLOR="\e[31m"
readonly BOLD_UNDERLINE=""
readonly       NO_COLOR="\e[0m"

#	/*------------------------------------------------------------*/
#	/*--- Display help message                                 ---*/
#	/*------------------------------------------------------------*/

function show_usage() {

    cat <<EOF
Usage: $0 [options]

Options
    -p   <program> (default: '$DEFAULT_PROGRAM')
          The program to test.

    -i   <input_directory> (default: '$DEFAULT_INPUT_DIRECTORY')
          The directory containing the input files.

    -a   <args>
          Extra arguments to pass to the program.

    -s   <input_file_suffix> (default: '$DEFAULT_INPUT_SUFFIX')
          The suffix of the input files. 
    
    -o   <output_directory> (default: '$DEFAULT_OUTPUT_DIRECTORY')
          The directory to write the output files to.

    -m   <mode> (default: '$DEFAULT_MODE')
          The mode in which to run the tests.
          Available modes:
              - args
              - path
              - custom      
    -c    Don't do infile - outfile comparisons.
    
    -v    Run each test case through Valgrind.
  
    -r   <output_file>
          Redirect output (summary not included) to a file.
    
    -h    Show this help message.

EOF
}

#	/*------------------------------------------------------------*/
#	/*--- Parse arguments and setup vars                       ---*/
#	/*------------------------------------------------------------*/

if [ $# -eq 0 ];
    then
        show_usage
        exit 1
fi

while getopts "p:s:a:i:o:m:cvr:h" opt; do
    case $opt in
        p)       program_name="$OPTARG";;
        m)               mode="$OPTARG";;
        a)         extra_args="$OPTARG";;
        s)  input_file_suffix="$OPTARG";;
        i)    input_directory="$OPTARG";;
        o)   output_directory="$OPTARG";;
        v) run_under_valgrind=true;;
        r)     do_redirection="$OPTARG";;
        c)           compare=false;;
        h) show_usage; exit 0;;
        \?) show_usage; exit 1;;
    esac
done

      program_name=${program_name:-$DEFAULT_PROGRAM}
              mode=${mode:-$DEFAULT_MODE}
 input_file_suffix=${input_file_suffix:-$DEFAULT_INPUT_SUFFIX}
   input_directory=${input_directory:-$DEFAULT_INPUT_DIRECTORY}
  output_directory=${output_directory:-$DEFAULT_OUTPUT_DIRECTORY}
run_under_valgrind=${run_under_valgrind:-false}
    do_redirection=${do_redirection:-false}
           compare=${compare:-true}

#	/*------------------------------------------------------------*/
#	/*--- Exit with `error_message`                            ---*/
#	/*------------------------------------------------------------*/

function exit_with_error() {

    local error_message="$1"
    >&2 printf "$PROG: %s\n" "$error_message"
    exit 1
}

function output() {

    local output="$1"

    if [ "$do_redirection" != false ];
        then
            printf "%s\n" "$output" >> "$do_redirection"
    else
        printf "%s\n" "$output"
    fi
}

#	/*------------------------------------------------------------*/
#	/*--- Basic sanity checks                                  ---*/
#	/*------------------------------------------------------------*/

function check_prerequisites() {

    if [ ! -x "$program_name" ]; then
        exit_with_error "$program_name: Not an executable file (-p argument)" 
    fi
    
    if [ ! -d "$input_directory" ]; then
        exit_with_error "$input_directory: Not found (-i argument)" 
    elif [ ! -r "$input_directory" ]; then
        exit_with_error "$input_directory: Not readable (-i argument)" 
    fi
    
    if [ -z "$(ls -A $input_directory/*.$input_file_suffix 2> /dev/null)" ]; then
        exit_with_error "$input_directory: Is empty (-i argument)" 
    fi
    
    if [ ! -d "$output_directory" ]; then
        mkdir -vp "$output_directory" &> /dev/null
    elif [ ! -w "$output_directory" ]; then
        exit_with_error "$output_directory: Not writable (-o argument)" 
    fi
    
    if [ "$run_under_valgrind" = true ] && [ ! -x "$(command -v valgrind)" ]; then
        exit_with_error "Valgrind: Not found (-v argument)" 
    fi
}

#	/*------------------------------------------------------------*/
#	/*--- the content of the infile is passes as arguments to  ---*/
# /*--- the program                                          ---*/
#	/*------------------------------------------------------------*/

function __args_mode() {

    local input_file="$1"
    local actual_output_file="$2"
    local valgrind_log_file="$3"

    if [ "$run_under_valgrind" = true ];
        then
            timeout $DEFAULT_TIMEOUT                                 \
            cat "$input_file"                                        \
            | xargs                                                  \
            valgrind                                                 \
                -q                                                   \
                --leak-check=full                                    \
                --show-leak-kinds=all                                \
                --track-origins=yes                                  \
                --log-file="$valgrind_log_file"                      \
                --suppressions="ignore_readline.supp"                  \
                --error-exitcode=1                                   \
                ./"$program_name" $extra_args &> "$actual_output_file"
    else
        timeout $DEFAULT_TIMEOUT               \
        cat "$input_file"                      \
        | xargs                                \
        ./"$program_name" $extra_args &> "$actual_output_file"
    fi

    exit_code=$?
    return $exit_code
}

#	/*------------------------------------------------------------*/
#	/*--- The infile itself is passed to the program.          ---*/
#	/*------------------------------------------------------------*/

function __path_mode() {

    local input_file="$1"
    local actual_output_file="$2"
    local valgrind_log_file="$3"

    if [ "$run_under_valgrind" = true ];
        then
            timeout $DEFAULT_TIMEOUT                                               \
            valgrind                                                               \
                -q                                                                 \
                --leak-check=full                                                  \
                --show-leak-kinds=all                                              \
                --track-origins=yes                                                \
                --log-file="$valgrind_log_file"                                    \
                --error-exitcode=1                                                 \
                ./"$program_name" $extra_args "$input_file" &> "$actual_output_file"
    else
        timeout $DEFAULT_TIMEOUT                                           \
        ./"$program_name" $extra_args "$input_file" &> "$actual_output_file"
    fi
    exit_code=$?
    return $exit_code
}

#	/*------------------------------------------------------------*/
#	/*--- Random custom tests                                  ---*/
#	/*--- Currently set up for minishell                       ---*/
#	/*------------------------------------------------------------*/

function __custom_mode() {
    local input_file="$1"
    local actual_output_file="$2"
    local valgrind_log_file="$3"
    
    if [ "$run_under_valgrind" = true ];
        then
            timeout $DEFAULT_TIMEOUT                         \
            cat "$input_file"                                \
            | valgrind                                       \
                -q                                           \
                --leak-check=full                            \
                --show-leak-kinds=all                        \
                --track-origins=yes                          \
                --log-file="$valgrind_log_file"              \
                --suppressions=leaks.txt                     \
				--error-exitcode=1                           \
                ./"$program_name" 2>&- > "$actual_output_file"
    else
        timeout $DEFAULT_TIMEOUT                                         \
        cat "$input_file" | ./"$program_name" 2>&- > "$actual_output_file"
    fi
    exit_code=$?
    return $exit_code
}

#	/*------------------------------------------------------------*/
#	/*--- Run the current test                                 ---*/
#	/*------------------------------------------------------------*/

function run_test() {

    local name="$1"
    local input_file="$2"
    local actual_output_file="$3"
    local valgrind_log_file="$4"
    local expected_output_file="$5"

    output "$((passed + failed + skipped)). $name"
    output "    └── Input: $input_file"

    func="None"
    if   [ "$mode" = "args"   ]; then func='__args_mode'
    elif [ "$mode" = "path"   ]; then func='__path_mode'
    elif [ "$mode" = "custom" ]; then func='__custom_mode'
    else
        output "    └── Status: ${ERROR_COLOR}Aborted${NO_COLOR}"
        exit_with_error "$mode: Not supported"
    fi

    $func "$input_file" "$actual_output_file" "$valgrind_log_file"

    exit_code=$?

    # Remove leading and trailing whitespaces from both the
    # expected and actual output
    # sed -i 's/^[ \t]*//;s/[ \t]*$//' "$actual_output_file"
    # sed -i 's/^[ \t]*//;s/[ \t]*$//' "$expected_output_file"

    if [ ! -f "$actual_output_file" ] ;
        then
            output "    └── Status: ${ERROR_COLOR}Incomplete${NO_COLOR}"
            output "        └── Reason: Actual output not found"
            output ""
            ((failed++))
            return
    fi

    #output "    └── Output: $actual_output_file"
    #output "        └── Return code: $exit_code"

    if [ "$exit_code" = 124 ] ;
        then
            output "        └── Status: ${ERROR_COLOR}TIMEOUT${NO_COLOR}"
            output ""
            ((failed++))
            return 
    fi

    if [ "$compare" = true ];
        then
            if cmp -s "$actual_output_file" "$expected_output_file";
                then
                    output "        └── Status: ${OK_COLOR}OK${NO_COLOR}"
                    ((passed++))
            else
                output "        └── Status: ${ERROR_COLOR}KO${NO_COLOR}"
                output "            └── Expected: $expected_output_file"
                output "            └── Actual: $actual_output_file"
                output "            └── Diff:"
                output \
                    "$(\
                        2>&1                    \
                        diff --color -Tp        \
                        "$actual_output_file"   \
                        "$expected_output_file" \
                        | sed 's/^/                /'
                    )"
                ((failed++))
            fi
    else
        ((skipped++))
    fi

    if [ -s "$valgrind_log_file" ];
        then
            sed -i 's/^==[0-9]*== //' "$valgrind_log_file"
            output "    └── Valgrind:"
            output "        └── ${ERROR_COLOR}MEMORY ERROR${NO_COLOR}"
            output \
                "$(\
                    2>&1                     \
                    cat "$valgrind_log_file" \
                    | sed 's/^/          /'
                )"
            ((memory_errors++))
    fi

    output ""
}

#	/*------------------------------------------------------------*/
#	/*--- Display test results                                 ---*/
#	/*------------------------------------------------------------*/

print_summary() {

    cat << EOF

    Summary: [$passed/$((passed + failed + skipped))]
    --------------------------- 
    Memory errors: $memory_errors
    Tests Skipped: $skipped


EOF
}

#	/*------------------------------------------------------------*/
#	/*---                         ENTRY                        ---*/
#	/*------------------------------------------------------------*/

check_prerequisites

passed=0
failed=0
skipped=0
memory_errors=0

for file in "$input_directory"/*."$input_file_suffix"; 
    do
        filename=$(basename -- "$file")
        test_name="${filename%.*}"

        run_test                                \
            "$test_name"                        \
            "$file"                             \
            "$output_directory/$test_name.out"  \
            "$output_directory/$test_name.valg" \
            "$input_directory/$test_name.out"
done

print_summary

if [ $failed -eq 0 ];
    then
        if [ $memory_errors -eq 0 ];
            then
                exit 0
        fi
fi

exit 1

# Last update: 05.02.23
