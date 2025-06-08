#
dir=$(pwd)

valgrind -q --track-fds=yes --leak-check=full --show-leak-kinds=all --trace-children=yes --track-origins=yes --show-reachable=yes --suppressions=$dir/readline.supp ./Pratt
