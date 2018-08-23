mkdir build
pushd .
cd build
REM goto skip
echo Remove these 3 --build lines once they are no longer needed.
conan install --build xmscore -pr ..\dev\xmsprofile_debug ..
conan install --build xmsgrid -pr ..\dev\xmsprofile_debug ..
conan install --build xmsinterp -pr ..\dev\xmsprofile_debug ..
conan install --build xmsgrid -pr ..\dev\xmsprofile_debug ..
conan install --build xmscore -pr ..\dev\xmsprofile_debug ..
pause
:skip
conan install -pr ..\dev\xmsprofile_debug ..
pause
cmake .. -G "Visual Studio 14 2015 Win64" -DIS_PYTHON_BUILD=False -DBUILD_TESTING=True
pause
popd
