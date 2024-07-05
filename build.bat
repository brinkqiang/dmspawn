
rmdir /S /Q build
mkdir build
pushd build
cmake -A x64 -DCMAKE_BUILD_TYPE=relwithdebinfo -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
cmake --build .
popd

rem pause