/*
 @licstart  The following is the entire license notice for the
 JavaScript code in this file.

 Copyright (C) 1997-2017 by Dimitri van Heesch

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 @licend  The above is the entire license notice
 for the JavaScript code in this file
 */
function SearchBox(name, resultsPath, inFrame, label)
{
  this.searchLabel = label;
  this.DOMSearchField = function()
  {  return document.getElementById("MSearchField");  }
  this.DOMSearchBox = function()
  {  return document.getElementById("MSearchBox");  }
  this.OnSearchFieldFocus = function(isActive)
  {
    if (isActive)
    {
      this.DOMSearchBox().className = 'MSearchBoxActive';
      var searchField = this.DOMSearchField();
      if (searchField.value == this.searchLabel)
      {
        searchField.value = '';
      }
    }
    else
    {
      this.DOMSearchBox().className = 'MSearchBoxInactive';
      this.DOMSearchField().value   = this.searchLabel;
    }
  }
}

function trim(s) {
  return s?s.replace(/^\s\s*/, '').replace(/\s\s*$/, ''):'';
}

function getURLParameter(name) {
  return decodeURIComponent((new RegExp('[?|&]'+name+
         '='+'([^&;]+?)(&|#|;|$)').exec(location.search)
         ||[,""])[1].replace(/\+/g, '%20'))||null;
}

var entityMap = {
  "&": "&amp;",
  "<": "&lt;",
  ">": "&gt;",
  '"': '&quot;',
  "'": '&#39;',
  "/": '&#x2F;'
};

function escapeHtml(s) {
  return String(s).replace(/[&<>"'\/]/g, function (s) {
    return entityMap[s];
  });
}

function searchFor(query,page,count) {
  $.getJSON(serverUrl+"?cb=?",
  {
    n:count,
    p:page,
    q:query
  },
  function(data) {
    var results = $('#searchresults');
    $('#MSearchField').val(query);
    if (data.hits>0) {
      if (data.hits==1) {
        results.html('<p>'+searchResultsText[1]+'</p>');
      } else {
        results.html('<p>'+searchResultsText[2].replace(/\$num/,data.hits)+'</p>');
      }
      var r='<table>';
      $.each(data.items, function(i,item){
        var prefix = tagMap[item.tag];
        if (prefix) prefix+='/'; else prefix='';
        r+='<tr class="searchresult">'+
           '<td align="right">'+(data.first+i+1)+'.</td>'+
           '<td>'+escapeHtml(item.type)+'&#160;'+
                '<a href="'+escapeHtml(prefix+item.url)+
                '">'+escapeHtml(item.name)+'</a>';
        if (item.type=="source") {
          var l=item.url.match(/[1-9][0-9]*$/);
          if (l) r+=' at line '+parseInt(l[0]);
        }
        r+='</td>';
        for (var i=0;i<item.fragments.length;i++)
        {
          r+='<tr><td></td><td>'+item.fragments[i]+'</td></tr>';
        }
        r+='</tr>';
      });
      r+='</table>';
      if (data.pages>1) // write multi page navigation bar
      {
        r+='<div class="searchpages">';
        if (data.page>0)
        {
          r+='<span class="pages"><a href="javascript:searchFor(\''+escapeHtml(query)+'\','+(page-1).toString()+','+count.toString()+')">&laquo;</a></span>&nbsp;';
        }
        var firstPage = data.page-5;
        var lastPage  = data.page+5;
        if (firstPage<0)
        {
          lastPage-=firstPage;
          firstPage=0;
        }
        if (lastPage>data.pages)
        {
          lastPage=data.pages;
        }
        for(var i=firstPage;i<lastPage;i++)
        {
          if (i==data.page)
          {
            r+='<span class="pages"><b>'+(i+1).toString()+'</b></span>&nbsp;';
          }
          else
          {
            r+='<span class="pages"><a href="javascript:searchFor(\''+escapeHtml(query)+'\','+i.toString()+','+count.toString()+')">'+(i+1).toString()+'</a></span>&nbsp;';
          }
        }
        if (data.page+1<data.pages)
        {
          r+='<span class="pages"><a href="javascript:searchFor(\''+escapeHtml(query)+'\','+(page+1).toString()+','+count.toString()+')">&raquo;</a></span>';
        }
        r+='</div>';
      }
      results.append(r);
    } else {
      results.html('<p>'+searchResultsText[0]+'</p>');
    }
  });
}
/* @license-end */
