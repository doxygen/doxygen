set Dir_Old=%cd%
cd /D %~dp0

del /s /f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl doxygen_manual.pdf

pdflatex doxygen_manual
echo ----
makeindex doxygen_manual.idx
echo ----
pdflatex doxygen_manual

setlocal enabledelayedexpansion
set count=8
:repeat
set content=X
for /F "tokens=*" %%T in ( 'findstr /C:"Rerun LaTeX" doxygen_manual.log' ) do set content="%%~T"
if !content! == X for /F "tokens=*" %%T in ( 'findstr /C:"Rerun to get cross-references right" doxygen_manual.log' ) do set content="%%~T"
if !content! == X goto :skip
set /a count-=1
if !count! EQU 0 goto :skip

echo ----
pdflatex doxygen_manual
goto :repeat
:skip
endlocal
makeindex doxygen_manual.idx
pdflatex doxygen_manual
cd /D %Dir_Old%
set Dir_Old=
