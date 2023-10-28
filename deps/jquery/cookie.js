/*!
 Cookie helper functions
 Copyright (c) 2023 Dimitri van Heesch
 Released under MIT license.
*/
var Cookie =
{
  cookie_namespace: 'doxygen_',

  readSetting: function(cookie,defVal) {
    if (window.chrome) {
      var val = localStorage.getItem(this.cookie_namespace+cookie);
      if (val) return val;
    } else {
      var myCookie = this.cookie_namespace+cookie+"=";
      if (document.cookie) {
        var index = document.cookie.indexOf(myCookie);
        if (index != -1) {
          var valStart = index + myCookie.length;
          var valEnd = document.cookie.indexOf(";", valStart);
          if (valEnd == -1) {
            valEnd = document.cookie.length;
          }
          var val = document.cookie.substring(valStart, valEnd);
          return val;
        }
      }
    }
    return defVal;
  },

  writeSetting: function(cookie,val,days=10*365) { // 10 years -> forever
    if (window.chrome) {
      localStorage.setItem(this.cookie_namespace+cookie,val);
    } else {
      var date = new Date();
      date.setTime(date.getTime()+(days*24*60*60*1000));
      expiration = date.toGMTString();
      document.cookie = this.cookie_namespace + cookie + "=" +
                        val + "; SameSite=Lax; expires=" + expiration+"; path=/";
    }
  },

  eraseSetting: function(cookie) {
    if (window.chrome) {
      var val = localStorage.removeItem(this.cookie_namespace+cookie);
      if (val) return val;
    } else {
      this.writeSetting(cookie,'',-1);
    }
  },
}
