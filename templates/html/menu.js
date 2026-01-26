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
/* eslint-disable no-unused-vars */
function initMenu(relPath,treeview) {
/* eslint-enable no-unused-vars */
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

  // Helper function to slide down element
  function slideDown(element, duration, callback) {
    element.style.removeProperty('display');
    let display = window.getComputedStyle(element).display;
    if (display === 'none') display = 'block';
    element.style.display = display;
    let height = element.offsetHeight;
    element.style.overflow = 'hidden';
    element.style.height = 0;
    element.style.paddingTop = 0;
    element.style.paddingBottom = 0;
    element.style.marginTop = 0;
    element.style.marginBottom = 0;
    element.offsetHeight; // force reflow
    element.style.boxSizing = 'border-box';
    element.style.transitionProperty = "height, margin, padding";
    element.style.transitionDuration = duration + 'ms';
    element.style.height = height + 'px';
    element.style.removeProperty('padding-top');
    element.style.removeProperty('padding-bottom');
    element.style.removeProperty('margin-top');
    element.style.removeProperty('margin-bottom');
    window.setTimeout(() => {
      element.style.removeProperty('height');
      element.style.removeProperty('overflow');
      element.style.removeProperty('transition-duration');
      element.style.removeProperty('transition-property');
      if (callback) callback();
    }, duration);
  }

  // Helper function to slide up element
  function slideUp(element, duration, callback) {
    element.style.transitionProperty = 'height, margin, padding';
    element.style.transitionDuration = duration + 'ms';
    element.style.boxSizing = 'border-box';
    element.style.height = element.offsetHeight + 'px';
    element.offsetHeight; // force reflow
    element.style.overflow = 'hidden';
    element.style.height = 0;
    element.style.paddingTop = 0;
    element.style.paddingBottom = 0;
    element.style.marginTop = 0;
    element.style.marginBottom = 0;
    window.setTimeout(() => {
      element.style.display = 'none';
      element.style.removeProperty('height');
      element.style.removeProperty('padding-top');
      element.style.removeProperty('padding-bottom');
      element.style.removeProperty('margin-top');
      element.style.removeProperty('margin-bottom');
      element.style.removeProperty('overflow');
      element.style.removeProperty('transition-duration');
      element.style.removeProperty('transition-property');
      if (callback) callback();
    }, duration);
  }

  const mainNav = document.getElementById('main-nav');
  const firstChild = mainNav.children[0];
  firstChild.insertAdjacentHTML('afterbegin', makeTree(menudata,relPath,true)); // eslint-disable-line no-undef
  
  const searchBoxPos2 = document.getElementById('searchBoxPos2');
  const searchBoxContents = searchBoxPos2 ? searchBoxPos2.innerHTML : '';
  
  const mainMenuState = document.getElementById('main-menu-state');
  let prevWidth = 0;
  const initResizableIfExists = function() {
    if (typeof initResizableFunc==='function') initResizableFunc(treeview); // eslint-disable-line no-undef
  }
  if (mainMenuState) {
    
    // animate mobile menu
    mainMenuState.addEventListener('change', function() {
      const menu = document.getElementById('main-menu');
      if (this.checked) {
        slideDown(menu, 250, () => {
          initResizableIfExists();
        });
      } else {
        slideUp(menu, 250, () => {
          initResizableIfExists();
        });
      }
    });
    
    // set default menu visibility
    const resetState = function() {
      const menu = document.getElementById('main-menu');
      const newWidth = window.innerWidth;
      if (newWidth !== prevWidth) {
        const searchBoxPos1 = document.getElementById('searchBoxPos1');
        const searchBoxPos2 = document.getElementById('searchBoxPos2');
        if (window.innerWidth < 768) {
          mainMenuState.checked = false;
          menu.style.display = 'none';
          searchBoxPos2.innerHTML = '';
          searchBoxPos2.style.display = 'none';
          searchBoxPos1.innerHTML = searchBoxContents;
          searchBoxPos1.style.display = 'block';
        } else {
          menu.style.display = 'block';
          searchBoxPos1.innerHTML = '';
          searchBoxPos1.style.display = 'none';
          searchBoxPos2.innerHTML = searchBoxContents;
          searchBoxPos2.style.display = 'block';
        }
        if (typeof searchBox !== 'undefined') {
          searchBox.CloseResultsWindow(); // eslint-disable-line no-undef
        }
        prevWidth = newWidth;
      }
    }
    
    window.addEventListener('load', function() {
      resetState();
      initResizableIfExists();
    });
    window.addEventListener('resize', resetState);
  } else {
    initResizableIfExists();
  }
  
  // Initialize smartmenu-like dropdown behavior
  initSmartMenus();
}

