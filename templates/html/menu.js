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
function initMenu(relPath,treeview) {
  function makeTree(data,relPath,topLevel=false) {
    let result='';
    if ('children' in data) {
      if (!topLevel) {
        result+='<ul>';
      }
      for (let i in data.children) {
        let url;
        const link = data.children[i].url;
        if (link.substring(0,1)=='^') {
          url = link.substring(1);
        } else {
          url = relPath+link;
        }
        result+='<li><a href="'+url+'">'+
                                data.children[i].text+'</a>'+
                                makeTree(data.children[i],relPath)+'</li>';
      }
      if (!topLevel) {
        result+='</ul>';
      }
    }
    return result;
  }

  $('#main-nav').children().first().prepend(makeTree(menudata,relPath,true));
  searchBoxContents = $('#searchBoxPos2').html();
  const $mainMenuState = $('#main-menu-state');
  let prevWidth = 0;
  const initResizableIfExists = function() {
    if (typeof initResizableFunc==='function') initResizableFunc(treeview);
  }
  if ($mainMenuState.length) {
    // animate mobile menu
    $mainMenuState.change(function() {
      const $menu = $('#main-menu');
      let options = { duration: 250, step: initResizableIfExists };
      if (this.checked) {
        options['complete'] = () => $menu.css('display', 'block');
        $menu.hide().slideDown(options);
      } else {
        options['complete'] = () => $menu.css('display', 'none');
        $menu.show().slideUp(options);
      }
    });
    // set default menu visibility
    const resetState = function() {
      const $menu = $('#main-menu');
      const newWidth = $(window).outerWidth();
      if (newWidth!=prevWidth) {
        if ($(window).outerWidth()<768) {
          $mainMenuState.prop('checked',false); $menu.hide();
          $('#searchBoxPos2').empty();
          $('#searchBoxPos2').hide();
          $('#searchBoxPos1').html(searchBoxContents);
          $('#searchBoxPos1').show();
        } else {
          $menu.show();
          $('#searchBoxPos1').empty();
          $('#searchBoxPos1').hide();
          $('#searchBoxPos2').html(searchBoxContents);
          $('#searchBoxPos2').show();
        }
        if (typeof searchBox!=='undefined') {
          searchBox.CloseResultsWindow();
        }
        prevWidth = newWidth;
      }
    }
    $(window).ready(function() { resetState(); initResizableIfExists(); });
    $(window).resize(resetState);
  } else {
    initResizableIfExists();
  }
  $('#main-menu').smartmenus();
}
/* @license-end */
