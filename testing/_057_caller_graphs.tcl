proc inFileB args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    inFileA
}
