#! /bin/bash

total_line_nmbr=0

current_parent="."
current_parent_deps=0

visit_dir() {
	parent_dir=$1
	pushd $parent_dir > /dev/null

	if [ $parent_dir != "." ]; then
		current_parent_deps=$((current_parent_deps+1))
		current_parent=$current_parent/$1
	fi

	sub_dirs=$(ls)
	sub_files="$(ls | egrep "*\.c$") $(ls | egrep "*\.h$") $(ls | egrep "*\.def") $(ls | egrep "*\.sh$")" 

	for file in $sub_files; do
		line_nmbr=$(wc -l $file | cut -d " " -f1)
		total_line_nmbr=$((total_line_nmbr+line_nmbr))
		printf "   %-4d lines : %s\n" $line_nmbr $(echo $current_parent/$file | cut -c 3-) 
	done

	for dir in $sub_dirs
	do
		if [ -d $dir ]; then 
			visit_dir $dir
		fi
	done

	popd > /dev/null
	
	last_but_not_least=$((current_parent_deps))

	if [ $current_parent_deps -gt 0 ]; then
		current_parent=$(echo $current_parent | cut -d "/" -f 1-$last_but_not_least)
		current_parent_deps=$((current_parent_deps-1));
	fi
}

printf "==================================================\n\n"

current_dir=$(dirname $0)
cd $current_dir/../../ > /dev/null


visit_dir .
printf "\n==================================================\n"
printf "#  Total line number: %d" $total_line_nmbr 
printf "\n==================================================\n"


