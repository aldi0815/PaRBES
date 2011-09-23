@echo off
mkdir ..\eval\%1
cp x64\Debug\PaRBES.exe ..\eval\%1	
cd ..\eval\%1

for %%f in (..\..\bes\*.bes) do (
	
	cp %%f .
	
	for %%g in (*.bes) do (
	
		.\PaRBES -e %%g
		
		gnuplot %%g.plot
		
		diff -q %%g.orig.seq.sol %%g.rev.seq.sol
		diff -q %%g.orig.seq.sol %%g.rnd.seq.sol
REM		diff -q %%g.orig.seq.sol %%g.asc.seq.sol
REM		diff -q %%g.orig.seq.sol %%g.dsc.seq.sol
	
		rm %%g.rev.seq.sol
		rm %%g.rnd.seq.sol
		
		mv %%g.orig.seq.sol %%g.sol
	)
	
	rm *.bes
)