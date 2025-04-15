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

function initResizable(treeview) {
  let sidenav,mainnav,navtree,content,header,footer,barWidth=6;
  const RESIZE_COOKIE_NAME = '$PROJECTID'+'width';
  const PAGENAV_COOKIE_NAME = '$PROJECTID'+'pagenav';
  const fullSidebar = typeof page_layout!=='undefined' && page_layout==1;

  function showHideNavBar() {
    let bar = $('div.sm-dox');
    if (fullSidebar && mainnav && bar) {
      if (mainnav.width()<768) {
        bar.hide();
      } else {
        bar.show();
      }
    }
  }

  function resizeWidth() {
    const sidenavWidth = $(sidenav).outerWidth();
    const widthStr = parseInt(sidenavWidth-barWidth)+"px";
    content.css({marginLeft:widthStr});
    if (fullSidebar) {
      footer.css({marginLeft:widthStr});
      if (mainnav) {
        mainnav.css({marginLeft:widthStr});
      }
    }
    Cookie.writeSetting(RESIZE_COOKIE_NAME,sidenavWidth-barWidth);
  }

  function restoreWidth(navWidth,pagenavWidth) {
    const widthStr = parseInt(navWidth)+"px";
    content.css({marginLeft:widthStr});
    if (fullSidebar) {
      footer.css({marginLeft:widthStr});
      if (mainnav) {
        mainnav.css({marginLeft:widthStr});
      }
    }
    sidenav.css({width:navWidth + "px"});
    if (pagenav.length!=0) {
      container.css({gridTemplateColumns:'auto '+parseInt(pagenavWidth)+'px'});
      pagenav.css({width:parseInt(pagenavWidth)+'px'});
    }
    showHideNavBar();
  }

  function resizeHeight(treeview) {
    const headerHeight = header.outerHeight();
    const windowHeight = $(window).height();
    let contentHeight;
    if (treeview)
    {
      const footerHeight = footer.outerHeight();
      let navtreeHeight,sideNavHeight;
      if (!fullSidebar) {
        contentHeight = windowHeight - headerHeight - footerHeight - 1;
        navtreeHeight = contentHeight;
        sideNavHeight = contentHeight;
      } else if (fullSidebar) {
        contentHeight = windowHeight - footerHeight - 1;
        navtreeHeight = windowHeight - headerHeight - 1;
        sideNavHeight = windowHeight - 1;
        if (mainnav) {
          contentHeight -= mainnav.outerHeight();
        }
      }
      navtree.css({height:navtreeHeight + "px"});
      sidenav.css({height:sideNavHeight + "px"});
    } else {
      contentHeight = windowHeight - headerHeight - 1;
    }
    content.css({height:contentHeight + "px"});
    showHideNavBar();
    if (location.hash.slice(1)) {
      (document.getElementById(location.hash.slice(1))||document.body).scrollIntoView();
    }
  }

  function collapseExpand() {
    let newWidth;
    if (sidenav.width()>0) {
      newWidth=0;
    } else {
      const width = Cookie.readSetting(RESIZE_COOKIE_NAME,$TREEVIEW_WIDTH);
      newWidth = (width>$TREEVIEW_WIDTH && width<$(window).width()) ? width : $TREEVIEW_WIDTH;
    }
    restoreWidth(newWidth,250);
    const sidenavWidth = $(sidenav).outerWidth();
    Cookie.writeSetting(RESIZE_COOKIE_NAME,sidenavWidth-barWidth);
  }

  header  = $("#top");
  content = $("#doc-content");
  footer  = $("#nav-path");
  sidenav = $("#side-nav");
  if (document.getElementById('main-nav')) {
    mainnav = $("#main-nav");
  }
  if (treeview) {
    navtree = $("#nav-tree");
    pagenav   = $("#page-nav");
    container = $("#container");
    $(".side-nav-resizable").resizable({resize: function(e, ui) { resizeWidth(); } });
    $(sidenav).resizable({ minWidth: 0 });
    if (pagenav.length!=0) {
      pagehandle  = $("#page-nav-resize-handle");
      pagehandle.on('mousedown',function(e) { 
         container.addClass('resizing');
         pagehandle.addClass('dragging');
         $(document).on('mousemove',function(e) {
           let pagenavWidth = container[0].offsetWidth-e.clientX+barWidth/2;
           if (pagenavWidth<barWidth) {
             pagenavWidth = barWidth;
           }
           container.css({gridTemplateColumns:'auto '+parseInt(pagenavWidth)+'px'});
           pagenav.css({width:parseInt(pagenavWidth)+'px'});
           Cookie.writeSetting(PAGENAV_COOKIE_NAME,pagenavWidth);
         });
         $(document).on('mouseup', function(e) {
           container.removeClass('resizing');
           pagehandle.removeClass('dragging');
           $(document).off('mousemove');
           $(document).off('mouseup');
         });
      });
    } else {
      container.css({gridTemplateColumns:'auto'});
    }
  }
  $(window).resize(function() { resizeHeight(treeview); });
  if (treeview)
  {
    const width = Cookie.readSetting(RESIZE_COOKIE_NAME,$TREEVIEW_WIDTH);
    const pagenavWidth = Cookie.readSetting(PAGENAV_COOKIE_NAME,250);
    if (width) { restoreWidth(width,pagenavWidth); } else { resizeWidth(); }
  }
  resizeHeight(treeview);
  const url = location.href;
  const i=url.indexOf("#");
  if (i>=0) window.location.hash=url.substr(i);
  const _preventDefault = function(evt) { evt.preventDefault(); };
  if (treeview)
  {
    $("#splitbar").bind("dragstart", _preventDefault).bind("selectstart", _preventDefault);
    $(".ui-resizable-handle").dblclick(collapseExpand);
  }
  $(window).on('load',function() { resizeHeight(treeview); });
}
/* @license-end */
