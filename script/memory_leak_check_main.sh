current_path=$(dirname $0)
pushd $current_path > /dev/null

../build/xcc > /dev/null
mtrace ../build/xcc $MALLOC_TRACE

popd > /dev/null
