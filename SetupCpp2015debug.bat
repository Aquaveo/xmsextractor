mkdir build
pushd .
cd build
conan install -pr ..\dev\xmsprofile_debug .. --build missing
pause
cmake .. -G "Visual Studio 14 2015 Win64" -DIS_PYTHON_BUILD=False -DBUILD_TESTING=True
pause
popd
