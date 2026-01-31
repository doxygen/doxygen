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

  // Helper function for slideDown animation
  function slideDown(element, duration, callback) {
    if (element.dataset.animating) return;
    element.dataset.animating = 'true';
    
    element.style.removeProperty('display');
    let display = window.getComputedStyle(element).display;
    if (display === 'none') display = 'block';
    element.style.display = display;
    const height = element.offsetHeight;
    element.style.overflow = 'hidden';
    element.style.height = 0;
    element.style.paddingTop = 0;
    element.style.paddingBottom = 0;
    element.style.marginTop = 0;
    element.style.marginBottom = 0;
    element.offsetHeight; // force reflow
    element.style.transitionProperty = 'height, margin, padding';
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
      delete element.dataset.animating;
      if (callback) callback();
    }, duration);
  }

  // Helper function for slideUp animation
  function slideUp(element, duration, callback) {
    if (element.dataset.animating) return;
    element.dataset.animating = 'true';
    
    element.style.transitionProperty = 'height, margin, padding';
    element.style.transitionDuration = duration + 'ms';
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
      delete element.dataset.animating;
      if (callback) callback();
    }, duration);
  }

  const mainNav = document.getElementById('main-nav');
  if (mainNav && mainNav.children.length > 0) {
    const firstChild = mainNav.children[0];
    firstChild.insertAdjacentHTML('afterbegin', makeTree(menudata, relPath, true));
  }

  const searchBoxPos2 = document.getElementById('searchBoxPos2');
  let searchBoxContents = searchBoxPos2 ? searchBoxPos2.innerHTML : '';
  const mainMenuState = document.getElementById('main-menu-state');
  let prevWidth = 0;
  
  const initResizableIfExists = function() {
    if (typeof initResizableFunc === 'function') initResizableFunc(treeview);
  }

  if (mainMenuState) {
    const mainMenu = document.getElementById('main-menu');
    const searchBoxPos1 = document.getElementById('searchBoxPos1');

    // animate mobile menu
    mainMenuState.addEventListener('change', function() {
      if (this.checked) {
        slideDown(mainMenu, 250, () => {
          mainMenu.style.display = 'block';
          initResizableIfExists();
        });
      } else {
        slideUp(mainMenu, 250, () => {
          mainMenu.style.display = 'none';
        });
      }
    });

    // set default menu visibility
    const resetState = function() {
      const newWidth = window.innerWidth;
      if (newWidth !== prevWidth) {
        if (newWidth < 768) {
          mainMenuState.checked = false;
          mainMenu.style.display = 'none';
          if (searchBoxPos2) {
            searchBoxPos2.innerHTML = '';
            searchBoxPos2.style.display = 'none';
          }
          if (searchBoxPos1) {
            searchBoxPos1.innerHTML = searchBoxContents;
            searchBoxPos1.style.display = '';
          }
        } else {
          mainMenu.style.display = '';
          if (searchBoxPos1) {
            searchBoxPos1.innerHTML = '';
            searchBoxPos1.style.display = 'none';
          }
          if (searchBoxPos2) {
            searchBoxPos2.innerHTML = searchBoxContents;
            searchBoxPos2.style.display = '';
          }
        }
        if (typeof searchBox !== 'undefined') {
          searchBox.CloseResultsWindow();
        }
        prevWidth = newWidth;
      }
    }

    if (document.readyState === 'loading') {
      document.addEventListener('DOMContentLoaded', function() {
        resetState();
        initResizableIfExists();
      });
    } else {
      resetState();
      initResizableIfExists();
    }
    window.addEventListener('resize', resetState);
  } else {
    initResizableIfExists();
  }

  // Initialize dropdown menu behavior (replaces smartmenus)
  initDropdownMenu();
}

// Dropdown menu functionality to replace smartmenus
function initDropdownMenu() {
  const mainMenu = document.getElementById('main-menu');
  if (!mainMenu) return;

  const menuItems = mainMenu.querySelectorAll('li');
  let isMobile = () => window.innerWidth < 768;
  
  menuItems.forEach(item => {
    const submenu = item.querySelector('ul');
    if (submenu) {
      const link = item.querySelector('a');
      if (link) {
        // Add class and ARIA attributes for accessibility
        link.classList.add('has-submenu');
        link.setAttribute('aria-haspopup', 'true');
        link.setAttribute('aria-expanded', 'false');
        
        // Add sub-arrow indicator
        const span = document.createElement('span');
        span.classList.add('sub-arrow');
        link.append(span);
        
        // Check if this is a level 2+ submenu (nested within another dropdown)
        const isNestedSubmenu = item.parentElement && item.parentElement.id !== 'main-menu';
        
        // Desktop: show on hover
        item.addEventListener('mouseenter', function() {
          if (!isMobile()) {
            submenu.style.display = 'block';
            // Only apply marginLeft for nested submenus (level 2+)
            if (isNestedSubmenu) {
              submenu.style.marginLeft = link.offsetWidth + 'px';
            }
            link.setAttribute('aria-expanded', 'true');
          }
        });
        
        item.addEventListener('mouseleave', function() {
          if (!isMobile()) {
            submenu.style.display = 'none';
            link.setAttribute('aria-expanded', 'false');
          }
        });

        // Mobile/Touch: toggle on click
        link.addEventListener('click', function(e) {
          if (isMobile()) {
            e.preventDefault();
            const isExpanded = link.getAttribute('aria-expanded') === 'true';
            if (isExpanded) {
              submenu.style.display = 'none';
              link.setAttribute('aria-expanded', 'false');
            } else {
              submenu.style.display = 'block';
              // Only apply marginLeft for nested submenus (level 2+)
              if (isNestedSubmenu) {
                submenu.style.marginLeft = link.offsetWidth + 'px';
              }
              link.setAttribute('aria-expanded', 'true');
            }
          }
        });

        // Keyboard navigation
        // On desktop: Enter/Space follows link, Escape closes dropdown
        // On mobile: Enter/Space toggles dropdown, Escape closes it
        link.addEventListener('keydown', function(e) {
          if (e.key === 'Enter' || e.key === ' ') {
            if (isMobile()) {
              // Mobile: toggle dropdown
              e.preventDefault();
              const isExpanded = link.getAttribute('aria-expanded') === 'true';
              if (isExpanded) {
                submenu.style.display = 'none';
                link.setAttribute('aria-expanded', 'false');
              } else {
                submenu.style.display = 'block';
                // Only apply marginLeft for nested submenus (level 2+)
                if (isNestedSubmenu) {
                  submenu.style.marginLeft = link.offsetWidth + 'px';
                }
                link.setAttribute('aria-expanded', 'true');
              }
            }
            // Desktop: let the link navigate normally, but show submenu on focus
            else if (e.key === ' ') {
              // Space bar: toggle dropdown without following link
              e.preventDefault();
              const isExpanded = link.getAttribute('aria-expanded') === 'true';
              if (isExpanded) {
                submenu.style.display = 'none';
                link.setAttribute('aria-expanded', 'false');
              } else {
                submenu.style.display = 'block';
                // Only apply marginLeft for nested submenus (level 2+)
                if (isNestedSubmenu) {
                  submenu.style.marginLeft = link.offsetWidth + 'px';
                }
                link.setAttribute('aria-expanded', 'true');
              }
            }
          } else if (e.key === 'Escape') {
            submenu.style.display = 'none';
            link.setAttribute('aria-expanded', 'false');
            link.focus();
          }
        });
      }
    }
  });
}
/* @license-end */
