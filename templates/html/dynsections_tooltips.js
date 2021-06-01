$(document).ready(function() {
  $('.code,.codeRef').each(function() {
    $(this).data('powertip',$('#a'+$(this).attr('href').replace(/.*\//,'').replace(/[^a-z_A-Z0-9]/g,'_')).html());
    $(this).powerTip({ placement: 's', smartPlacement: true, mouseOnToPopup: true });
  });
});
