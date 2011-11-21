@echo off
mkdir ..\eval\%1%2
copy x64\Release\PaRBES.exe ..\eval\%1%2
cd ..\eval\%1%2
	
REM ping -n 30 127.0.0.1 > NUL

for %%f in (..\..\bes\*%2.bes) do (
	
	copy %%f .
	
	for %%g in (*.bes) do (
	
		.\PaRBES -e %%g
		
		gnuplot %%g.plot
		
		diff -q %%g.orig.seq.sol %%g.rev.seq.sol
		diff -q %%g.orig.seq.sol %%g.rnd.seq.sol
		diff -q %%g.orig.seq.sol %%g.orig.par.sol
REM		diff -q %%g.orig.seq.sol %%g.asc.seq.sol
REM		diff -q %%g.orig.seq.sol %%g.dsc.seq.sol
	
		del %%g.rev.seq.sol
		del %%g.rnd.seq.sol
		
		move %%g.orig.seq.sol %%g.sol
	)
	
	del *.bes
)