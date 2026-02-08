/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2026 by Dimitri van Heesch

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

function toggleVisibility(linkObj) {
  return dynsection.toggleVisibility(linkObj);
}

let dynsection = {
  // helper function
  updateStripes : function() {
    const rows = document.querySelectorAll('table.directory tr');
    rows.forEach(row => {
      row.classList.remove('even', 'odd');
    });
    const visibleRows = Array.from(rows).filter(row => {
      return row.offsetParent !== null; // checks if element is visible
    });
    visibleRows.forEach((row, index) => {
      if (index % 2 === 0) {
        row.classList.add('even');
      } else {
        row.classList.add('odd');
      }
    });
  },

  slide : function(element, fromHeight, toHeight, duration=200) {
    element.style.overflow = 'hidden';
    element.style.transition = `height ${duration}ms ease-out`;
    element.style.height = fromHeight;
    setTimeout(() => {
      element.style.height = toHeight;
      setTimeout(() => {
        element.style.height = '';
        element.style.transition = '';
        element.style.overflow = '';
        if (toHeight === '0px') {
          element.style.display = 'none';
        }
      }, duration);
    }, 0);
  },

  toggleVisibility : function(linkObj) {
    const base = linkObj.getAttribute('id');
    const summary = document.getElementById(base+'-summary');
    const content = document.getElementById(base+'-content');
    const trigger = document.getElementById(base+'-trigger');
    const src = trigger ? trigger.getAttribute('src') : null;
    if (content.offsetParent !== null) { // checks if element is visible
      const height = content.offsetHeight;
      this.slide(content, height + 'px', '0px');
      if (summary) summary.style.display = '';
      linkObj.querySelectorAll('.arrowhead').forEach(el => {
        el.classList.add('closed');
        el.classList.remove('opened');
      });
    } else {
      // slideDown animation
      content.style.display = 'block';
      const height = content.scrollHeight;
      this.slide(content, '0px', height + 'px');
      if (summary) summary.style.display = 'none';
      linkObj.querySelectorAll('.arrowhead').forEach(el => {
        el.classList.remove('closed');
        el.classList.add('opened');
      });
    }
    return false;
  },

  toggleLevel : function(level) {
    document.querySelectorAll('table.directory tr').forEach(function(row) {
      const l = row.id.split('_').length-1;
      const i = document.getElementById('img'+row.id.substring(3));
      const a = document.getElementById('arr'+row.id.substring(3));
      if (l<level+1) {
        if (i) i.querySelectorAll('.folder-icon').forEach(el => el.classList.add('open'));
        if (a) {
          a.querySelectorAll('.arrowhead').forEach(el => {
            el.classList.remove('closed');
            el.classList.add('opened');
          });
        }
        row.style.display = '';
      } else if (l==level+1) {
        if (a) {
          a.querySelectorAll('.arrowhead').forEach(el => {
            el.classList.remove('opened');
            el.classList.add('closed');
          });
        }
        if (i) i.querySelectorAll('.folder-icon').forEach(el => el.classList.remove('open'));
        row.style.display = '';
      } else {
        row.style.display = 'none';
      }
    });
    this.updateStripes();
  },

  toggleFolder : function(id) {
    // the clicked row
    const currentRow = document.getElementById('row_'+id);
    if (!currentRow) return;

    // all rows after the clicked row
    const rows = [];
    let nextRow = currentRow.nextElementSibling;
    while (nextRow && nextRow.tagName === 'TR') {
      rows.push(nextRow);
      nextRow = nextRow.nextElementSibling;
    }

    const re = new RegExp('^row_'+id+'\\d+_$', "i"); //only one sub

    // only match elements AFTER this one (can't hide elements before)
    const childRows = rows.filter(function(row) { return row.id.match(re); });

    if (childRows.length === 0) return;

    function replaceClass(el,fromClass,toClass) {
      if (el.classList.contains(fromClass)) {
        el.classList.remove(fromClass);
        el.classList.add(toClass);
      }
    }

    // first row is visible we are HIDING
    if (childRows[0].offsetParent !== null) { // checks if element is visible
      // replace down arrow by right arrow for current row
      const currentRowSpans = currentRow.querySelectorAll("span");
      currentRowSpans.forEach(span => {
        if (span.classList.contains('iconfolder')) {
          span.querySelectorAll('.folder-icon').forEach(el => el.classList.remove("open"));
        }
        replaceClass(span,'opened','closed');
      });
      rows.forEach(row => {
        if (row.id.startsWith('row_'+id)) {
          row.style.display = 'none'; // hide all children
        }
      });
    } else { // we are SHOWING
      // replace right arrow by down arrow for current row
      const currentRowSpans = currentRow.querySelectorAll("span");
      currentRowSpans.forEach(span => {
        if (span.classList.contains('iconfolder')) {
          span.querySelectorAll('.folder-icon').forEach(el => el.classList.add("open"));
        }
        replaceClass(span,'closed','opened');
      });
      // replace down arrows by right arrows for child rows
      childRows.forEach(row => {
        const childRowSpans = row.querySelectorAll("span");
        childRowSpans.forEach(span => {
          if (span.classList.contains('iconfolder')) {
            span.querySelectorAll('.folder-icon').forEach(el => el.classList.remove("open"));
          }
          replaceClass(span,'opened','closed');
        });
        row.style.display = ''; //show all children
      });
    }
    this.updateStripes();
  },

  toggleInherit : function(id) {
    const rows = document.querySelectorAll('tr.inherit.'+id);
    const header = document.querySelector('tr.inherit_header.'+id);
    if (rows.length > 0 && rows[0].offsetParent !== null) { // checks if element is visible
      rows.forEach(row => row.style.display = 'none');
      if (header) {
        header.querySelectorAll('.arrowhead').forEach(el => {
          el.classList.add('closed');
          el.classList.remove('opened');
        });
      }
    } else {
      rows.forEach(row => row.style.display = '');
      if (header) {
        header.querySelectorAll('.arrowhead').forEach(el => {
          el.classList.remove('closed');
          el.classList.add('opened');
        });
      }
    }
  },

};