function initSmartMenus() {
  const mainMenu = document.getElementById('main-menu');
  if (!mainMenu) return;

  // Get all menu items with submenus
  const menuItems = mainMenu.querySelectorAll('li');
  
  menuItems.forEach(function(item) {
    const submenu = item.querySelector('ul');
    if (submenu) {
      const link = item.querySelector('a');
      
      // Add submenu indicator
      if (link && !link.querySelector('.sub-arrow')) {
        const arrow = document.createElement('span');
        arrow.className = 'sub-arrow';
        link.appendChild(arrow);
      }
      
      // Add has-submenu class to the link
      if (link) {
        link.classList.add('has-submenu');
      }
      
      // Desktop: Show submenu on hover
      item.addEventListener('mouseenter', function() {
        if (window.innerWidth >= 768) {
          submenu.style.display = 'block';
        }
      });
      
      item.addEventListener('mouseleave', function() {
        if (window.innerWidth >= 768) {
          submenu.style.display = 'none';
        }
      });
      
      // Mobile: Toggle on click
      if (link) {
        link.addEventListener('click', function(e) {
          if (window.innerWidth < 768 && submenu) {
            e.preventDefault();
            const isVisible = submenu.style.display === 'block';
            submenu.style.display = isVisible ? 'none' : 'block';
          }
        });
      }
    }
  });
  
  // Handle keyboard navigation
  mainMenu.addEventListener('keydown', function(e) {
    const target = e.target;
    if (target.tagName !== 'A') return;
    
    const item = target.parentElement;
    const submenu = item.querySelector('ul');
    
    // Arrow right: open submenu
    if (e.key === 'ArrowRight' && submenu) {
      e.preventDefault();
      submenu.style.display = 'block';
      const firstLink = submenu.querySelector('a');
      if (firstLink) firstLink.focus();
    }
    
    // Arrow left: close submenu and go back
    if (e.key === 'ArrowLeft') {
      e.preventDefault();
      const parentItem = item.parentElement.closest('li');
      if (parentItem) {
        const parentSubmenu = item.parentElement;
        parentSubmenu.style.display = 'none';
        const parentLink = parentItem.querySelector('a');
        if (parentLink) parentLink.focus();
      }
    }
    
    // Arrow down: next item
    if (e.key === 'ArrowDown') {
      e.preventDefault();
      const nextItem = item.nextElementSibling;
      if (nextItem) {
        const nextLink = nextItem.querySelector('a');
        if (nextLink) nextLink.focus();
      }
    }
    
    // Arrow up: previous item
    if (e.key === 'ArrowUp') {
      e.preventDefault();
      const prevItem = item.previousElementSibling;
      if (prevItem) {
        const prevLink = prevItem.querySelector('a');
        if (prevLink) prevLink.focus();
      }
    }
    
    // Escape: close all submenus
    if (e.key === 'Escape') {
      const openSubmenus = mainMenu.querySelectorAll('ul[style*="display: block"]');
      openSubmenus.forEach(function(submenu) {
        submenu.style.display = 'none';
      });
    }
  });
}
/* @license-end */
