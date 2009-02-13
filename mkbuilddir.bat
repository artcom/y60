if exist %1 goto build_dir_exists

mkdir %1

:build_dir_exists

if exist %1\%2 goto build_type_dir_exists

mkdir %1\%2

:build_type_dir_exists

exit 0
