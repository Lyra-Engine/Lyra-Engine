@echo off

for /f "delims=" %%i in ('git rev-parse --show-toplevel') do set GIT_ROOT=%%i

echo ######################
echo ### Configure CMake...
echo ######################
cmake -S %GIT_ROOT% -B %GIT_ROOT%\Scratch -A x64 --preset MSVC

echo ###########################
echo ### Build debug binaries...
echo ###########################
cmake --build %GIT_ROOT%\Scratch --config Debug

echo #############################
echo ### Build release binaries...
echo #############################
cmake --build %GIT_ROOT%\Scratch --config Release

echo #################################
echo ### Install binaries to system...
echo #################################
cmake --install %GIT_ROOT%\Scratch

pause
