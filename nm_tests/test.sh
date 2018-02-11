if [ $# -ne 2 -o \( "$1" != "-o" -a "$1" != "-n" \) ]; then
	echo "Usage: $0 -[n | o] mach-o_file"
	echo "Example: $0 -n /usr/bin/python"
	exit 2
fi

if [ "$1" = "-n" ]; then
	diff <(./ft_nm $2) <(nm $2)
fi

if [ "$1" = "-o" ]; then
	diff <(./ft_otool $2) <(otool -t $2)
fi
