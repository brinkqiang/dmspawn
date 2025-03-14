

rmdir /S /Q build 2> nul
mkdir build
pushd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=relwithdebinfo -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=relwithdebinfo -DVCPKG_TARGET_TRIPLET=x64-windows ..
cmake --build . --config relwithdebinfo
popd

rem pause