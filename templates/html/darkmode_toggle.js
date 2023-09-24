/**

The code below is based on the Doxygen Awesome project with some minor modifications
https://github.com/jothepro/doxygen-awesome-css

MIT License

Copyright (c) 2021 - 2022 jothepro

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

class DarkModeToggle extends HTMLElement {
    static icon = '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" height="1em" width="1em"><g fill="none" fill-rule="evenodd"><path d="M0 0h24v24H0z"></path><rect width="1" height="3" x="12" fill="currentColor" rx=".5"></rect><rect width="1" height="3" x="12" y="21" fill="currentColor" rx=".5"></rect><rect width="1" height="3" x="22" y="10.5" fill="currentColor" rx=".5" transform="rotate(90 22.5 12)"></rect><rect width="1" height="3" x="1" y="10.5" fill="currentColor" rx=".5" transform="rotate(90 1.5 12)"></rect><rect width="1" height="3" x="19" y="3" fill="currentColor" rx=".5" transform="rotate(-135 19.5 4.5)"></rect><rect width="1" height="3" x="19" y="18" fill="currentColor" rx=".5" transform="rotate(135 19.5 19.5)"></rect><rect width="1" height="3" x="4" y="3" fill="currentColor" rx=".5" transform="scale(1 -1) rotate(45 15.37 0)"></rect><rect width="1" height="3" x="4" y="18" fill="currentColor" rx=".5" transform="scale(1 -1) rotate(-45 -42.57 0)"></rect><circle cx="12" cy="12" r="6.5" stroke="currentColor"></circle><path fill="currentColor" stroke="currentColor" d="M12.5 18.48V5.52a6.5 6.5 0 010 12.96z"></path></g></svg>';
    static title = "Toggle Light/Dark Mode"

    static prefersLightModeInDarkModeKey = "prefers-light-mode-in-dark-mode"
    static prefersDarkModeInLightModeKey = "prefers-dark-mode-in-light-mode"

    static _staticConstructor = function() {
        DarkModeToggle.enableDarkMode(DarkModeToggle.userPreference)
        // Update the color scheme when the browsers preference changes
        // without user interaction on the website.
        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', event => {
            DarkModeToggle.onSystemPreferenceChanged()
        })
        // Update the color scheme when the tab is made visible again.
        // It is possible that the appearance was changed in another tab 
        // while this tab was in the background.
        document.addEventListener("visibilitychange", visibilityState => {
            if (document.visibilityState === 'visible') {
                DarkModeToggle.onSystemPreferenceChanged()
            }
        });
    }()

    static init() {
        $(function() {
            $(document).ready(function() {
                const toggleButton = document.createElement('dark-mode-toggle')
                toggleButton.title = DarkModeToggle.title
                toggleButton.innerHTML = DarkModeToggle.icon
                toggleButton.tabIndex = 0;

                function addButton() {
                  var titleArea = document.getElementById("titlearea");
                  var searchBox = document.getElementById("MSearchBox");
                  var mainMenu  = document.getElementById("main-menu");
                  var navRow1   = document.getElementById("navrow1");
                  var mainMenuVisible = false;
                  if (mainMenu) {
                    var menuStyle = window.getComputedStyle(mainMenu);
                    mainMenuVisible = menuStyle.display!=='none'
                  }
                  var searchBoxPos1 = document.getElementById("searchBoxPos1");
                  if (searchBox) { // (1) search box visible
                    searchBox.parentNode.appendChild(toggleButton)
                  } else if (navRow1) { // (2) no search box, static menu bar
                    var li = document.createElement('li');
                    li.style = 'float: right;'
                    li.appendChild(toggleButton);
                    toggleButton.style = 'width: 24px; height: 25px; padding-top: 11px; float: right;';
                    var row = document.querySelector('#navrow1 > ul:first-of-type');
                    row.appendChild(li)
                  } else if (mainMenu && mainMenuVisible) { // (3) no search box + dynamic menu bar expanded
                    var li = document.createElement('li');
                    li.style = 'float: right;'
                    li.appendChild(toggleButton);
                    toggleButton.style = 'width: 14px; height: 36px; padding-top: 10px; float: right;';
                    mainMenu.appendChild(li)
                  } else if (searchBoxPos1) { // (4) no search box + dynamic menu bar collapsed
                    toggleButton.style = 'width: 24px; height: 36px; padding-top: 10px; float: right;';
                    searchBoxPos1.style = 'top: 0px;'
                    searchBoxPos1.appendChild(toggleButton);
                  } else if (titleArea) { // (5) no search box and no navigation tabs
                    toggleButton.style = 'width: 24px; height: 24px; position: absolute; right: 0px; top: 34px;';
                    titleArea.append(toggleButton);
                  }
                }

                $(document).ready(function(){
                    addButton();
                })
                $(window).resize(function(){
                    addButton();
                })
                var inFocus = false;
                $(document).focusin(function(e) {
                    inFocus = true;
                })
                $(document).focusout(function(e) {
                    inFocus = false;
                })
                $(document).keyup(function(e) {
                    if (e.keyCode==27 && !inFocus) { // escape key maps to keycode `27`
                       e.stopPropagation();
                       DarkModeToggle.userPreference = !DarkModeToggle.userPreference
                   }
                })
                DarkModeToggle.setDarkModeVisibility(DarkModeToggle.darkModeEnabled)
            })
        })
    }

    constructor() {
        super();
        this.onclick=this.toggleDarkMode
        this.onkeypress=function(e){if (e.keyCode==13) { this.toggleDarkMode(); }};
    }

    static createCookie(name, value, days) {
        if (days) {
            var date = new Date();
            date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
            var expires = "; expires=" + date.toGMTString();
        }
        else var expires = "";

        document.cookie = name + "=" + value + expires + "; path=/";
    }

    static readCookie(name) {
        var nameEQ = name + "=";
        var ca = document.cookie.split(';');
        for (var i = 0; i < ca.length; i++) {
            var c = ca[i];
            while (c.charAt(0) == ' ') c = c.substring(1, c.length);
            if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
        }
        return null;
    }

    static eraseCookie(name) {
        DarkModeToggle.createCookie(name, "", -1);
    }

    /**
     * @returns `true` for dark-mode, `false` for light-mode system preference
     */
    static get systemPreference() {
        return window.matchMedia('(prefers-color-scheme: dark)').matches
    }

    static get prefersDarkModeInLightMode() {
        if (window.chrome) { // Chrome supports localStorage in combination with file:// but not cookies
          return localStorage.getItem(DarkModeToggle.prefersDarkModeInLightModeKey)
        } else { // Other browsers support cookies in combination with file:// but not localStorage
          return DarkModeToggle.readCookie('doxygen_prefers_dark')=='1'
        }
    }

    static set prefersDarkModeInLightMode(preference) {
        if (window.chrome) {
            if (preference) {
                localStorage.setItem(DarkModeToggle.prefersDarkModeInLightModeKey, true)
            } else {
                localStorage.removeItem(DarkModeToggle.prefersDarkModeInLightModeKey)
            }
        } else {
            if (preference) {
               DarkModeToggle.createCookie('doxygen_prefers_dark','1',365)
            } else {
               DarkModeToggle.eraseCookie('doxygen_prefers_dark')
            }
        }
    }

    static get prefersLightModeInDarkMode() {
        if (window.chrome) { // Chrome supports localStorage in combination with file:// but not cookies
          return localStorage.getItem(DarkModeToggle.prefersLightModeInDarkModeKey)
        } else { // Other browsers support cookies in combination with file:// but not localStorage
          return DarkModeToggle.readCookie('doxygen_prefers_light')=='1'
        }
    }

    static set prefersLightModeInDarkMode(preference) {
        if (window.chrome) {
            if (preference) {
                localStorage.setItem(DarkModeToggle.prefersLightModeInDarkModeKey, true)
            } else {
                localStorage.removeItem(DarkModeToggle.prefersLightModeInDarkModeKey)
            }
        } else {
            if (preference) {
               DarkModeToggle.createCookie('doxygen_prefers_light','1',365)
            } else {
               DarkModeToggle.eraseCookie('doxygen_prefers_light')
            }
        }
    }


    /**
     * @returns `true` for dark-mode, `false` for light-mode user preference
     */
    static get userPreference() {
        return (!DarkModeToggle.systemPreference && DarkModeToggle.prefersDarkModeInLightMode) ||
        (DarkModeToggle.systemPreference && !DarkModeToggle.prefersLightModeInDarkMode)
    }

    static set userPreference(userPreference) {
        DarkModeToggle.darkModeEnabled = userPreference
        if (!userPreference) {
            if (DarkModeToggle.systemPreference) {
                DarkModeToggle.prefersLightModeInDarkMode = true
            } else {
                DarkModeToggle.prefersDarkModeInLightMode = false
            }
        } else {
            if (!DarkModeToggle.systemPreference) {
                DarkModeToggle.prefersDarkModeInLightMode = true
            } else {
                DarkModeToggle.prefersLightModeInDarkMode = false
            }
        }
        DarkModeToggle.onUserPreferenceChanged()
    }

    static setDarkModeVisibility(enable) {
        var darkModeStyle, lightModeStyle;
        if(enable) {
          darkModeStyle  = 'inline-block';
          lightModeStyle = 'none'
        } else {
          darkModeStyle  = 'none';
          lightModeStyle = 'inline-block'
        }
        document.querySelectorAll('.dark-mode-visible').forEach(function(el) {
            el.style.display = darkModeStyle;
        });
        document.querySelectorAll('.light-mode-visible').forEach(function(el) {
            el.style.display = lightModeStyle;
        });
    }
    static enableDarkMode(enable) {
        if(enable) {
            DarkModeToggle.darkModeEnabled = true
            document.documentElement.classList.add("dark-mode")
            document.documentElement.classList.remove("light-mode")
        } else {
            DarkModeToggle.darkModeEnabled = false
            document.documentElement.classList.remove("dark-mode")
            document.documentElement.classList.add("light-mode")
        }
        DarkModeToggle.setDarkModeVisibility(enable)
    }

    static onSystemPreferenceChanged() {
        DarkModeToggle.darkModeEnabled = DarkModeToggle.userPreference
        DarkModeToggle.enableDarkMode(DarkModeToggle.darkModeEnabled)
    }

    static onUserPreferenceChanged() {
        DarkModeToggle.enableDarkMode(DarkModeToggle.darkModeEnabled)
    }

    toggleDarkMode() {
        DarkModeToggle.userPreference = !DarkModeToggle.userPreference
    }
}

customElements.define("dark-mode-toggle", DarkModeToggle);

DarkModeToggle.init();
