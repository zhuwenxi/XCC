#! /bin/bash

total_line_nmbr=0

visit_dir() {
	parent_dir=$1
	pushd $parent_dir > /dev/null

	sub_dirs=$(ls)
	sub_files="$(ls | egrep "*\.c$") $(ls | egrep "*\.h$") $(ls | egrep "*\.def")"

	for file in $sub_files; do
		line_nmbr=$(wc -l $file | cut -d " " -f1)
		total_line_nmbr=$((total_line_nmbr+line_nmbr))
		printf "   %-4d lines : %s\n" $line_nmbr $1/$file
	done

	for dir in $sub_dirs
	do
		if [ -d $dir ]; then 
			visit_dir $dir
		# elif [ -f $dir]; then
			# line_nmbr = $(wc -l $dir)
			# total_line_nmbr=$((total_line_nmbr+line_number))
			# echo "count lines in " $dir": " $line_numbr 
		fi
	done

	popd > /dev/null
}

printf "==================================================\n\n"

current_dir=$(dirname $0)
cd $current_dir/../../ > /dev/null


visit_dir .
printf "\n==================================================\n"
printf "#  Total line number: %d" $total_line_nmbr 
printf "\n==================================================\n"


