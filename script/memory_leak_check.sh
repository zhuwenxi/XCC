current_path=$(dirname $0)
pushd $current_path > /dev/null

../build/test_all > /dev/null
mtrace ../build/test_all $MALLOC_TRACE

popd > /dev/null