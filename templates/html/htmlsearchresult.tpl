{# input: baseName #}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html><head><title></title>
<meta http-equiv="Content-Type" content="text/xhtml;charset=UTF-8"/>
<meta name="generator" content="Doxygen {{ doxygen.version }}"/>
<link rel="stylesheet" type="text/css" href="search.css"/>
<script type="text/javascript" src="{{ baseName }}.js"></script>
<script type="text/javascript" src="search.js"></script>
</head>
<body class="SRPage">
<div id="SRIndex">
<div class="SRStatus" id="Loading">{{ tr.loading }}</div>
<div id="SRResults"></div>
<script type="text/javascript"><!--
createResults();
--></script>
<div class="SRStatus" id="Searching">{{ tr.searching }}</div>
<div class="SRStatus" id="NoMatches">{{ tr.noMatches }}</div>
<script type="text/javascript"><!--
document.getElementById("Loading").style.display="none";
document.getElementById("NoMatches").style.display="none";
var searchResults = new SearchResults("searchResults");
searchResults.Search();
--></script>
</div>
</body>
</html>
