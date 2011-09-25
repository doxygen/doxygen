#! Template that count number of C++ lines
The number of C++ code lines for #$ $text=$project_name;
#${
    $files = $project{"HEADERS"} . " " . $project{"SOURCES"};
    $text = `wc -l $files`;
#$}
