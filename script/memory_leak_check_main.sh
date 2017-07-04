current_path=$(dirname $0)
pushd $current_path > /dev/null

../build/xcc
mtrace ../build/xcc $MALLOC_TRACE

popd > /dev/null
