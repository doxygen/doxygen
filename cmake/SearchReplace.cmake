message("Replacing ${search} by ${replace} in file ${src} and writing to ${dst}...")
file(READ ${src} file_contents)
string(REPLACE "${search}" "${replace}" file_contents ${file_contents})
file(WRITE ${dst} ${file_contents})
