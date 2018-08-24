mkdir build_py
pushd .
cd build_py
conan install -pr ..\dev\xmsprofile_py .. --build missing
pause
cmake .. -G"Visual Studio 14 2015 Win64" -DIS_PYTHON_BUILD=True -DBUILD_TESTING=False -DPYTHON_TARGET_VERSION=3.5
pause
popd
echo off
echo OPEN VS2015 FILE build_py\xmsextractor.sln
echo BUILD PROJECT xmsextractor_py FOR RELEASE x64
echo COPY PYTHON LIBS FROM C:\Users\<account>\.conan\data\xmsgrid\...\*.pyd TO build_py\lib\.
echo RUN TEST FILE Test_py.bat
pause
