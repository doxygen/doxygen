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
  let sidenav,mainnav,pagenav,container,navtree,content,header,footer,barWidth=6;
  const RESIZE_COOKIE_NAME = ''+'width';
  const PAGENAV_COOKIE_NAME = ''+'pagenav';
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

  function constrainPanelWidths(leftPanelWidth,rightPanelWidth) {
    const contentWidth = container.width()-leftPanelWidth-rightPanelWidth;
    const minContentWidth = 250;
    const minPanelWidth = barWidth;
    if (contentWidth<minContentWidth) // need to shrink panels
    {
      const deficit = minContentWidth - contentWidth;
      const shrinkRight = Math.min(deficit, rightPanelWidth-minPanelWidth);
      rightPanelWidth -= shrinkRight;
      const remainingDeficit = deficit - shrinkRight;
      const shrinkLeft = Math.min(remainingDeficit, leftPanelWidth-minPanelWidth);
      leftPanelWidth -= shrinkLeft;
    } else {
      rightPanelWidth = pagenav.length ? Math.max(minPanelWidth,rightPanelWidth) : 0;
      leftPanelWidth = Math.max(minPanelWidth,leftPanelWidth);
      }
    return { leftPanelWidth, rightPanelWidth }
  }

  function updateWidths(sidenavWidth,pagenavWidth)
  {
    const widths = constrainPanelWidths(sidenavWidth,pagenavWidth);
    const widthStr = parseInt(widths.leftPanelWidth)+"px";
    content.css({marginLeft:widthStr});
    if (fullSidebar) {
      footer.css({marginLeft:widthStr});
      if (mainnav) {
        mainnav.css({marginLeft:widthStr});
      }
    }
    sidenav.css({width:widthStr});
    if (pagenav.length) {
      container.css({gridTemplateColumns:'auto '+parseInt(widths.rightPanelWidth)+'px'});
      pagenav.css({width:parseInt(widths.rightPanelWidth-1)+'px'});
    }
    return widths;
  }

  function resizeWidth() {
    const sidenavWidth = $(sidenav).outerWidth()-barWidth;
    const pagenavWidth = pagenav.length ? $(pagenav).outerWidth() : 0;
    const widths = updateWidths(sidenavWidth,pagenavWidth);
    Cookie.writeSetting(RESIZE_COOKIE_NAME,widths.leftPanelWidth-barWidth);
    if (pagenav.length) {
      Cookie.writeSetting(PAGENAV_COOKIE_NAME,widths.rightPanelWidth);
    }
  }

  function restoreWidth(sidenavWidth,pagenavWidth) {
    updateWidths(sidenavWidth,pagenavWidth);
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
    resizeWidth();
    showHideNavBar();
    if (location.hash.slice(1)) {
      (document.getElementById(location.hash.slice(1))||document.body).scrollIntoView();
    }
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
    if (pagenav.length) {
      pagehandle  = $("#page-nav-resize-handle");
      pagehandle.on('mousedown',function(e) { 
         container.addClass('resizing');
         pagehandle.addClass('dragging');
         $(document).on('mousemove',function(e) {
           let pagenavWidth = container[0].offsetWidth-e.clientX+barWidth/2;
           const sidenavWidth = sidenav.width();
           const widths = constrainPanelWidths(sidenavWidth,pagenavWidth);
           container.css({gridTemplateColumns:'auto '+parseInt(widths.rightPanelWidth)+'px'});
           pagenav.css({width:parseInt(widths.rightPanelWidth-1)+'px'});
           content.css({marginLeft:parseInt(widths.leftPanelWidth)+'px'});
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
    const width = parseInt(Cookie.readSetting(RESIZE_COOKIE_NAME,$TREEVIEW_WIDTH));
    const pagenavWidth = parseInt(Cookie.readSetting(PAGENAV_COOKIE_NAME,250));
    if (width) { restoreWidth(width+barWidth,pagenavWidth); } else { resizeWidth(); }
  }
  resizeHeight(treeview);
  const url = location.href;
  const i=url.indexOf("#");
  if (i>=0) window.location.hash=url.substr(i);
  const _preventDefault = function(evt) { evt.preventDefault(); };
  if (treeview)
  {
    $("#splitbar").bind("dragstart", _preventDefault).bind("selectstart", _preventDefault);
  }
  $(window).on('load',function() { resizeHeight(treeview); });
}
/* @license-end */