let codefold = {
  opened : true,

  show_plus : function(el) {
    if (el) {
      el.classList.remove('minus');
      el.classList.add('plus');
    }
  },

  show_minus : function(el) {
    if (el) {
      el.classList.add('minus');
      el.classList.remove('plus');
    }
  },

  // toggle all folding blocks
  toggle_all : function() {
    if (this.opened) {
      const foldAll = document.getElementById('fold_all');
      this.show_plus(foldAll);
      document.querySelectorAll('div[id^=foldopen]').forEach(el => el.style.display = 'none');
      document.querySelectorAll('div[id^=foldclosed]').forEach(el => el.style.display = '');
      document.querySelectorAll('div[id^=foldclosed] span.fold').forEach(el => this.show_plus(el));
    } else {
      const foldAll = document.getElementById('fold_all');
      this.show_minus(foldAll);
      document.querySelectorAll('div[id^=foldopen]').forEach(el => el.style.display = '');
      document.querySelectorAll('div[id^=foldclosed]').forEach(el => el.style.display = 'none');
    }
    this.opened=!this.opened;
  },

  // toggle single folding block
  toggle : function(id) {
    const openEl = document.getElementById('foldopen'+id);
    const closedEl = document.getElementById('foldclosed'+id);
    if (openEl) {
      openEl.style.display = openEl.style.display === 'none' ? '' : 'none';
      const nextEl = openEl.nextElementSibling;
      if (nextEl) {
        nextEl.querySelectorAll('span.fold').forEach(el => this.show_plus(el));
      }
    }
    if (closedEl) {
      closedEl.style.display = closedEl.style.display === 'none' ? '' : 'none';
    }
  },

  init : function() {
    // add code folding line and global control
    document.querySelectorAll('span.lineno').forEach((el, index) => {
      el.style.paddingRight = '4px';
      el.style.marginRight = '2px';
      el.style.display = 'inline-block';
      el.style.width = '54px';
      el.style.background = 'linear-gradient(var(--fold-line-color),var(--fold-line-color)) no-repeat 46px/2px 100%';
      const span = document.createElement('span');
      if (index === 0) { // add global toggle to first line
        span.className = 'fold minus';
        span.id = 'fold_all';
        span.onclick = () => this.toggle_all();
      } else {  // add vertical lines to other rows
        span.className = 'fold'
      }
      el.appendChild(span);
    });
    // add toggle controls to lines with fold divs
    document.querySelectorAll('div.foldopen').forEach(el => {
      // extract specific id to use
      const id = el.getAttribute('id').replace('foldopen','');
      // extract start and end foldable fragment attributes
      const start = el.getAttribute('data-start');
      const end = el.getAttribute('data-end');
      // replace normal fold span with controls for the first line of a foldable fragment
      const firstFold = el.querySelector('span.fold');
      if (firstFold) {
        const span = document.createElement('span');
        span.className = 'fold minus';
        span.onclick = () => this.toggle(id);
        firstFold.replaceWith(span);
      }
      // append div for folded (closed) representation
      const closedDiv = document.createElement('div');
      closedDiv.id = 'foldclosed'+id;
      closedDiv.className = 'foldclosed';
      closedDiv.style.display = 'none';
      el.after(closedDiv);
      // extract the first line from the "open" section to represent closed content
      const line = el.children[0] ? el.children[0].cloneNode(true) : null;
      if (line) {
        // remove any glow that might still be active on the original line
        line.classList.remove('glow');
        if (start) {
          // if line already ends with a start marker (e.g. trailing {), remove it
          line.innerHTML = line.innerHTML.replace(new RegExp('\\s*'+start+'\\s*$','g'),'');
        }
        // replace minus with plus symbol
        line.querySelectorAll('span.fold').forEach(span => {
          codefold.show_plus(span);
          // re-apply click handler as it is not copied with cloneNode
          span.onclick = () => codefold.toggle(id);
        });
        // append ellipsis
        const ellipsisLink = document.createElement('a');
        ellipsisLink.href = "javascript:codefold.toggle('"+id+"')";
        ellipsisLink.innerHTML = '&#8230;';
        line.appendChild(document.createTextNode(' '+start));
        line.appendChild(ellipsisLink);
        line.appendChild(document.createTextNode(end));
        // insert constructed line into closed div
        closedDiv.appendChild(line);
      }
    });
  },
};
/* @license-end */
