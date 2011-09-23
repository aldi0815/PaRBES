@echo off
mkdir ..\solve\%1
cp x64\Debug\PaRBES.exe ..\solve\%1	
cd ..\solve\%1

for %%f in (..\..\bes\*.bes) do (
	
		.\PaRBES -s %%f
)