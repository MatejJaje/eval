@echo off
setlocal

:: Prompt the user for input
set /p user_input=task: 

:: Check if input was provided
if "%user_input%"=="" (
    echo No input provided. Exiting...
    pause
    exit /b
)

:: Call the other batch script with the input as an argument
call p.bat %user_input%

:: Keep the terminal window open after execution
pause