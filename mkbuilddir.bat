if exist %BUILD_DIR% goto build_dir_exists

mkdir %BUILD_DIR%

:build_dir_exists

if exist %BUILD_DIR%\%BUILD_TYPE% goto build_type_dir_exists

mkdir %BUILD_DIR%\%BUILD_TYPE%

:build_type_dir_exists

exit 0
