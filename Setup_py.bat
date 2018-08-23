mkdir build_py
pushd .
cd build_py
conan install -pr ..\dev\xmsprofile_py .. --build missing
pause
cmake .. -G"Visual Studio 14 2015 Win64" -DIS_PYTHON_BUILD=True -DBUILD_TESTING=False -DPYTHON_TARGET_VERSION=3.5
pause
popd
