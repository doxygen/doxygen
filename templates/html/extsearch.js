/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
 */
function SearchBox(name, resultsPath, extension) {
  this.DOMSearchField = () => document.getElementById("MSearchField");
  this.DOMSearchBox = () => document.getElementById("MSearchBox");
  this.OnSearchFieldFocus = isActive => {
    if (isActive) {
      this.DOMSearchBox().className = 'MSearchBoxActive';
    } else {
      this.DOMSearchBox().className = 'MSearchBoxInactive';
    }
  }
  this.CloseResultsWindow = function() {}
}

function trim(s) {
  return s?s.replace(/^\s\s*/, '').replace(/\s\s*$/, ''):'';
}

function getURLParameter(name) {
  return decodeURIComponent((new RegExp('[?|&]'+name+
         '='+'([^&;]+?)(&|#|;|$)').exec(location.search)
         ||[,""])[1].replace(/\+/g, '%20'))||null;
}

const entityMap = {
  "&": "&amp;",
  "<": "&lt;",
  ">": "&gt;",
  '"': '&quot;',
  "'": '&#39;',
  "/": '&#x2F;'
};

// see https://github.com/doxygen/doxygen/pull/10661 for a script to create a minimal font for just these icons.
const iconMap = {
  "source":            "E86F", // code
  "function":          "F1FC", // data_object
  "slot":              "F850", // location_chip
  "signal":            "F2A4", // data_alert
  "variable":          "E273", // variables
  "typedef":           "EAC7", // numbers
  "enum":              "F1FF", // data_array
  "enumvalue":         "F1FF", // data_array
  "property":          "E241", // list_alt
  "event":             "E4A7", // overview
  "related":           "E5D2", // read_more
  "friend":            "E0D0", // contact_page
  "define":            "EAC7", // numbers
  "file":              "E873", // description
  "namespace":         "F0C5", // clarify
  "concept":           "F1DA", // quick_phrases
  "group":             "EB12", // full_coverage
  "package":           "E2C6", // folder_zip
  "page":              "E88E", // article
  "dir":               "E2C7", // folder
  "module":            "E9B0", // grid_view
  "constants":         "F045", // pin
  "library":           "EB39", // book_5
  "type":              "E8F0", // type_specimen
  "union":             "F84F", // join
  "interface":         "EA77", // login
  "protocol category": "EBCB", // handshake
  "exception":         "E002", // warning
  "class":             "E574", // category
  "struct":            "F200", // foundation
  "service":           "E8B9", // settings_applications
  "singleton":         "E400", // looks_one
};

function getIconCode(t) {
  return iconMap[t] || "F525"; // asterisk
}

function escapeHtml(s) {
  return String(s).replace(/[&<>"'/]/g, function (s) {
    return entityMap[s];
  });
}

function capitalize(s) {
  return s.charAt(0).toUpperCase() + s.slice(1);
}

let jsonpRequestCounter = 0;

function getJSONP(url, params, callback) {
  const callbackName = '__doxygenSearchCb' + (jsonpRequestCounter++);
  const query = Object.keys(params).map(key =>
      encodeURIComponent(key) + '=' + encodeURIComponent(params[key])).join('&');
  const script = document.createElement('script');
  const parent = document.body || document.head || document.documentElement;
  let done = false;
  const cleanup = () => {
    if (done) return;
    done = true;
    delete window[callbackName];
    if (script.parentNode) script.parentNode.removeChild(script);
  };
  window[callbackName] = function(data) {
    cleanup();
    callback(data);
  };
  script.onerror = cleanup;
  script.src = url + (url.indexOf('?') >= 0 ? '&' : '?') + (query ? query + '&' : '') + 'cb=' + callbackName;
  parent.appendChild(script);
  setTimeout(cleanup, 30000);
}

function searchFor(query,page,count) {
  getJSONP(serverUrl, {
    n:count,
    p:page,
    q:query
  },
  function(data) {
    const results = document.getElementById('searchresults');
    document.getElementById('MSearchField').value = query;
    if (data.hits>0) {
      if (data.hits==1) {
        results.innerHTML = '<p>'+searchResultsText[1]+'</p>';
      } else {
        results.innerHTML = '<p>'+searchResultsText[2].replace(/\$num/,data.hits)+'</p>';
      }
      let r='<table>';
      data.items.forEach(function(item,i){
        let prefix = tagMap[item.tag];
        if (prefix) prefix+='/'; else prefix='';

        r+='<tr class="searchresult" title="'+capitalize(escapeHtml(item.type))+'">'+
           '<td align="right"><span class="material-symbols-outlined">&#x'+getIconCode(item.type)+';</span></td>'+
           '<td><a href="'+escapeHtml(prefix+item.url)+
                '">'+escapeHtml(item.name)+'</a>';
        if (item.type=="source") {
          const l=item.url.match(/[1-9][0-9]*$/);
          if (l) r+=' at line '+parseInt(l[0]);
        }

        for (let i=0;i<item.fragments.length;i++) {
          r+='<tr><td></td><td>'+item.fragments[i]+'</td></tr>';
        }
        r+='</tr>';
      });
      r+='</table>';
      if (data.pages>1) { // write multi page navigation bar
        r+='<div class="searchpages">';
        if (data.page>0) {
          r+='<span class="pages"><a href="javascript:searchFor(\''+escapeHtml(query)+'\','+(page-1).toString()+','+count.toString()+')">&laquo;</a></span>&#160;';
        }
        let firstPage = data.page-5;
        let lastPage  = data.page+5;
        if (firstPage<0) {
          lastPage-=firstPage;
          firstPage=0;
        }
        if (lastPage>data.pages) {
          lastPage=data.pages;
        }
        for (let i=firstPage;i<lastPage;i++) {
          if (i==data.page) {
            r+='<span class="pages"><b>'+(i+1).toString()+'</b></span>&#160;';
          } else {
            r+='<span class="pages"><a href="javascript:searchFor(\''+escapeHtml(query)+'\','+i.toString()+','+count.toString()+')">'+(i+1).toString()+'</a></span>&#160;';
          }
        }
        if (data.page+1<data.pages) {
          r+='<span class="pages"><a href="javascript:searchFor(\''+escapeHtml(query)+'\','+(page+1).toString()+','+count.toString()+')">&raquo;</a></span>';
        }
        r+='</div>';
      }
      results.insertAdjacentHTML('beforeend', r);
    } else {
      results.innerHTML = '<p>'+searchResultsText[0]+'</p>';
    }
  });
}
/* @license-end */
